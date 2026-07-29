#include "../../Service/STD_Types.h"
#include "../../Service/Bit_Math.h"
#include "../../MCL/GPIO/gpio_interface.h"
#include "../../MCL/Timer/timer_registers.h"
#include "servo_motor.h"

/*
 * The pulse timing is derived from F_CPU. The Makefile passes -DF_CPU; this
 * fallback only keeps the file compilable on its own.
 */
#ifndef F_CPU
#define F_CPU 8000000UL
#endif

/* ================================================================================
 *  RC SERVO DRIVER - IMPLEMENTATION (HAL)
 *  ------------------------------------------------------------------------------
 *  Each body keeps the ordered steps it implements as comments, followed by the
 *  actual register / GPIO work.
 *
 *  HARDWARE PATH (Timer1, Fast PWM mode 14, TOP = ICR1, prescaler 8)
 *      1 timer tick = 8 / F_CPU seconds  ->  0.5 us at 16 MHz, 1 us at 8 MHz.
 *      TOP is set so one frame is exactly 20 ms, and OCR1A/OCR1B carry the pulse
 *      width. Non-inverting compare output means the pin is HIGH from the start
 *      of the frame until the compare match - which is exactly the servo pulse.
 *
 *  SOFTWARE PATH
 *      The pin is raised, held for the pulse width by a cycle-counted busy loop,
 *      then dropped. The 20 ms spacing comes from how often the application calls
 *      Servo_Motor_SoftwareRefresh().
 * ============================================================================== */

/* Timer1 ticks in one millisecond at the configured clock (prescaler 8). */
#define SERVO_TICKS_PER_MS     ((uint32_h)((F_CPU) / 8UL / 1000UL))

/* TOP value that makes one Fast-PWM frame last SERVO_MOTOR_FRAME_MS. */
#define SERVO_TIMER1_TOP       ((uint16_h)((SERVO_TICKS_PER_MS * (uint32_h)SERVO_MOTOR_FRAME_MS) - 1UL))

/* Pins the silicon hard-wires to the Timer1 compare outputs. */
#define SERVO_OC1A_PORT        GPIO_PORTD
#define SERVO_OC1A_PIN         GPIO_PIN5
#define SERVO_OC1B_PORT        GPIO_PORTD
#define SERVO_OC1B_PIN         GPIO_PIN4


/* --------------------------------------------------------------------------
 *  DRIVER-PRIVATE STATE
 *  Only the shared peripherals live here. Everything that belongs to one servo
 *  lives in that servo's handle, which is what makes several servos possible.
 * ------------------------------------------------------------------------ */

/* Registered software servos; a NULL slot is free. */
static Servo_MotorHandleType *Servo_SoftList[SERVO_MOTOR_MAX_SOFTWARE];

/* How many hardware servos are using Timer1 right now (0, 1 or 2). */
static uint8_h Servo_HardwareCount = 0U;

/* 1 once Timer1 has been programmed for the 50 Hz frame. */
static uint8_h Servo_Timer1Ready = 0U;


/* --------------------------------------------------------------------------
 *  INTERNAL HELPERS (static - not part of the public interface)
 * ------------------------------------------------------------------------ */

/*
 * Cycle-counted microsecond delay for the software path.
 *
 * The two-instruction loop below costs exactly 4 CPU cycles per iteration
 * (sbiw = 2, taken brne = 2), so:
 *
 *      loops = us * F_CPU / 4 000 000
 *
 * At 16 MHz that is 4 loops per microsecond, which puts the pulse resolution
 * well under the ~10 us a servo can actually resolve.
 */
static void Servo_DelayUs(uint16_h us)
{
    uint16_h local_Loops = 0U;

    local_Loops = (uint16_h)(((uint32_h)us * ((uint32_h)F_CPU / 1000UL)) / 4000UL);

    if (local_Loops == 0U)
    {
        return;      /* a zero counter would wrap and delay for 65536 loops */
    }

    __asm__ __volatile__
    (
        "1: sbiw %0, 1"  "\n\t"
        "   brne 1b"
        : "+w" (local_Loops)
    );
}

/* Converts a pulse width in microseconds into Timer1 ticks. */
static uint16_h Servo_UsToTicks(uint16_h us)
{
    return (uint16_h)(((uint32_h)us * SERVO_TICKS_PER_MS) / 1000UL);
}

