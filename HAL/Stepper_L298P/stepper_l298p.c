#include "../../Service/STD_Types.h"
#include "../../Service/Bit_Math.h"
#include "../../MCL/GPIO/gpio_interface.h"
#include "stepper_l298p.h"

/*
 * util/delay.h needs F_CPU at compile time. The Makefile passes -DF_CPU, this
 * fallback only keeps the file compilable on its own.
 */
#ifndef F_CPU
#define F_CPU 8000000UL
#endif
#include <util/delay.h>

/* ================================================================================
 *  STEPPER MOTOR DRIVER - IMPLEMENTATION (HAL, L298P / L298N H-bridge)
 *  ------------------------------------------------------------------------------
 *  Each body keeps the ordered steps it implements as comments, followed by the
 *  actual GPIO calls. All state lives in the caller's handle, so two motors
 *  never interfere.
 *
 *  The excitation tables below hold one nibble per step:
 *      bit0 -> IN1, bit1 -> IN2, bit2 -> IN3, bit3 -> IN4
 * ============================================================================== */

/* WAVE: one coil at a time. Cheapest on current, weakest torque. */
static const uint8_h STEPPER_WAVE_TABLE[4] =
{
    0x01U,   /* 0001 : IN1                */
    0x02U,   /* 0010 :      IN2           */
    0x04U,   /* 0100 :           IN3      */
    0x08U    /* 1000 :                IN4 */
};

/* FULL: two coils at a time. Same resolution as WAVE, roughly double the torque. */
static const uint8_h STEPPER_FULL_TABLE[4] =
{
    0x03U,   /* 0011 : IN1 + IN2 */
    0x06U,   /* 0110 : IN2 + IN3 */
    0x0CU,   /* 1100 : IN3 + IN4 */
    0x09U    /* 1001 : IN4 + IN1 */
};

/* HALF: alternates one and two coils, so each entry is half a full step. */
static const uint8_h STEPPER_HALF_TABLE[8] =
{
    0x01U,   /* 0001 */
    0x03U,   /* 0011 */
    0x02U,   /* 0010 */
    0x06U,   /* 0110 */
    0x04U,   /* 0100 */
    0x0CU,   /* 1100 */
    0x08U,   /* 1000 */
    0x09U    /* 1001 */
};


/* --------------------------------------------------------------------------
 *  INTERNAL HELPERS (static - not part of the public interface)
 * ------------------------------------------------------------------------ */

/* Number of entries in the table used by the active mode. */
static uint8_h Stepper_TableLength(Stepper_L298P_ModeType mode)
{
    return (mode == STEPPER_L298P_MODE_HALF) ? 8U : 4U;
}

/* The excitation pattern for one index of the active mode's table. */
static uint8_h Stepper_TableEntry(Stepper_L298P_ModeType mode, uint8_h index)
{
    uint8_h local_Pattern = 0U;

    switch (mode)
    {
        case STEPPER_L298P_MODE_WAVE:  local_Pattern = STEPPER_WAVE_TABLE[index & 0x03U]; break;
        case STEPPER_L298P_MODE_HALF:  local_Pattern = STEPPER_HALF_TABLE[index & 0x07U]; break;
        case STEPPER_L298P_MODE_FULL:
        default:                       local_Pattern = STEPPER_FULL_TABLE[index & 0x03U]; break;
    }

    return local_Pattern;
}

/* Writes one excitation nibble to the four bridge inputs. */
static void Stepper_ApplyPattern(Stepper_L298P_HandleType *handle, uint8_h pattern)
{
    (void)GPIO_SetPinValue(handle->in1Port, handle->in1Pin, (uint8_h)GET_BIT(pattern, 0));
    (void)GPIO_SetPinValue(handle->in2Port, handle->in2Pin, (uint8_h)GET_BIT(pattern, 1));
    (void)GPIO_SetPinValue(handle->in3Port, handle->in3Pin, (uint8_h)GET_BIT(pattern, 2));
    (void)GPIO_SetPinValue(handle->in4Port, handle->in4Pin, (uint8_h)GET_BIT(pattern, 3));

    handle->energized = (pattern != 0U) ? 1U : 0U;
}

