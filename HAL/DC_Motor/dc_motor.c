#include "../../Service/STD_Types.h"
#include "../../Service/Bit_Math.h"
#include "../../MCL/GPIO/gpio_interface.h"
#include "../../MCL/Timer/timer_registers.h"
#include "dc_motor.h"

/* ================================================================================
 *  BRUSHED DC MOTOR DRIVER - IMPLEMENTATION (HAL, H-bridge)
 *  ------------------------------------------------------------------------------
 *  Each body keeps the ordered steps it implements as comments, followed by the
 *  actual register / GPIO work. All per-motor state lives in the caller's handle,
 *  so several motors never interfere - only the shared timers do, which is why
 *  the header asks you to give each timer one owner.
 *
 *  PWM: 8-bit fast PWM, non-inverting, prescaler 64.
 *       At F_CPU = 16 MHz that is 16 MHz / 64 / 256 = ~976 Hz, which is above the
 *       audible whine of a slow PWM and slow enough that an L298 keeps up.
 * ============================================================================== */

/* Compare-output pins fixed by the silicon. */
#define DC_MOTOR_OC0_PORT     GPIO_PORTB
#define DC_MOTOR_OC0_PIN      GPIO_PIN3
#define DC_MOTOR_OC1A_PORT    GPIO_PORTD
#define DC_MOTOR_OC1A_PIN     GPIO_PIN5
#define DC_MOTOR_OC1B_PORT    GPIO_PORTD
#define DC_MOTOR_OC1B_PIN     GPIO_PIN4
#define DC_MOTOR_OC2_PORT     GPIO_PORTD
#define DC_MOTOR_OC2_PIN      GPIO_PIN7

#define DC_MOTOR_PWM_MAX      255U


/* --------------------------------------------------------------------------
 *  INTERNAL HELPERS (static - not part of the public interface)
 * ------------------------------------------------------------------------ */

/* The port/pin the chosen compare channel is wired to inside the chip. */
static void DC_Motor_PwmPin(DC_MotorPwmChannelType channel, uint8_h *pPort, uint8_h *pPin)
{
    switch (channel)
    {
        case DC_MOTOR_PWM_OC0:  *pPort = DC_MOTOR_OC0_PORT;  *pPin = DC_MOTOR_OC0_PIN;  break;
        case DC_MOTOR_PWM_OC1A: *pPort = DC_MOTOR_OC1A_PORT; *pPin = DC_MOTOR_OC1A_PIN; break;
        case DC_MOTOR_PWM_OC1B: *pPort = DC_MOTOR_OC1B_PORT; *pPin = DC_MOTOR_OC1B_PIN; break;
        case DC_MOTOR_PWM_OC2:  *pPort = DC_MOTOR_OC2_PORT;  *pPin = DC_MOTOR_OC2_PIN;  break;
        default:                *pPort = 0U;                 *pPin = 0U;                break;
    }
}

/*
 * Puts the timer behind the chosen channel into 8-bit fast PWM with a /64
 * prescaler. Timer0 and Timer2 own their whole control register, so those are
 * written outright; Timer1 is shared between OC1A and OC1B, so its bits are set
 * one at a time and the sibling channel's settings survive.
 */
