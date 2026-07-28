#ifndef STEPPER_L298P_H
#define STEPPER_L298P_H

#include "../../Service/STD_Types.h"
#include "../../MCL/GPIO/gpio_interface.h"

/* ================================================================================
 *  STEPPER MOTOR DRIVER - PUBLIC INTERFACE (HAL, L298P / L298N H-bridge)
 *  ------------------------------------------------------------------------------
 *  Drives a 4-wire stepper through a dual H-bridge (L298P shield, L298N module,
 *  L293D, or a ULN2003 board for the 28BYJ-48 unipolar motor - the pin pattern
 *  is the same in every case).
 *
 *      MCU IN1 ---> bridge IN1 ---> coil A
 *      MCU IN2 ---> bridge IN2 ---> coil A'   (the other end of coil A)
 *      MCU IN3 ---> bridge IN3 ---> coil B
 *      MCU IN4 ---> bridge IN4 ---> coil B'
 *      ENA / ENB : bridge enables. Tie them high with a jumper, or wire them to
 *                  MCU pins and set 'useEnablePins' = 1 so the driver can cut the
 *                  current (see Stepper_L298P_Release).
 *
 *  --------------------------------------------------------------------------
 *  STEP MODES
 *  --------------------------------------------------------------------------
 *  | Mode      | Coils on | Torque | Steps/rev | Notes                        |
 *  |-----------|----------|--------|-----------|------------------------------|
 *  | WAVE      | 1        | lowest | N         | Cheapest on current          |
 *  | FULL      | 2        | 100%   | N         | The usual choice             |
 *  | HALF      | 1 and 2  | ~70%   | 2N        | Twice the resolution, smoother|
 *
 *  --------------------------------------------------------------------------
 *  MULTIPLE MOTORS
 *  --------------------------------------------------------------------------
 *  Every function takes a pointer to YOUR handle and the driver keeps no global
 *  state, so each motor is just another handle:
 *
 *      Stepper_L298P_HandleType g_stepperX;
 *      Stepper_L298P_HandleType g_stepperY;
 *
 *  Two motors need two bridges (one L298 = one stepper) and eight MCU pins.
 *  Note that Stepper_L298P_Step() BLOCKS while it steps, so two motors driven
 *  that way move one after the other, not together. To move them at the same
 *  time use Stepper_L298P_StepOnce() from your scheduler and do the timing
 *  yourself - see the example under Stepper_L298P_StepOnce().
 *
 *  --------------------------------------------------------------------------
 *  HOW TO USE
 *  --------------------------------------------------------------------------
 *      Stepper_L298P_HandleType motor;
 *
 *      motor.in1Port = GPIO_PORTB;  motor.in1Pin = GPIO_PIN0;
 *      motor.in2Port = GPIO_PORTB;  motor.in2Pin = GPIO_PIN1;
 *      motor.in3Port = GPIO_PORTB;  motor.in3Pin = GPIO_PIN2;
 *      motor.in4Port = GPIO_PORTB;  motor.in4Pin = GPIO_PIN3;
 *      motor.useEnablePins = 0;                 // ENA/ENB jumpered high
 *      motor.stepMode      = STEPPER_L298P_MODE_FULL;
 *      motor.stepsPerRev   = 200;               // full steps per revolution
 *      motor.stepDelayMs   = 5;                 // 5 ms between steps
 *
 *      Stepper_L298P_Init(&motor);
 *      Stepper_L298P_Step(&motor, 200, STEPPER_L298P_DIR_CW);   // one turn
 *      Stepper_L298P_RotateAngle(&motor, 90, STEPPER_L298P_DIR_CCW);
 *      Stepper_L298P_Release(&motor);           // stop heating the coils
 * ============================================================================== */

/* ---------------- Step Mode ---------------- */
/**
 * @brief Coil excitation sequence.
 *  - WAVE : one coil at a time  (A, B, A', B')      - least current, least torque.
 *  - FULL : two coils at a time (AB, BA', A'B', B'A) - full torque, same resolution.
 *  - HALF : alternates one and two coils             - double resolution, smoother.
 */
typedef enum
{
    STEPPER_L298P_MODE_WAVE = 0,
    STEPPER_L298P_MODE_FULL = 1,
    STEPPER_L298P_MODE_HALF = 2
} Stepper_L298P_ModeType;

/* ---------------- Direction ---------------- */
/**
 * @brief Rotation direction. Which way the shaft actually turns depends on how
 *        the coils are wired; swap IN1/IN2 (or CW/CCW in your code) if it is
 *        backwards.
 */
typedef enum
{
    STEPPER_L298P_DIR_CW  = 0,   /* phase index counts up   */
    STEPPER_L298P_DIR_CCW = 1    /* phase index counts down */
} Stepper_L298P_DirType;