/*
 * Programs Timer1 for the servo frame - once, no matter how many hardware
 * servos ask for it. Fast PWM mode 14: WGM13:WGM10 = 1110, TOP = ICR1.
 */
static void Servo_Timer1Setup(void)
{
    if (Servo_Timer1Ready != 0U)
    {
        return;
    }

    TIMER_TCCR1A_REG = (uint8_h)(1U << TIMER_WGM11_BIT);                    /* WGM11 = 1, no output connected yet */
    TIMER_TCCR1B_REG = (uint8_h)((1U << TIMER_WGM13_BIT) |
                                 (1U << TIMER_WGM12_BIT) |
                                 (1U << TIMER_CS11_BIT));                  /* mode 14, prescaler 8               */
    TIMER_ICR1_REG   = SERVO_TIMER1_TOP;                                   /* TOP -> 20 ms frame                 */
    TIMER_TCNT1_REG  = 0U;

    Servo_Timer1Ready = 1U;
}

/* Maps an angle onto a pulse width using this servo's calibration. */
static uint16_h Servo_AngleToPulse(const Servo_MotorHandleType *handle, uint8_h angle)
{
    uint32_h local_Span = (uint32_h)(handle->maxPulseUs - handle->minPulseUs);

    return (uint16_h)((uint32_h)handle->minPulseUs +
                      (((uint32_h)angle * local_Span) / (uint32_h)handle->maxAngle));
}

/* Pushes the handle's current pulse width out to the hardware it belongs to. */
static void Servo_ApplyPulse(Servo_MotorHandleType *handle)
{
    switch (handle->drive)
    {
        case SERVO_MOTOR_DRIVE_TIMER1_OC1A:
            TIMER_OCR1A_REG = Servo_UsToTicks(handle->currentPulseUs);
            break;

        case SERVO_MOTOR_DRIVE_TIMER1_OC1B:
            TIMER_OCR1B_REG = Servo_UsToTicks(handle->currentPulseUs);
            break;

        case SERVO_MOTOR_DRIVE_SOFTWARE:
        default:
            /* Nothing to write: the value is used by the next refresh. */
            break;
    }
}


/* --------------------------------------------------------------------------
 *  PUBLIC FUNCTIONS
 * ------------------------------------------------------------------------ */

STD_ReturnType Servo_Motor_Init(Servo_MotorHandleType *handle)
{
    uint8_h local_Slot  = 0U;
    uint8_h local_Found = 0U;

    /* STEP 1: Validate the handle and the calibration it carries. */
    if (handle == NULL)
    {
        return E_NOK;
    }

    if (handle->drive > SERVO_MOTOR_DRIVE_SOFTWARE)
    {
        return E_NOK;
    }

    if ((handle->maxAngle == 0U) || (handle->minPulseUs >= handle->maxPulseUs))
    {
        return E_NOK;
    }

    handle->initialized = 0U;
    handle->running     = 0U;

    /* STEP 2: Park the servo at the centre of its travel before anything moves. */
    handle->currentAngle   = (uint8_h)(handle->maxAngle / 2U);
    handle->currentPulseUs = Servo_AngleToPulse(handle, handle->currentAngle);

    /* STEP 3: Bring up whichever back end this servo uses. */
    if (handle->drive == SERVO_MOTOR_DRIVE_SOFTWARE)
    {
        /* 3a: Software - the pin must be a real, in-range output. */
        if (handle->port >= GPIO_NUMBER_OF_PORTS)
        {
            return E_NOK;
        }

        (void)GPIO_SetPinDirection(handle->port, handle->pin, GPIO_OUTPUT);
        (void)GPIO_SetPinValue(handle->port, handle->pin, PIN_LOW);

        /* 3b: Take a free slot in the refresh list; refuse if the list is full. */
        for (local_Slot = 0U; local_Slot < SERVO_MOTOR_MAX_SOFTWARE; local_Slot++)
        {
            if (Servo_SoftList[local_Slot] == NULL)
            {
                Servo_SoftList[local_Slot] = handle;
                local_Found = 1U;
                break;
            }
        }

        if (local_Found == 0U)
        {
            return E_NOK;   /* raise SERVO_MOTOR_MAX_SOFTWARE if you need more */
        }
    }
    else
    {
        /* 3c: Hardware - program the shared frame, then connect this channel. */
        Servo_Timer1Setup();

        if (handle->drive == SERVO_MOTOR_DRIVE_TIMER1_OC1A)
        {
            (void)GPIO_SetPinDirection(SERVO_OC1A_PORT, SERVO_OC1A_PIN, GPIO_OUTPUT);
            SET_BIT(TIMER_TCCR1A_REG, TIMER_COM1A1_BIT);   /* non-inverting on OC1A */
        }
        else
        {
            (void)GPIO_SetPinDirection(SERVO_OC1B_PORT, SERVO_OC1B_PIN, GPIO_OUTPUT);
            SET_BIT(TIMER_TCCR1A_REG, TIMER_COM1B1_BIT);   /* non-inverting on OC1B */
        }

        Servo_HardwareCount++;
    }

    /* STEP 4: Emit the centre position and mark the handle usable. */
    handle->initialized = 1U;
    handle->running     = 1U;
    Servo_ApplyPulse(handle);

    return E_OK;
}