static void DC_Motor_PwmTimerSetup(DC_MotorPwmChannelType channel)
{
    switch (channel)
    {
        case DC_MOTOR_PWM_OC0:
            /* Fast PWM (WGM01:WGM00 = 11), clk/64 (CS01:CS00 = 11). */
            TIMER_TCCR0_REG = (uint8_h)((1U << TIMER_WGM00_BIT) | (1U << TIMER_WGM01_BIT) |
                                        (1U << TIMER_CS01_BIT)  | (1U << TIMER_CS00_BIT));
            TIMER_OCR0_REG  = 0U;
            break;

        case DC_MOTOR_PWM_OC1A:
        case DC_MOTOR_PWM_OC1B:
            /* Fast PWM 8-bit (WGM12:WGM10 = 101), clk/64 (CS11:CS10 = 11). */
            SET_BIT(TIMER_TCCR1A_REG, TIMER_WGM10_BIT);
            SET_BIT(TIMER_TCCR1B_REG, TIMER_WGM12_BIT);
            SET_BIT(TIMER_TCCR1B_REG, TIMER_CS11_BIT);
            SET_BIT(TIMER_TCCR1B_REG, TIMER_CS10_BIT);
            break;

        case DC_MOTOR_PWM_OC2:
            /*
             * Fast PWM (WGM21:WGM20 = 11), clk/64.
             * Timer2 does NOT share Timer0's prescaler encoding: /64 is
             * CS22:CS20 = 100 here, not 011. Getting this wrong is a classic
             * way to end up with a motor whining at the wrong frequency.
             */
            TIMER_TCCR2_REG = (uint8_h)((1U << TIMER_WGM20_BIT) | (1U << TIMER_WGM21_BIT) |
                                        (1U << TIMER_CS22_BIT));
            TIMER_OCR2_REG  = 0U;
            break;

        default:
            /* DC_MOTOR_PWM_NONE - no timer involved. */
            break;
    }
}

/* Connects (enable != 0) or disconnects the compare output from its pin. */
static void DC_Motor_PwmConnect(DC_MotorPwmChannelType channel, uint8_h enable)
{
    switch (channel)
    {
        case DC_MOTOR_PWM_OC0:
            if (enable != 0U) { SET_BIT(TIMER_TCCR0_REG, TIMER_COM01_BIT); }
            else              { CLR_BIT(TIMER_TCCR0_REG, TIMER_COM01_BIT); }
            break;

        case DC_MOTOR_PWM_OC1A:
            if (enable != 0U) { SET_BIT(TIMER_TCCR1A_REG, TIMER_COM1A1_BIT); }
            else              { CLR_BIT(TIMER_TCCR1A_REG, TIMER_COM1A1_BIT); }
            break;

        case DC_MOTOR_PWM_OC1B:
            if (enable != 0U) { SET_BIT(TIMER_TCCR1A_REG, TIMER_COM1B1_BIT); }
            else              { CLR_BIT(TIMER_TCCR1A_REG, TIMER_COM1B1_BIT); }
            break;

        case DC_MOTOR_PWM_OC2:
            if (enable != 0U) { SET_BIT(TIMER_TCCR2_REG, TIMER_COM21_BIT); }
            else              { CLR_BIT(TIMER_TCCR2_REG, TIMER_COM21_BIT); }
            break;

        default:
            break;
    }
}

/* Writes the compare value that sets the duty cycle. */
static void DC_Motor_PwmSetDuty(DC_MotorPwmChannelType channel, uint8_h duty)
{
    switch (channel)
    {
        case DC_MOTOR_PWM_OC0:  TIMER_OCR0_REG  = duty;                break;
        case DC_MOTOR_PWM_OC1A: TIMER_OCR1A_REG = (uint16_h)duty;      break;
        case DC_MOTOR_PWM_OC1B: TIMER_OCR1B_REG = (uint16_h)duty;      break;
        case DC_MOTOR_PWM_OC2:  TIMER_OCR2_REG  = duty;                break;
        default:                                                       break;
    }
}

/*
 * Applies the handle's stored speed to its enable output.
 *
 * Speed 0 is handled specially: in non-inverting fast PWM a compare value of 0
 * still produces a one-tick pulse at the top of every period, so the output is
 * disconnected and driven low instead of merely set to 0 % duty.
 */
