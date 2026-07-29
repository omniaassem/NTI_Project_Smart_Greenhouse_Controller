#ifndef SERVO_MOTOR_H
#define SERVO_MOTOR_H

#include "../../Service/STD_Types.h"
#include "../../MCL/GPIO/gpio_interface.h"

/* ================================================================================
 *  RC SERVO DRIVER - PUBLIC INTERFACE (HAL)
 *  ------------------------------------------------------------------------------
 *  A hobby servo (SG90, MG996R, ...) is commanded by the WIDTH of a pulse that is
 *  repeated every 20 ms (50 Hz):
 *
 *      |<------------------ 20 ms frame ------------------>|
 *       ____                                                ____
 *      |    |______________________________________________|    |___
 *      |<-->|  1.0 ms  = one end of travel   (0 deg)
 *      |<------>|  1.5 ms  = centre              (90 deg)
 *      |<---------->|  2.0 ms  = the other end      (180 deg)
 *
 *  Only the width matters; the gap is just the frame. Everything this driver does
 *  is producing that pulse accurately, over and over.
 *
 *  --------------------------------------------------------------------------
 *  TWO WAYS TO DRIVE A SERVO - AND WHY THERE ARE TWO
 *  --------------------------------------------------------------------------
 *  HARDWARE (SERVO_MOTOR_DRIVE_TIMER1_OC1A / _OC1B)
 *      Timer1 generates the waveform in silicon: rock steady, zero CPU cost, and
 *      the servo does not twitch when an interrupt fires. But the ATmega32 has
 *      exactly two Timer1 compare outputs, so this mode gives you TWO servos:
 *          OC1A -> pin PD5 ,  OC1B -> pin PD4
 *      Use this for anything that has to hold a position precisely.
 *
 *  SOFTWARE (SERVO_MOTOR_DRIVE_SOFTWARE)
 *      The driver toggles an ordinary GPIO pin and times the pulse with a
 *      calibrated busy-wait. Any pin works and you can have up to
 *      SERVO_MOTOR_MAX_SOFTWARE of them, but YOU must call
 *      Servo_Motor_SoftwareRefresh() every 20 ms, and that call blocks for the
 *      sum of the pulse widths (about 1.5 ms per servo). Interrupts that fire
 *      mid-pulse stretch it and make the servo jitter.
 *
 *  Mixing them is fine: two precise servos on Timer1 plus a few software ones.
 *
 *  --------------------------------------------------------------------------
 *  MULTIPLE SERVOS
 *  --------------------------------------------------------------------------
 *      Servo_MotorHandleType g_servoPan;    // hardware, OC1A -> PD5
 *      Servo_MotorHandleType g_servoTilt;   // hardware, OC1B -> PD4
 *      Servo_MotorHandleType g_servoGate;   // software, any pin
 *
 *  Both hardware servos share Timer1's 50 Hz frame but have independent widths,
 *  so they move independently. Software servos are pulsed one after another
 *  inside each refresh.
 *
 *  --------------------------------------------------------------------------
 *  PERIPHERAL OWNERSHIP - READ THIS BEFORE WIRING
 *  --------------------------------------------------------------------------
 *  A hardware servo makes this driver the owner of TIMER1: its mode, prescaler
 *  and TOP are set for a 50 Hz frame. Nothing else may reprogram Timer1 - in
 *  particular do not also ask the DC-motor driver for PWM on OC1A/OC1B, and do
 *  not use Timer1 as your system tick. Timer0 and Timer2 are untouched.
 *
 *  --------------------------------------------------------------------------
 *  HOW TO USE
 *  --------------------------------------------------------------------------
 *      Servo_MotorHandleType pan;
 *
 *      pan.drive      = SERVO_MOTOR_DRIVE_TIMER1_OC1A;   // pin PD5
 *      pan.minPulseUs = SERVO_MOTOR_DEFAULT_MIN_US;      // 1000
 *      pan.maxPulseUs = SERVO_MOTOR_DEFAULT_MAX_US;      // 2000
 *      pan.maxAngle   = 180;
 *
 *      Servo_Motor_Init(&pan);
 *      Servo_Motor_SetAngle(&pan, 90);       // centre
 *
 *  and for a software servo:
 *
 *      Servo_MotorHandleType gate;
 *      gate.drive      = SERVO_MOTOR_DRIVE_SOFTWARE;
 *      gate.port       = GPIO_PORTB;  gate.pin = GPIO_PIN0;
 *      gate.minPulseUs = 1000;  gate.maxPulseUs = 2000;  gate.maxAngle = 180;
 *      Servo_Motor_Init(&gate);
 *      Servo_Motor_SetAngle(&gate, 45);
 *
 *      while (1) {
 *          Servo_Motor_SoftwareRefresh();   // every 20 ms, from your scheduler
 *      }
 * ============================================================================== */