STD_ReturnType Servo_Motor_SetAngle(Servo_MotorHandleType *handle, uint8_h angle)
{
    /* STEP 1: Validate the handle. */
    if ((handle == NULL) || (handle->initialized == 0U))
    {
        return E_NOK;
    }

    /* STEP 2: Refuse an angle the servo cannot reach. */
    if (angle > handle->maxAngle)
    {
        return E_NOK;
    }

    /* STEP 3: Map angle -> pulse width with this servo's own calibration. */
    handle->currentAngle   = angle;
    handle->currentPulseUs = Servo_AngleToPulse(handle, angle);

    /* STEP 4: Hand the new width to the hardware (or to the next refresh). */
    Servo_ApplyPulse(handle);

    return E_OK;
}


STD_ReturnType Servo_Motor_SetPulseUs(Servo_MotorHandleType *handle, uint16_h pulseUs)
{
    /* STEP 1: Validate the handle. */
    if ((handle == NULL) || (handle->initialized == 0U))
    {
        return E_NOK;
    }

    /*
     * STEP 2: Clamp into the calibrated range. Commanding a servo past its
     *         mechanical stop makes it stall against the end and strip gears,
     *         so the limits are enforced rather than reported.
     */
    if (pulseUs < handle->minPulseUs)
    {
        pulseUs = handle->minPulseUs;
    }
    else if (pulseUs > handle->maxPulseUs)
    {
        pulseUs = handle->maxPulseUs;
    }
    else
    {
        /* already inside the range */
    }

    handle->currentPulseUs = pulseUs;

    /* STEP 3: Keep the reported angle consistent with the width just set. */
    handle->currentAngle = (uint8_h)((((uint32_h)(pulseUs - handle->minPulseUs) *
                                       (uint32_h)handle->maxAngle) /
                                      (uint32_h)(handle->maxPulseUs - handle->minPulseUs)));

    /* STEP 4: Push it out. */
    Servo_ApplyPulse(handle);

    return E_OK;
}


STD_ReturnType Servo_Motor_GetAngle(const Servo_MotorHandleType *handle, uint8_h *pAngle)
{
    /* STEP 1: Validate the handle and the output pointer. */
    if ((handle == NULL) || (handle->initialized == 0U) || (pAngle == NULL))
    {
        return E_NOK;
    }

    /* STEP 2: Report the last COMMANDED angle - there is no feedback to read. */
    *pAngle = handle->currentAngle;

    return E_OK;
}


STD_ReturnType Servo_Motor_Stop(Servo_MotorHandleType *handle)
{
    /* STEP 1: Validate the handle. */
    if ((handle == NULL) || (handle->initialized == 0U))
    {
        return E_NOK;
    }

    /*
     * STEP 2: Hardware - disconnect the compare output so the pin stops pulsing,
     *         then drive it low. Timer1 keeps running for the other channel.
     */
    if (handle->drive == SERVO_MOTOR_DRIVE_TIMER1_OC1A)
    {
        CLR_BIT(TIMER_TCCR1A_REG, TIMER_COM1A1_BIT);
        (void)GPIO_SetPinValue(SERVO_OC1A_PORT, SERVO_OC1A_PIN, PIN_LOW);
    }
    else if (handle->drive == SERVO_MOTOR_DRIVE_TIMER1_OC1B)
    {
        CLR_BIT(TIMER_TCCR1A_REG, TIMER_COM1B1_BIT);
        (void)GPIO_SetPinValue(SERVO_OC1B_PORT, SERVO_OC1B_PIN, PIN_LOW);
    }
    else
    {
        /* STEP 3: Software - clearing 'running' makes the refresh skip it. */
        (void)GPIO_SetPinValue(handle->port, handle->pin, PIN_LOW);
    }

    /* STEP 4: No pulses = no holding torque. The servo goes limp. */
    handle->running = 0U;

    return E_OK;
}