static void DC_Motor_ApplySpeed(const DC_MotorHandleType *handle)
{
    uint8_h local_Port = 0U;
    uint8_h local_Pin  = 0U;
    uint8_h local_Duty = 0U;

    if (handle->pwmChannel == DC_MOTOR_PWM_NONE)
    {
        /* No PWM: the enable is either on or off. */
        (void)GPIO_SetPinValue(handle->enPort, handle->enPin,
                               (handle->speedPercent > 0U) ? PIN_HIGH : PIN_LOW);
        return;
    }

    DC_Motor_PwmPin(handle->pwmChannel, &local_Port, &local_Pin);

    if (handle->speedPercent == 0U)
    {
        DC_Motor_PwmConnect(handle->pwmChannel, 0U);
        (void)GPIO_SetPinValue(local_Port, local_Pin, PIN_LOW);
        return;
    }

    /* percent -> 8-bit compare value, rounded down. */
    local_Duty = (uint8_h)(((uint16_h)handle->speedPercent * DC_MOTOR_PWM_MAX) / 100U);

    DC_Motor_PwmSetDuty(handle->pwmChannel, local_Duty);
    DC_Motor_PwmConnect(handle->pwmChannel, 1U);
}

/* Drives the two bridge inputs for one of the four bridge states. */
static void DC_Motor_ApplyState(DC_MotorHandleType *handle, DC_MotorStateType state)
{
    uint8_h local_In1 = PIN_LOW;
    uint8_h local_In2 = PIN_LOW;

    switch (state)
    {
        case DC_MOTOR_STATE_FORWARD:   local_In1 = PIN_HIGH; local_In2 = PIN_LOW;  break;
        case DC_MOTOR_STATE_BACKWARD:  local_In1 = PIN_LOW;  local_In2 = PIN_HIGH; break;
        case DC_MOTOR_STATE_BRAKE:     local_In1 = PIN_HIGH; local_In2 = PIN_HIGH; break;
        case DC_MOTOR_STATE_STOP:
        default:                       local_In1 = PIN_LOW;  local_In2 = PIN_LOW;  break;
    }

    /*
     * 'invertDirection' swaps the two drive states only. Stop and brake are
     * symmetrical, so flipping them would change nothing.
     */
    if ((handle->invertDirection != 0U) &&
        ((state == DC_MOTOR_STATE_FORWARD) || (state == DC_MOTOR_STATE_BACKWARD)))
    {
        uint8_h local_Swap = local_In1;
        local_In1 = local_In2;
        local_In2 = local_Swap;
    }

    (void)GPIO_SetPinValue(handle->in1Port, handle->in1Pin, local_In1);
    (void)GPIO_SetPinValue(handle->in2Port, handle->in2Pin, local_In2);

    handle->state = state;
}


/* --------------------------------------------------------------------------
 *  PUBLIC FUNCTIONS
 * ------------------------------------------------------------------------ */

STD_ReturnType DC_Motor_Init(DC_MotorHandleType *handle)
{
    uint8_h local_Port = 0U;
    uint8_h local_Pin  = 0U;

    /* STEP 1: Validate the handle, the bridge ports and the PWM selection. */
    if (handle == NULL)
    {
        return E_NOK;
    }

    if ((handle->in1Port >= GPIO_NUMBER_OF_PORTS) || (handle->in2Port >= GPIO_NUMBER_OF_PORTS))
    {
        return E_NOK;
    }

    if (handle->pwmChannel > DC_MOTOR_PWM_OC2)
    {
        return E_NOK;
    }

    /* STEP 2: Both bridge inputs are outputs, both low - the motor stays still. */
    (void)GPIO_SetPinDirection(handle->in1Port, handle->in1Pin, GPIO_OUTPUT);
    (void)GPIO_SetPinDirection(handle->in2Port, handle->in2Pin, GPIO_OUTPUT);
    (void)GPIO_SetPinValue(handle->in1Port, handle->in1Pin, PIN_LOW);
    (void)GPIO_SetPinValue(handle->in2Port, handle->in2Pin, PIN_LOW);

    /* STEP 3: Set up whichever enable this motor uses. */
    if (handle->pwmChannel == DC_MOTOR_PWM_NONE)
    {
        /* 3a: A plain GPIO enable - it must be a real port. */
        if (handle->enPort >= GPIO_NUMBER_OF_PORTS)
        {
            return E_NOK;
        }

        (void)GPIO_SetPinDirection(handle->enPort, handle->enPin, GPIO_OUTPUT);
        (void)GPIO_SetPinValue(handle->enPort, handle->enPin, PIN_LOW);
    }
    else
    {
        /*
         * 3b: A compare output. The pin is fixed by silicon, and it only drives
         *     the waveform once its data-direction bit is set to output.
         */
        DC_Motor_PwmPin(handle->pwmChannel, &local_Port, &local_Pin);
        (void)GPIO_SetPinDirection(local_Port, local_Pin, GPIO_OUTPUT);
        (void)GPIO_SetPinValue(local_Port, local_Pin, PIN_LOW);

        DC_Motor_PwmTimerSetup(handle->pwmChannel);
        DC_Motor_PwmConnect(handle->pwmChannel, 0U);   /* stays disconnected until a speed is set */
    }

    /* STEP 4: Start stopped, at zero speed. Nothing should spin on power-up. */
    handle->speedPercent = 0U;
    handle->initialized  = 1U;
    DC_Motor_ApplyState(handle, DC_MOTOR_STATE_STOP);
    DC_Motor_ApplySpeed(handle);

    return E_OK;
}


