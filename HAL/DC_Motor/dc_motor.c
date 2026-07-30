#include "../../Service/STD_Types.h"
#include "../../Service/Bit_Math.h"
#include "../../MCAL/GPIO/gpio_interface.h"
#include "dc_motor.h"

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

STD_ReturnType DC_Motor_Init(DC_MotorHandleType *handle)
{
    if (handle == NULL)
    {
        return E_NOK;
    }

    if ((handle->in1Port >= GPIO_NUMBER_OF_PORTS) || (handle->in2Port >= GPIO_NUMBER_OF_PORTS))
    {
        return E_NOK;
    }

    (void)GPIO_SetPinDirection(handle->in1Port, handle->in1Pin, GPIO_OUTPUT);
    (void)GPIO_SetPinDirection(handle->in2Port, handle->in2Pin, GPIO_OUTPUT);
    (void)GPIO_SetPinValue(handle->in1Port, handle->in1Pin, PIN_LOW);
    (void)GPIO_SetPinValue(handle->in2Port, handle->in2Pin, PIN_LOW);

    handle->initialized = 1U;
    DC_Motor_ApplyState(handle, DC_MOTOR_STATE_STOP);

    return E_OK;
}

STD_ReturnType DC_Motor_Forward(DC_MotorHandleType *handle)
{
    if ((handle == NULL) || (handle->initialized == 0U))
    {
        return E_NOK;
    }

    DC_Motor_ApplyState(handle, DC_MOTOR_STATE_FORWARD);
    return E_OK;
}

STD_ReturnType DC_Motor_Backward(DC_MotorHandleType *handle)
{
    if ((handle == NULL) || (handle->initialized == 0U))
    {
        return E_NOK;
    }

    DC_Motor_ApplyState(handle, DC_MOTOR_STATE_BACKWARD);
    return E_OK;
}

STD_ReturnType DC_Motor_SetDirection(DC_MotorHandleType *handle, DC_MotorDirectionType dir)
{
    if ((handle == NULL) || (handle->initialized == 0U))
    {
        return E_NOK;
    }

    if (dir > DC_MOTOR_DIR_BACKWARD)
    {
        return E_NOK;
    }

    return (dir == DC_MOTOR_DIR_FORWARD) ? DC_Motor_Forward(handle) : DC_Motor_Backward(handle);
}

STD_ReturnType DC_Motor_Stop(DC_MotorHandleType *handle)
{
    if ((handle == NULL) || (handle->initialized == 0U))
    {
        return E_NOK;
    }

    DC_Motor_ApplyState(handle, DC_MOTOR_STATE_STOP);
    return E_OK;
}

STD_ReturnType DC_Motor_Brake(DC_MotorHandleType *handle)
{
    if ((handle == NULL) || (handle->initialized == 0U))
    {
        return E_NOK;
    }

    DC_Motor_ApplyState(handle, DC_MOTOR_STATE_BRAKE);
    return E_OK;
}

STD_ReturnType DC_Motor_GetState(const DC_MotorHandleType *handle, DC_MotorStateType *pState)
{
    if ((handle == NULL) || (handle->initialized == 0U) || (pState == NULL))
    {
        return E_NOK;
    }

    *pState = handle->state;
    return E_OK;
}

STD_ReturnType DC_Motor_DeInit(DC_MotorHandleType *handle)
{
    if ((handle == NULL) || (handle->initialized == 0U))
    {
        return E_NOK;
    }

    (void)DC_Motor_Stop(handle);
    handle->initialized = 0U;

    return E_OK;
}