STD_ReturnType Servo_Motor_Start(Servo_MotorHandleType *handle)
{
    /* STEP 1: Validate the handle. */
    if ((handle == NULL) || (handle->initialized == 0U))
    {
        return E_NOK;
    }

    /* STEP 2: Hardware - reconnect the compare output to the pin. */
    if (handle->drive == SERVO_MOTOR_DRIVE_TIMER1_OC1A)
    {
        SET_BIT(TIMER_TCCR1A_REG, TIMER_COM1A1_BIT);
    }
    else if (handle->drive == SERVO_MOTOR_DRIVE_TIMER1_OC1B)
    {
        SET_BIT(TIMER_TCCR1A_REG, TIMER_COM1B1_BIT);
    }
    else
    {
        /* Software needs nothing here - the refresh picks it up again. */
    }

    /* STEP 3: Re-command the last width and let it pulse again. */
    handle->running = 1U;
    Servo_ApplyPulse(handle);

    return E_OK;
}


STD_ReturnType Servo_Motor_SoftwareRefresh(void)
{
    uint8_h local_Slot   = 0U;
    uint8_h local_Served = 0U;
    Servo_MotorHandleType *local_Servo = NULL;

    /*
     * STEP 1: Walk the registered software servos and give each exactly one
     *         pulse. They are pulsed one after another, not at the same time,
     *         so the whole call costs about 1.5 ms per servo.
     */
    for (local_Slot = 0U; local_Slot < SERVO_MOTOR_MAX_SOFTWARE; local_Slot++)
    {
        local_Servo = Servo_SoftList[local_Slot];

        /* STEP 2: Skip empty slots and servos that were stopped. */
        if ((local_Servo == NULL) || (local_Servo->running == 0U))
        {
            continue;
        }

        /* STEP 3: The pulse itself - raise, hold for the width, drop. */
        (void)GPIO_SetPinValue(local_Servo->port, local_Servo->pin, PIN_HIGH);
        Servo_DelayUs(local_Servo->currentPulseUs);
        (void)GPIO_SetPinValue(local_Servo->port, local_Servo->pin, PIN_LOW);

        local_Served++;
    }

    /*
     * STEP 4: Report whether there was anything to do. The 20 ms gap between
     *         frames is the CALLER's job - this function only makes the pulses.
     */
    return (local_Served > 0U) ? E_OK : E_NOK;
}


STD_ReturnType Servo_Motor_DeInit(Servo_MotorHandleType *handle)
{
    uint8_h local_Slot = 0U;

    /* STEP 1: Validate the handle. */
    if ((handle == NULL) || (handle->initialized == 0U))
    {
        return E_NOK;
    }

    /* STEP 2: Stop the pulses first, so the pin is left in a safe state. */
    (void)Servo_Motor_Stop(handle);

    if (handle->drive == SERVO_MOTOR_DRIVE_SOFTWARE)
    {
        /* STEP 3a: Free the refresh slot so another servo can use it. */
        for (local_Slot = 0U; local_Slot < SERVO_MOTOR_MAX_SOFTWARE; local_Slot++)
        {
            if (Servo_SoftList[local_Slot] == handle)
            {
                Servo_SoftList[local_Slot] = NULL;
                break;
            }
        }
    }
    else
    {
        /*
         * STEP 3b: Release Timer1 only when the LAST hardware servo leaves -
         *          the other channel may still be driving a servo.
         */
        if (Servo_HardwareCount > 0U)
        {
            Servo_HardwareCount--;
        }

        if (Servo_HardwareCount == 0U)
        {
            TIMER_TCCR1A_REG  = 0U;
            TIMER_TCCR1B_REG  = 0U;
            Servo_Timer1Ready = 0U;
        }
    }

    /* STEP 4: The handle is no longer usable until Init() runs again. */
    handle->initialized = 0U;

    return E_OK;
}