STD_ReturnType DC_Motor_SetSpeed(DC_MotorHandleType *handle, uint8_h speedPercent)
{
    /* STEP 1: Validate the handle. */
    if ((handle == NULL) || (handle->initialized == 0U))
    {
        return E_NOK;
    }

    /* STEP 2: Clamp to a sane percentage rather than rejecting the call. */
    if (speedPercent > 100U)
    {
        speedPercent = 100U;
    }

    handle->speedPercent = speedPercent;

    /* STEP 3: Push it to the enable output (PWM duty, or on/off). */
    DC_Motor_ApplySpeed(handle);

    return E_OK;
}


STD_ReturnType DC_Motor_Forward(DC_MotorHandleType *handle)
{
    /* STEP 1: Validate the handle. */
    if ((handle == NULL) || (handle->initialized == 0U))
    {
        return E_NOK;
    }

    /* STEP 2: IN1 high, IN2 low (swapped if invertDirection is set). */
    DC_Motor_ApplyState(handle, DC_MOTOR_STATE_FORWARD);

    /* STEP 3: Re-apply the speed - Stop() may have switched the enable off. */
    DC_Motor_ApplySpeed(handle);

    return E_OK;
}


STD_ReturnType DC_Motor_Backward(DC_MotorHandleType *handle)
{
    /* STEP 1: Validate the handle. */
    if ((handle == NULL) || (handle->initialized == 0U))
    {
        return E_NOK;
    }

    /* STEP 2: IN1 low, IN2 high (swapped if invertDirection is set). */
    DC_Motor_ApplyState(handle, DC_MOTOR_STATE_BACKWARD);

    /* STEP 3: Re-apply the speed. */
    DC_Motor_ApplySpeed(handle);

    return E_OK;
}


STD_ReturnType DC_Motor_SetDirection(DC_MotorHandleType *handle, DC_MotorDirectionType dir)
{
    /* STEP 1: Validate the handle and the direction. */
    if ((handle == NULL) || (handle->initialized == 0U))
    {
        return E_NOK;
    }

    if (dir > DC_MOTOR_DIR_BACKWARD)
    {
        return E_NOK;
    }

    /* STEP 2: Turn the direction into the matching bridge state. */
    return (dir == DC_MOTOR_DIR_FORWARD) ? DC_Motor_Forward(handle) : DC_Motor_Backward(handle);
}