/* Blocking millisecond delay built from constant-argument _delay_ms(1) chunks. */
static void Stepper_DelayMs(uint16_h ms)
{
    while (ms > 0U)
    {
        _delay_ms(1);
        ms--;
    }
}


/* --------------------------------------------------------------------------
 *  PUBLIC FUNCTIONS
 * ------------------------------------------------------------------------ */

STD_ReturnType Stepper_L298P_Init(Stepper_L298P_HandleType *handle)
{
    /* STEP 1: Validate the handle, the four ports and the motor data. */
    if (handle == NULL)
    {
        return E_NOK;
    }

    if ((handle->in1Port >= GPIO_NUMBER_OF_PORTS) || (handle->in2Port >= GPIO_NUMBER_OF_PORTS) ||
        (handle->in3Port >= GPIO_NUMBER_OF_PORTS) || (handle->in4Port >= GPIO_NUMBER_OF_PORTS))
    {
        return E_NOK;
    }

    if (handle->stepsPerRev == 0U)
    {
        return E_NOK;
    }

    /* STEP 2: All four bridge inputs are outputs. */
    (void)GPIO_SetPinDirection(handle->in1Port, handle->in1Pin, GPIO_OUTPUT);
    (void)GPIO_SetPinDirection(handle->in2Port, handle->in2Pin, GPIO_OUTPUT);
    (void)GPIO_SetPinDirection(handle->in3Port, handle->in3Pin, GPIO_OUTPUT);
    (void)GPIO_SetPinDirection(handle->in4Port, handle->in4Pin, GPIO_OUTPUT);

    /*
     * STEP 3: If the driver owns ENA/ENB, make them outputs and enable the
     *         bridge. Boards with the enable jumpers fitted skip this.
     */
    if (handle->useEnablePins != 0U)
    {
        if ((handle->enAPort >= GPIO_NUMBER_OF_PORTS) || (handle->enBPort >= GPIO_NUMBER_OF_PORTS))
        {
            return E_NOK;
        }

        (void)GPIO_SetPinDirection(handle->enAPort, handle->enAPin, GPIO_OUTPUT);
        (void)GPIO_SetPinDirection(handle->enBPort, handle->enBPin, GPIO_OUTPUT);
        (void)GPIO_SetPinValue(handle->enAPort, handle->enAPin, PIN_HIGH);
        (void)GPIO_SetPinValue(handle->enBPort, handle->enBPin, PIN_HIGH);
    }

    /* STEP 4: A step delay of zero would mean "step as fast as the CPU can". */
    if (handle->stepDelayMs == 0U)
    {
        handle->stepDelayMs = 1U;
    }

    /* STEP 5: Start from a known phase with the coils off and the position zeroed. */
    handle->phaseIndex = 0U;
    handle->position   = 0;
    handle->energized  = 0U;
    Stepper_ApplyPattern(handle, 0x00U);

    /* STEP 6: Mark the handle usable. */
    handle->initialized = 1U;

    return E_OK;
}


STD_ReturnType Stepper_L298P_SetStepMode(Stepper_L298P_HandleType *handle,
                                         Stepper_L298P_ModeType mode)
{
    /* STEP 1: Validate the handle and the mode. */
    if ((handle == NULL) || (handle->initialized == 0U))
    {
        return E_NOK;
    }

    if (mode > STEPPER_L298P_MODE_HALF)
    {
        return E_NOK;
    }

    /*
     * STEP 2: Store the mode and restart the sequence. The tables do not line up
     *         with each other, so continuing from the old index would energize a
     *         pattern that has nothing to do with where the rotor actually is.
     */
    handle->stepMode   = mode;
    handle->phaseIndex = 0U;

    return E_OK;
}


STD_ReturnType Stepper_L298P_SetStepDelay(Stepper_L298P_HandleType *handle,
                                          uint16_h stepDelayMs)
{
    /* STEP 1: Validate the handle. */
    if ((handle == NULL) || (handle->initialized == 0U))
    {
        return E_NOK;
    }

    /* STEP 2: Clamp to 1 ms - zero would spin the step loop as fast as the CPU. */
    handle->stepDelayMs = (stepDelayMs == 0U) ? 1U : stepDelayMs;

    return E_OK;
}