/* ---------------- Handle ---------------- */
/**
 * @brief One stepper instance: its wiring, its motor data and the driver's
 *        private position tracking.
 *
 * Fill the CONFIGURATION fields before calling Stepper_L298P_Init().
 * The RUNTIME fields belong to the driver.
 *
 * @var Stepper_L298P_HandleType::in1Port/in1Pin  Bridge input 1 (coil A).
 * @var Stepper_L298P_HandleType::in2Port/in2Pin  Bridge input 2 (coil A').
 * @var Stepper_L298P_HandleType::in3Port/in3Pin  Bridge input 3 (coil B).
 * @var Stepper_L298P_HandleType::in4Port/in4Pin  Bridge input 4 (coil B').
 * @var Stepper_L298P_HandleType::enAPort/enAPin  ENA pin (only if useEnablePins).
 * @var Stepper_L298P_HandleType::enBPort/enBPin  ENB pin (only if useEnablePins).
 * @var Stepper_L298P_HandleType::useEnablePins   0 = ENA/ENB jumpered high,
 *                                                1 = driver owns them.
 * @var Stepper_L298P_HandleType::stepMode        Excitation sequence.
 * @var Stepper_L298P_HandleType::stepsPerRev     FULL steps per revolution of the
 *                                                motor (200 for a 1.8 deg motor;
 *                                                2048 for a geared 28BYJ-48).
 * @var Stepper_L298P_HandleType::stepDelayMs     Milliseconds between steps. This
 *                                                is the speed control - smaller is
 *                                                faster, and too small makes the
 *                                                motor stall and buzz instead of turn.
 */
typedef struct
{
    /* ---- configuration: fill these before Init ---- */
    uint8_h in1Port;  uint8_h in1Pin;
    uint8_h in2Port;  uint8_h in2Pin;
    uint8_h in3Port;  uint8_h in3Pin;
    uint8_h in4Port;  uint8_h in4Pin;
    uint8_h enAPort;  uint8_h enAPin;
    uint8_h enBPort;  uint8_h enBPin;
    uint8_h useEnablePins;

    Stepper_L298P_ModeType stepMode;
    uint16_h stepsPerRev;
    uint16_h stepDelayMs;

    /* ---- runtime: owned by the driver, do not modify ---- */
    uint8_h  initialized;
    uint8_h  phaseIndex;    /* where we are in the excitation table */
    uint8_h  energized;     /* 1 = coils currently powered          */
    sint32   position;      /* net steps since Init / ResetPosition */
} Stepper_L298P_HandleType;

/* ================================================================================
 *  FUNCTION PROTOTYPES
 * ============================================================================== */

/**
 * @brief  Sets the four bridge inputs (and the enables, if owned) to output, puts
 *         the motor in a known de-energized state and zeroes the position counter.
 * @param  handle  Pointer to YOUR handle with the configuration fields filled in.
 * @return E_OK on success; E_NOK on a NULL handle, a port out of range, or
 *         stepsPerRev == 0.
 * @note   The motor is left released (all coils off). Call a Step function or
 *         Stepper_L298P_Hold() to energize it.
 */
STD_ReturnType Stepper_L298P_Init(Stepper_L298P_HandleType *handle);

/**
 * @brief  Changes the excitation sequence at runtime, e.g. FULL for a fast move
 *         then HALF for a fine approach.
 * @param  handle  Initialized motor.
 * @param  mode    New step mode.
 * @return E_OK/E_NOK.
 * @note   The phase index restarts at 0, so the motor may jump up to one step.
 *         Change modes while stopped, not mid-move.
 */
STD_ReturnType Stepper_L298P_SetStepMode(Stepper_L298P_HandleType *handle,
                                         Stepper_L298P_ModeType mode);

/**
 * @brief  Sets the delay between steps directly. This is the raw speed control.
 * @param  handle       Initialized motor.
 * @param  stepDelayMs  Milliseconds between steps; forced to a minimum of 1.
 * @return E_OK/E_NOK.
 */
STD_ReturnType Stepper_L298P_SetStepDelay(Stepper_L298P_HandleType *handle,
                                          uint16_h stepDelayMs);

/**
 * @brief  Sets the speed in revolutions per minute and converts it into a step
 *         delay using stepsPerRev and the active step mode.
 * @param  handle  Initialized motor.
 * @param  rpm     Target speed, must be > 0.
 * @return E_OK/E_NOK (E_NOK if rpm is 0 or the resulting delay would be 0 ms,
 *         which means the requested speed is beyond what this driver can time).
 * @note   The real ceiling is the motor's, not the driver's: ask for more torque
 *         than the coils can build at that step rate and the shaft just buzzes.
 */
STD_ReturnType Stepper_L298P_SetSpeedRpm(Stepper_L298P_HandleType *handle, uint16_h rpm);