STD_ReturnType DC_Motor_Stop(DC_MotorHandleType *handle)
{
    uint8_h local_Port = 0U;
    uint8_h local_Pin  = 0U;

    /* STEP 1: Validate the handle. */
    if ((handle == NULL) || (handle->initialized == 0U))
    {
        return E_NOK;
    }

    /* STEP 2: Both inputs low - the bridge stops driving and the motor coasts. */
    DC_Motor_ApplyState(handle, DC_MOTOR_STATE_STOP);

    /*
     * STEP 3: Switch the enable off as well, so nothing is driven at all. The
     *         stored speed is kept, so Forward() later resumes at that speed.
     */
    if (handle->pwmChannel == DC_MOTOR_PWM_NONE)
    {
        (void)GPIO_SetPinValue(handle->enPort, handle->enPin, PIN_LOW);
    }
    else
    {
        DC_Motor_PwmPin(handle->pwmChannel, &local_Port, &local_Pin);
        DC_Motor_PwmConnect(handle->pwmChannel, 0U);
        (void)GPIO_SetPinValue(local_Port, local_Pin, PIN_LOW);
    }

    return E_OK;
}


STD_ReturnType DC_Motor_Brake(DC_MotorHandleType *handle)
{
    /* STEP 1: Validate the handle. */
    if ((handle == NULL) || (handle->initialized == 0U))
    {
        return E_NOK;
    }

    /*
     * STEP 2: Both inputs high shorts the windings through the bridge. The
     *         motor's own back-EMF then brakes it.
     */
    DC_Motor_ApplyState(handle, DC_MOTOR_STATE_BRAKE);

    /*
     * STEP 3: Braking needs the bridge enabled, and it must be enabled FULLY -
     *         braking at a partial duty cycle would just chop the short.
     */
    if (handle->pwmChannel == DC_MOTOR_PWM_NONE)
    {
        (void)GPIO_SetPinValue(handle->enPort, handle->enPin, PIN_HIGH);
    }
    else
    {
        DC_Motor_PwmSetDuty(handle->pwmChannel, DC_MOTOR_PWM_MAX);
        DC_Motor_PwmConnect(handle->pwmChannel, 1U);
    }

    return E_OK;
}


STD_ReturnType DC_Motor_GetState(const DC_MotorHandleType *handle, DC_MotorStateType *pState)
{
    /* STEP 1: Validate the handle and the output pointer. */
    if ((handle == NULL) || (handle->initialized == 0U) || (pState == NULL))
    {
        return E_NOK;
    }

    /* STEP 2: Report the last COMMANDED state - this is not a measurement. */
    *pState = handle->state;

    return E_OK;
}


STD_ReturnType DC_Motor_GetSpeed(const DC_MotorHandleType *handle, uint8_h *pSpeed)
{
    /* STEP 1: Validate the handle and the output pointer. */
    if ((handle == NULL) || (handle->initialized == 0U) || (pSpeed == NULL))
    {
        return E_NOK;
    }

    /* STEP 2: Report the last speed percentage commanded. */
    *pSpeed = handle->speedPercent;

    return E_OK;
}


STD_ReturnType DC_Motor_DeInit(DC_MotorHandleType *handle)
{
    uint8_h local_Port = 0U;
    uint8_h local_Pin  = 0U;

    /* STEP 1: Validate the handle. */
    if ((handle == NULL) || (handle->initialized == 0U))
    {
        return E_NOK;
    }

    /* STEP 2: Stop the motor before taking anything apart. */
    (void)DC_Motor_Stop(handle);

    /*
     * STEP 3: Disconnect this motor's compare output and park its pin low. The
     *         timer itself is left running, because the sibling channel (OC1A vs
     *         OC1B) may still be driving another motor.
     */
    if (handle->pwmChannel != DC_MOTOR_PWM_NONE)
    {
        DC_Motor_PwmPin(handle->pwmChannel, &local_Port, &local_Pin);
        DC_Motor_PwmConnect(handle->pwmChannel, 0U);
        DC_Motor_PwmSetDuty(handle->pwmChannel, 0U);
        (void)GPIO_SetPinValue(local_Port, local_Pin, PIN_LOW);
    }

    /* STEP 4: The handle is unusable until Init() runs again. */
    handle->speedPercent = 0U;
    handle->initialized  = 0U;

    return E_OK;
}