STD_ReturnType Stepper_L298P_SetSpeedRpm(Stepper_L298P_HandleType *handle, uint16_h rpm)
{
    uint32_h local_StepsPerRev = 0UL;
    uint32_h local_DelayMs     = 0UL;

    /* STEP 1: Validate the handle and reject a zero speed. */
    if ((handle == NULL) || (handle->initialized == 0U) || (rpm == 0U))
    {
        return E_NOK;
    }

    /* STEP 2: Half-stepping doubles the number of steps in one revolution. */
    local_StepsPerRev = (uint32_h)handle->stepsPerRev;

    if (handle->stepMode == STEPPER_L298P_MODE_HALF)
    {
        local_StepsPerRev *= 2UL;
    }

    /*
     * STEP 3: One revolution takes 60000/rpm milliseconds, shared out over
     *         stepsPerRev steps:
     *             delay = 60000 / (stepsPerRev * rpm)
     */
    local_DelayMs = 60000UL / (local_StepsPerRev * (uint32_h)rpm);

    /*
     * STEP 4: A result of 0 ms means the requested speed is faster than this
     *         millisecond-resolution driver can time - report it instead of
     *         silently running at the wrong speed.
     */
    if (local_DelayMs == 0UL)
    {
        return E_NOK;
    }

    handle->stepDelayMs = (uint16_h)local_DelayMs;

    return E_OK;
}


STD_ReturnType Stepper_L298P_Step(Stepper_L298P_HandleType *handle,
                                  uint16_h steps, Stepper_L298P_DirType dir)
{
    uint16_h local_Step = 0U;

    /* STEP 1: Validate the handle and the direction. */
    if ((handle == NULL) || (handle->initialized == 0U))
    {
        return E_NOK;
    }

    if (dir > STEPPER_L298P_DIR_CCW)
    {
        return E_NOK;
    }

    /* STEP 2: Take one step, wait the step delay, repeat. This blocks. */
    for (local_Step = 0U; local_Step < steps; local_Step++)
    {
        (void)Stepper_L298P_StepOnce(handle, dir);
        Stepper_DelayMs(handle->stepDelayMs);
    }

    /*
     * STEP 3: Return with the coils still energized on the last phase, which is
     *         what holds the load in place. Call Release() to let go.
     */
    return E_OK;
}


STD_ReturnType Stepper_L298P_StepOnce(Stepper_L298P_HandleType *handle,
                                      Stepper_L298P_DirType dir)
{
    uint8_h local_Length = 0U;

    /* STEP 1: Validate the handle and the direction. */
    if ((handle == NULL) || (handle->initialized == 0U))
    {
        return E_NOK;
    }

    if (dir > STEPPER_L298P_DIR_CCW)
    {
        return E_NOK;
    }

    local_Length = Stepper_TableLength(handle->stepMode);

    /*
     * STEP 2: Move one entry along the excitation table. Going backwards adds
     *         (length - 1) instead of subtracting 1, so the unsigned index never
     *         wraps below zero.
     */
    if (dir == STEPPER_L298P_DIR_CW)
    {
        handle->phaseIndex = (uint8_h)((handle->phaseIndex + 1U) % local_Length);
        handle->position++;
    }
    else
    {
        handle->phaseIndex = (uint8_h)((handle->phaseIndex + local_Length - 1U) % local_Length);
        handle->position--;
    }

    /* STEP 3: Energize the coils for the new phase. */
    Stepper_ApplyPattern(handle, Stepper_TableEntry(handle->stepMode, handle->phaseIndex));

    return E_OK;
}


