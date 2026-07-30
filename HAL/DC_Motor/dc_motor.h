#ifndef DC_MOTOR_H
#define DC_MOTOR_H

#include "../../Service/STD_Types.h"
#include "../../MCAL/GPIO/gpio_interface.h"

typedef enum
{
    DC_MOTOR_DIR_FORWARD  = 0,
    DC_MOTOR_DIR_BACKWARD = 1
} DC_MotorDirectionType;

typedef enum
{
    DC_MOTOR_STATE_STOP     = 0,
    DC_MOTOR_STATE_FORWARD  = 1,
    DC_MOTOR_STATE_BACKWARD = 2,
    DC_MOTOR_STATE_BRAKE    = 3
} DC_MotorStateType;

typedef struct
{
    uint8_h in1Port;  uint8_h in1Pin;
    uint8_h in2Port;  uint8_h in2Pin;
    uint8_h invertDirection;

    uint8_h             initialized;
    DC_MotorStateType   state;
} DC_MotorHandleType;

STD_ReturnType DC_Motor_Init(DC_MotorHandleType *handle);
STD_ReturnType DC_Motor_Forward(DC_MotorHandleType *handle);
STD_ReturnType DC_Motor_Backward(DC_MotorHandleType *handle);
STD_ReturnType DC_Motor_SetDirection(DC_MotorHandleType *handle, DC_MotorDirectionType dir);
STD_ReturnType DC_Motor_Stop(DC_MotorHandleType *handle);
STD_ReturnType DC_Motor_Brake(DC_MotorHandleType *handle);
STD_ReturnType DC_Motor_GetState(const DC_MotorHandleType *handle, DC_MotorStateType *pState);
STD_ReturnType DC_Motor_DeInit(DC_MotorHandleType *handle);

#endif /* DC_MOTOR_H */