/**
 * @brief  Moves a number of steps in one direction, BLOCKING until finished.
 * @param  handle  Initialized motor.
 * @param  steps   How many steps to take (in the active mode's step unit).
 * @param  dir     STEPPER_L298P_DIR_CW or _CCW.
 * @return E_OK/E_NOK.
 * @note   Blocks for steps * stepDelayMs milliseconds. 200 steps at 5 ms is a
 *         full second in which nothing else in your program runs - which is why
 *         an alarm panel or any scheduled system should use StepOnce() instead.
 * @note   The coils stay energized (holding torque) when it returns. Call
 *         Stepper_L298P_Release() if the motor should not stay hot.
 */
STD_ReturnType Stepper_L298P_Step(Stepper_L298P_HandleType *handle,
                                  uint16_h steps, Stepper_L298P_DirType dir);

/**
 * @brief  Advances exactly ONE step and returns immediately - no delay inside.
 *         This is the non-blocking building block: you decide when the next step
 *         is due, so several motors can move at once and the rest of the program
 *         keeps running.
 * @param  handle  Initialized motor.
 * @param  dir     Direction of this single step.
 * @return E_OK/E_NOK.
 *
 * Example - two motors turning together from a 1 ms scheduler tick:
 * @code
 *     void Task_1ms(void)
 *     {
 *         static uint16_h tick = 0;
 *         tick++;
 *         if ((tick % 5u) == 0u) { Stepper_L298P_StepOnce(&motorX, DIR_CW);  }
 *         if ((tick % 8u) == 0u) { Stepper_L298P_StepOnce(&motorY, DIR_CCW); }
 *     }
 * @endcode
 */
STD_ReturnType Stepper_L298P_StepOnce(Stepper_L298P_HandleType *handle,
                                      Stepper_L298P_DirType dir);

/**
 * @brief  Rotates by an angle in whole degrees, BLOCKING until finished. The step
 *         count is computed from stepsPerRev and the active step mode.
 * @param  handle   Initialized motor.
 * @param  degrees  Angle to turn, 0 .. 65535 (values above 360 mean several turns).
 * @param  dir      Direction.
 * @return E_OK/E_NOK.
 * @note   Integer division truncates, so an angle that is not a whole number of
 *         steps loses the remainder. Repeated small moves therefore drift - use
 *         Stepper_L298P_GetPosition() as the truth, not the sum of your requests.
 */
STD_ReturnType Stepper_L298P_RotateAngle(Stepper_L298P_HandleType *handle,
                                         uint16_h degrees, Stepper_L298P_DirType dir);

/**
 * @brief  Re-energizes the current phase so the shaft resists being turned
 *         (holding torque) without moving.
 * @param  handle  Initialized motor.
 * @return E_OK/E_NOK.
 * @note   Holding draws full current and heats both motor and bridge. Hold only
 *         when something is actually pushing back on the shaft.
 */
STD_ReturnType Stepper_L298P_Hold(Stepper_L298P_HandleType *handle);

/**
 * @brief  Cuts the current to every coil: no torque, no heat, and the shaft turns
 *         freely. The position counter is kept, but anything that moves the shaft
 *         while released makes that counter a lie.
 * @param  handle  Initialized motor.
 * @return E_OK/E_NOK.
 * @note   If useEnablePins is set, ENA/ENB are driven low as well, which is the
 *         only way to be sure the bridge outputs are off.
 */
STD_ReturnType Stepper_L298P_Release(Stepper_L298P_HandleType *handle);

/**
 * @brief  Returns the net step count since Init() or the last ResetPosition():
 *         positive for clockwise, negative for counter-clockwise.
 * @param  handle     Initialized motor.
 * @param  pPosition  Receives the position; must not be NULL.
 * @return E_OK/E_NOK.
 */
STD_ReturnType Stepper_L298P_GetPosition(const Stepper_L298P_HandleType *handle,
                                         sint32 *pPosition);

/**
 * @brief  Declares the current shaft position to be zero. Call it after homing
 *         against a limit switch.
 * @param  handle  Initialized motor.
 * @return E_OK/E_NOK.
 */
STD_ReturnType Stepper_L298P_ResetPosition(Stepper_L298P_HandleType *handle);

/**
 * @brief  Returns the number of steps in one full revolution IN THE ACTIVE MODE
 *         (stepsPerRev, or twice that in half-step mode). Use it to convert
 *         between steps and angles yourself.
 * @param  handle      Initialized motor.
 * @param  pStepsPerRev Receives the value; must not be NULL.
 * @return E_OK/E_NOK.
 */
STD_ReturnType Stepper_L298P_GetStepsPerRev(const Stepper_L298P_HandleType *handle,
                                            uint16_h *pStepsPerRev);

#endif /* STEPPER_L298P_H */