STD_ReturnType Stepper_L298P_RotateAngle(Stepper_L298P_HandleType *handle,
                                         uint16_h degrees, Stepper_L298P_DirType dir)
{
    uint32_h local_StepsPerRev = 0UL;
    uint32_h local_Steps       = 0UL;

    /* STEP 1: Validate the handle. */
    if ((handle == NULL) || (handle->initialized == 0U))
    {
        return E_NOK;
    }

    /* STEP 2: Half-stepping doubles the steps in one revolution. */
    local_StepsPerRev = (uint32_h)handle->stepsPerRev;

    if (handle->stepMode == STEPPER_L298P_MODE_HALF)
    {
        local_StepsPerRev *= 2UL;
    }

    /*
     * STEP 3: steps = degrees * stepsPerRev / 360. The multiply happens first,
     *         in 32-bit, so the division loses as little as possible.
     */
    local_Steps = ((uint32_h)degrees * local_StepsPerRev) / 360UL;

    /* STEP 4: Hand the step count to the blocking stepper. */
    return Stepper_L298P_Step(handle, (uint16_h)local_Steps, dir);
}


STD_ReturnType Stepper_L298P_Hold(Stepper_L298P_HandleType *handle)
{
    /* STEP 1: Validate the handle. */
    if ((handle == NULL) || (handle->initialized == 0U))
    {
        return E_NOK;
    }

    /* STEP 2: If the driver owns the enables, switch the bridge back on. */
    if (handle->useEnablePins != 0U)
    {
        (void)GPIO_SetPinValue(handle->enAPort, handle->enAPin, PIN_HIGH);
        (void)GPIO_SetPinValue(handle->enBPort, handle->enBPin, PIN_HIGH);
    }

    /* STEP 3: Re-apply the current phase - current, but no movement. */
    Stepper_ApplyPattern(handle, Stepper_TableEntry(handle->stepMode, handle->phaseIndex));

    return E_OK;
}


STD_ReturnType Stepper_L298P_Release(Stepper_L298P_HandleType *handle)
{
    /* STEP 1: Validate the handle. */
    if ((handle == NULL) || (handle->initialized == 0U))
    {
        return E_NOK;
    }

    /* STEP 2: Drop all four inputs so no coil is driven. */
    Stepper_ApplyPattern(handle, 0x00U);

    /*
     * STEP 3: If the driver owns ENA/ENB, pull them low too. That disables the
     *         bridge outputs outright, which is the only guaranteed way to be
     *         sure nothing is being driven.
     */
    if (handle->useEnablePins != 0U)
    {
        (void)GPIO_SetPinValue(handle->enAPort, handle->enAPin, PIN_LOW);
        (void)GPIO_SetPinValue(handle->enBPort, handle->enBPin, PIN_LOW);
    }

    handle->energized = 0U;

    return E_OK;
}


STD_ReturnType Stepper_L298P_GetPosition(const Stepper_L298P_HandleType *handle,
                                         sint32 *pPosition)
{
    /* STEP 1: Validate the handle and the output pointer. */
    if ((handle == NULL) || (handle->initialized == 0U) || (pPosition == NULL))
    {
        return E_NOK;
    }

    /* STEP 2: Hand back the net step count. */
    *pPosition = handle->position;

    return E_OK;
}


STD_ReturnType Stepper_L298P_ResetPosition(Stepper_L298P_HandleType *handle)
{
    /* STEP 1: Validate the handle. */
    if ((handle == NULL) || (handle->initialized == 0U))
    {
        return E_NOK;
    }

    /* STEP 2: Declare here to be zero. The phase index is left alone - the rotor
     *         has not moved, only the label on its position has changed. */
    handle->position = 0;

    return E_OK;
}


STD_ReturnType Stepper_L298P_GetStepsPerRev(const Stepper_L298P_HandleType *handle,
                                            uint16_h *pStepsPerRev)
{
    /* STEP 1: Validate the handle and the output pointer. */
    if ((handle == NULL) || (handle->initialized == 0U) || (pStepsPerRev == NULL))
    {
        return E_NOK;
    }

    /* STEP 2: Report the steps per revolution IN THE ACTIVE MODE. */
    if (handle->stepMode == STEPPER_L298P_MODE_HALF)
    {
        *pStepsPerRev = (uint16_h)(handle->stepsPerRev * 2U);
    }
    else
    {
        *pStepsPerRev = handle->stepsPerRev;
    }

    return E_OK;
}
