#ifndef DC_MOTOR_H
#define DC_MOTOR_H

#include "../../Service/STD_Types.h"
#include "../../MCL/GPIO/gpio_interface.h"

/* ================================================================================
 *  BRUSHED DC MOTOR DRIVER - PUBLIC INTERFACE (HAL, H-bridge)
 *  ------------------------------------------------------------------------------
 *  Controls one brushed DC motor through an H-bridge (L298N / L298P / L293D).
 *  Two pins choose what the bridge does, one pin decides how hard:
 *
 *      IN1  IN2   bridge output       this driver calls it
 *      ---  ---   ----------------    --------------------
 *       0    0    both sides low      STOP   (coast: motor freewheels)
 *       1    0    forward             FORWARD
 *       0    1    reverse             BACKWARD
 *       1    1    both sides high     BRAKE  (windings shorted: stops hard)
 *
 *      EN / ENA   enable. Held high = full speed. Fed a PWM waveform = the duty
 *                 cycle sets the speed.
 *
 *  --------------------------------------------------------------------------
 *  SPEED CONTROL - PICK A PWM CHANNEL, OR NONE
 *  --------------------------------------------------------------------------
 *  | pwmChannel | Timer  | Pin | Notes                                        |
 *  |------------|--------|-----|----------------------------------------------|
 *  | NONE       | -      | any | Enable is a plain GPIO: full speed or stopped |
 *  | OC0        | Timer0 | PB3 | 8-bit fast PWM                                |
 *  | OC1A       | Timer1 | PD5 | 8-bit fast PWM                                |
 *  | OC1B       | Timer1 | PD4 | 8-bit fast PWM                                |
 *  | OC2        | Timer2 | PD7 | 8-bit fast PWM                                |
 *
 *  The compare-output pins are fixed in silicon: choosing OC0 means the enable
 *  wire goes to PB3, not to a pin of your choosing.
 *
 *  --------------------------------------------------------------------------
 *  MULTIPLE MOTORS
 *  --------------------------------------------------------------------------
 *  Every function takes a pointer to YOUR handle and the driver keeps no global
 *  state, so each motor is another handle:
 *
 *      DC_MotorHandleType g_motorLeft;    // IN1/IN2 + PWM on OC1A
 *      DC_MotorHandleType g_motorRight;   // IN1/IN2 + PWM on OC1B
 *
 *  Four motors can have independent speeds (four compare channels); beyond that,
 *  extra motors must use DC_MOTOR_PWM_NONE and run at full speed. One L298 board
 *  carries two motors, so a differential-drive robot fits on a single bridge and
 *  a single timer.
 *
 *  --------------------------------------------------------------------------
 *  PERIPHERAL OWNERSHIP - READ THIS BEFORE WIRING
 *  --------------------------------------------------------------------------
 *  Asking for a PWM channel makes this driver the owner of that timer's mode and
 *  prescaler. Two motors on OC1A and OC1B share Timer1 safely (same frequency,
 *  independent duty), but you may NOT also use that timer for something else:
 *    - OC1A/OC1B here conflict with the SERVO driver's hardware mode (also Timer1);
 *    - OC0 conflicts with a Timer0 system tick;
 *    - OC2 conflicts with using Timer2 for a sounder.
 *  Choose one owner per timer and write it down in your pin map.
 *
 *  --------------------------------------------------------------------------
 *  HOW TO USE
 *  --------------------------------------------------------------------------
 *      DC_MotorHandleType left;
 *
 *      left.in1Port = GPIO_PORTC;  left.in1Pin = GPIO_PIN0;
 *      left.in2Port = GPIO_PORTC;  left.in2Pin = GPIO_PIN1;
 *      left.pwmChannel      = DC_MOTOR_PWM_OC1A;   // enable wire -> PD5
 *      left.invertDirection = 0;
 *
 *      DC_Motor_Init(&left);
 *      DC_Motor_SetSpeed(&left, 70);      // 70 % duty
 *      DC_Motor_Forward(&left);
 *      ...
 *      DC_Motor_Brake(&left);
 * ============================================================================== */