/* ---------------- Tunables ---------------- */
/** @brief How many SOFTWARE servos may be registered at once. */
#ifndef SERVO_MOTOR_MAX_SOFTWARE
#define SERVO_MOTOR_MAX_SOFTWARE       8U
#endif

/** @brief Pulse width at angle 0 for a typical servo. */
#define SERVO_MOTOR_DEFAULT_MIN_US     1000U
/** @brief Pulse width at full angle for a typical servo. */
#define SERVO_MOTOR_DEFAULT_MAX_US     2000U
/** @brief Frame period the servo expects between pulses. */
#define SERVO_MOTOR_FRAME_MS           20U

/* ---------------- Drive Mode ---------------- */
/**
 * @brief How the pulse is produced for this servo.
 *  - TIMER1_OC1A : hardware PWM on pin PD5. Precise, free, only one of these.
 *  - TIMER1_OC1B : hardware PWM on pin PD4. Precise, free, only one of these.
 *  - SOFTWARE    : bit-banged on any GPIO pin; needs Servo_Motor_SoftwareRefresh().
 */
typedef enum
{
    SERVO_MOTOR_DRIVE_TIMER1_OC1A = 0,
    SERVO_MOTOR_DRIVE_TIMER1_OC1B = 1,
    SERVO_MOTOR_DRIVE_SOFTWARE    = 2
} Servo_MotorDriveType;

/* ---------------- Handle ---------------- */
/**
 * @brief One servo instance: how it is driven, its pulse calibration and the
 *        driver's private record of where it was last commanded.
 *
 * Fill the CONFIGURATION fields before calling Servo_Motor_Init().
 * The RUNTIME fields belong to the driver.
 *
 * @var Servo_MotorHandleType::drive       Hardware channel or software pin.
 * @var Servo_MotorHandleType::port        GPIO port  - SOFTWARE mode only.
 * @var Servo_MotorHandleType::pin         GPIO pin   - SOFTWARE mode only.
 *                                         (Hardware mode ignores both: the pin is
 *                                          fixed by silicon at PD5 or PD4.)
 * @var Servo_MotorHandleType::minPulseUs  Pulse width for angle 0 (typically 1000,
 *                                         some servos want 500 or 600).
 * @var Servo_MotorHandleType::maxPulseUs  Pulse width for 'maxAngle' (typically
 *                                         2000, some servos want 2400).
 * @var Servo_MotorHandleType::maxAngle    Mechanical travel in degrees (usually 180).
 */
typedef struct
{
    /* ---- configuration: fill these before Init ---- */
    Servo_MotorDriveType drive;
    uint8_h  port;
    uint8_h  pin;
    uint16_h minPulseUs;
    uint16_h maxPulseUs;
    uint8_h  maxAngle;

    /* ---- runtime: owned by the driver, do not modify ---- */
    uint8_h  initialized;
    uint8_h  running;          /* 1 = pulses are being produced         */
    uint8_h  currentAngle;     /* last angle commanded                  */
    uint16_h currentPulseUs;   /* last pulse width commanded            */
} Servo_MotorHandleType;

/* ================================================================================
 *  FUNCTION PROTOTYPES
 * ============================================================================== */

/**
 * @brief  Prepares one servo. In hardware mode it configures Timer1 for a 50 Hz
 *         frame (Fast PWM, TOP = ICR1, prescaler 8), connects the chosen compare
 *         output and makes its pin an output. In software mode it makes the
 *         chosen pin an output and registers the servo in the refresh list.
 *         Either way the servo is parked at the centre of its travel.
 * @param  handle  Pointer to YOUR handle with the configuration fields filled in.
 * @return E_OK on success; E_NOK on a NULL handle, an invalid drive mode, a port
 *         out of range, maxAngle == 0, minPulseUs >= maxPulseUs, or (software
 *         mode) when SERVO_MOTOR_MAX_SOFTWARE servos are already registered.
 * @note   Calling it for a second hardware servo does NOT reset the first: the
 *         shared Timer1 setup is written once and only the new compare channel
 *         is connected.
 */