/* ---------------- PWM Channel ---------------- */
/**
 * @brief Which compare output drives the bridge enable pin - or none at all.
 * @note  DC_MOTOR_PWM_NONE uses enPort/enPin as an ordinary output: the motor
 *        runs at full speed or not at all, and SetSpeed() only distinguishes
 *        zero from non-zero.
 */
typedef enum
{
    DC_MOTOR_PWM_NONE = 0,   /* plain GPIO enable, no speed control */
    DC_MOTOR_PWM_OC0  = 1,   /* Timer0 -> PB3 */
    DC_MOTOR_PWM_OC1A = 2,   /* Timer1 -> PD5 */
    DC_MOTOR_PWM_OC1B = 3,   /* Timer1 -> PD4 */
    DC_MOTOR_PWM_OC2  = 4    /* Timer2 -> PD7 */
} DC_MotorPwmChannelType;

/* ---------------- Direction ---------------- */
/**
 * @brief Which way the shaft turns. Which one is physically "forward" depends on
 *        how the motor is wired; set 'invertDirection' instead of swapping wires.
 */
typedef enum
{
    DC_MOTOR_DIR_FORWARD  = 0,
    DC_MOTOR_DIR_BACKWARD = 1
} DC_MotorDirectionType;

/* ---------------- State ---------------- */
/**
 * @brief What the bridge is currently doing.
 *  - STOP     : both inputs low. The motor coasts to a halt.
 *  - FORWARD  : driven one way.
 *  - BACKWARD : driven the other way.
 *  - BRAKE    : both inputs high. The windings are shorted and the motor stops
 *               sharply - useful, but hard on the gearbox and the supply.
 */
typedef enum
{
    DC_MOTOR_STATE_STOP     = 0,
    DC_MOTOR_STATE_FORWARD  = 1,
    DC_MOTOR_STATE_BACKWARD = 2,
    DC_MOTOR_STATE_BRAKE    = 3
} DC_MotorStateType;

/* ---------------- Handle ---------------- */
/**
 * @brief One motor instance: its wiring, its speed source and the driver's
 *        private record of what it was last told to do.
 *
 * Fill the CONFIGURATION fields before calling DC_Motor_Init().
 * The RUNTIME fields belong to the driver.
 *
 * @var DC_MotorHandleType::in1Port/in1Pin  Bridge input 1.
 * @var DC_MotorHandleType::in2Port/in2Pin  Bridge input 2.
 * @var DC_MotorHandleType::enPort/enPin    Enable pin - used ONLY when
 *                                          pwmChannel is DC_MOTOR_PWM_NONE.
 *                                          With a PWM channel the pin is fixed
 *                                          by silicon and these are ignored.
 * @var DC_MotorHandleType::pwmChannel      Speed source (see the table above).
 * @var DC_MotorHandleType::invertDirection 1 swaps FORWARD and BACKWARD, so a
 *                                          motor mounted mirror-image on the
 *                                          other side of a robot still drives
 *                                          the machine forwards.
 */
typedef struct
{
    /* ---- configuration: fill these before Init ---- */
    uint8_h in1Port;  uint8_h in1Pin;
    uint8_h in2Port;  uint8_h in2Pin;
    uint8_h enPort;   uint8_h enPin;
    DC_MotorPwmChannelType pwmChannel;
    uint8_h invertDirection;

    /* ---- runtime: owned by the driver, do not modify ---- */
    uint8_h           initialized;
    uint8_h           speedPercent;   /* last speed commanded, 0..100 */
    DC_MotorStateType state;          /* last state commanded        */
} DC_MotorHandleType;

/* ================================================================================
 *  FUNCTION PROTOTYPES
 * ============================================================================== */

/**
 * @brief  Prepares one motor: both bridge inputs become outputs and are driven
 *         low, and the enable is set up - either as a plain output, or as a fast
 *         PWM compare output on the chosen timer.
 * @param  handle  Pointer to YOUR handle with the configuration fields filled in.
 * @return E_OK on success; E_NOK on a NULL handle, a port out of range, or an
 *         invalid PWM channel.
 * @note   The motor is left STOPPED with speed 0 - a driver that spun a motor on
 *         start-up would be a hazard on the bench.
 * @note   Two motors that share a timer (OC1A + OC1B) may both call this; the
 *         timer setup is written twice with the same values, which is harmless.
 */
STD_ReturnType DC_Motor_Init(DC_MotorHandleType *handle);

/**
 * @brief  Sets the speed as a percentage of full scale, applied immediately
 *         without changing direction.
 * @param  handle        Initialized motor.
 * @param  speedPercent  0..100. Values above 100 are clamped to 100.
 * @return E_OK/E_NOK.
 * @note   Speed is duty cycle, not RPM. There is no feedback: 50 % duty on a
 *         loaded motor turns far slower than 50 % of its free speed, and below
 *         roughly 20 % most geared motors do not turn at all - they just buzz.
 * @note   A speed of 0 fully disconnects the enable output and drives it low,
 *         rather than leaving a 0 % duty waveform that still produces a spike
 *         at the start of every PWM period.
 */
STD_ReturnType DC_Motor_SetSpeed(DC_MotorHandleType *handle, uint8_h speedPercent);

/**
 * @brief  Drives the motor forward at the current speed setting.
 * @param  handle  Initialized motor.
 * @return E_OK/E_NOK.
 * @note   If the speed is still 0 the motor will not move. Set a speed first.
 * @note   Reversing straight from BACKWARD to FORWARD at high duty puts a large
 *         current spike through the bridge and the supply. Stop or brake first,
 *         and give the motor a few tens of milliseconds, if you can.
 */
STD_ReturnType DC_Motor_Forward(DC_MotorHandleType *handle);

/**
 * @brief  Drives the motor backward at the current speed setting.
 * @param  handle  Initialized motor.
 * @return E_OK/E_NOK.
 */
STD_ReturnType DC_Motor_Backward(DC_MotorHandleType *handle);

/**
 * @brief  Sets the direction without touching the speed - handy when the
 *         direction comes from one input and the speed from another.
 * @param  handle  Initialized motor.
 * @param  dir     DC_MOTOR_DIR_FORWARD or DC_MOTOR_DIR_BACKWARD.
 * @return E_OK/E_NOK.
 */
STD_ReturnType DC_Motor_SetDirection(DC_MotorHandleType *handle, DC_MotorDirectionType dir);

/**
 * @brief  Cuts the drive and lets the motor coast: both bridge inputs low, the
 *         enable off. The shaft keeps turning until friction stops it.
 * @param  handle  Initialized motor.
 * @return E_OK/E_NOK.
 * @note   The stored speed is kept, so a later Forward() resumes at that speed.
 */
STD_ReturnType DC_Motor_Stop(DC_MotorHandleType *handle);

/**
 * @brief  Brakes: both bridge inputs high shorts the windings, and the motor's
 *         own back-EMF stops it far faster than coasting.
 * @param  handle  Initialized motor.
 * @return E_OK/E_NOK.
 * @note   Braking dumps the motor's kinetic energy into the bridge as heat. It
 *         is fine as an occasional stop and unkind as a habit, especially at
 *         high speed with a heavy load.
 */
STD_ReturnType DC_Motor_Brake(DC_MotorHandleType *handle);

/**
 * @brief  Reports what the motor was last told to do (a command, not a
 *         measurement - a stalled motor still reports FORWARD).
 * @param  handle  Initialized motor.
 * @param  pState  Receives the state; must not be NULL.
 * @return E_OK/E_NOK.
 */
STD_ReturnType DC_Motor_GetState(const DC_MotorHandleType *handle, DC_MotorStateType *pState);

/**
 * @brief  Reports the last speed percentage commanded.
 * @param  handle  Initialized motor.
 * @param  pSpeed  Receives 0..100; must not be NULL.
 * @return E_OK/E_NOK.
 */
STD_ReturnType DC_Motor_GetSpeed(const DC_MotorHandleType *handle, uint8_h *pSpeed);

/**
 * @brief  Stops the motor and releases its PWM channel: the compare output is
 *         disconnected from the pin and the pin is driven low.
 * @param  handle  Initialized motor.
 * @return E_OK/E_NOK.
 * @note   The timer keeps running. That is deliberate: with two motors sharing
 *         Timer1, stopping the timer here would silently kill the other motor's
 *         waveform too. Only this motor's channel is released.
 */
STD_ReturnType DC_Motor_DeInit(DC_MotorHandleType *handle);

#endif /* DC_MOTOR_H */