STD_ReturnType Servo_Motor_Init(Servo_MotorHandleType *handle);

/**
 * @brief  Commands an angle. The angle is mapped linearly onto the pulse range:
 *             pulse = minPulseUs + angle * (maxPulseUs - minPulseUs) / maxAngle
 * @param  handle  Initialized servo.
 * @param  angle   0 .. maxAngle degrees.
 * @return E_OK/E_NOK (E_NOK if the angle is beyond maxAngle).
 * @note   The servo takes real time to get there - roughly 150 ms per 60 deg for
 *         an SG90. The call returns immediately; it commands, it does not wait.
 * @note   There is no feedback. If the arm is blocked, the servo will keep
 *         pushing, draw stall current and get hot.
 */
STD_ReturnType Servo_Motor_SetAngle(Servo_MotorHandleType *handle, uint8_h angle);

/**
 * @brief  Commands a raw pulse width, bypassing the angle mapping. Use it to
 *         calibrate a servo's real end stops before settling on min/max, or to
 *         drive a continuous-rotation servo (where the width sets SPEED, with
 *         about 1500 us meaning stop).
 * @param  handle    Initialized servo.
 * @param  pulseUs   Pulse width in microseconds.
 * @return E_OK/E_NOK.
 * @note   The value is clamped into [minPulseUs, maxPulseUs]. Widen those fields
 *         first if you are hunting for the true mechanical limits - driving a
 *         servo past its stop is how gears get stripped.
 */
STD_ReturnType Servo_Motor_SetPulseUs(Servo_MotorHandleType *handle, uint16_h pulseUs);

/**
 * @brief  Reports the last angle commanded (NOT a measurement - an RC servo
 *         gives no position feedback).
 * @param  handle  Initialized servo.
 * @param  pAngle  Receives the angle; must not be NULL.
 * @return E_OK/E_NOK.
 */
STD_ReturnType Servo_Motor_GetAngle(const Servo_MotorHandleType *handle, uint8_h *pAngle);

/**
 * @brief  Stops sending pulses. With no pulses a servo goes limp: it stops
 *         holding, stops buzzing and stops drawing holding current, and the arm
 *         can be turned by hand.
 * @param  handle  Initialized servo.
 * @return E_OK/E_NOK.
 * @note   Hardware mode disconnects the compare output from the pin and drives
 *         it low; software mode drops the servo out of the refresh list.
 */
STD_ReturnType Servo_Motor_Stop(Servo_MotorHandleType *handle);

/**
 * @brief  Resumes pulsing after Servo_Motor_Stop(), re-commanding the last angle.
 * @param  handle  Initialized servo.
 * @return E_OK/E_NOK.
 * @note   The servo will snap to that angle at full speed if something moved the
 *         arm while it was limp.
 */
STD_ReturnType Servo_Motor_Start(Servo_MotorHandleType *handle);

/**
 * @brief  Emits ONE pulse for every registered, running SOFTWARE servo, one
 *         after another. Call it every 20 ms - that cadence IS the frame rate.
 * @return E_OK, or E_NOK if no software servos are registered.
 *
 * @note   BLOCKS for the sum of the pulse widths: about 1.5 ms per servo, so
 *         ~6 ms with four servos. That is time your scheduler does not get.
 * @note   Called too slowly (or with a jittery period) the servo will twitch or
 *         creak. Called from a 20 ms task with interrupts light, it is steady
 *         enough for a gate, a lock or a pointer.
 * @note   Hardware servos ignore this function entirely - Timer1 pulses them
 *         whether you call it or not.
 *
 * Example:
 * @code
 *     void Task_20ms(void)
 *     {
 *         Servo_Motor_SoftwareRefresh();
 *     }
 * @endcode
 */
STD_ReturnType Servo_Motor_SoftwareRefresh(void);

/**
 * @brief  Releases the servo: stops the pulses and, in software mode, frees its
 *         slot in the refresh list so another servo can take it.
 * @param  handle  Initialized servo.
 * @return E_OK/E_NOK.
 * @note   Timer1 is left running for any other hardware servo; it is only fully
 *         released when the last hardware servo is de-initialized.
 */
STD_ReturnType Servo_Motor_DeInit(Servo_MotorHandleType *handle);

#endif /* SERVO_MOTOR_H */
