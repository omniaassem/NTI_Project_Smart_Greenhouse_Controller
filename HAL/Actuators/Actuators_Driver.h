#ifndef ACTUATORS_DRIVER_H
#define ACTUATORS_DRIVER_H

#include "../../Service/STD_Types.h"

typedef enum
{
    ACTUATOR_FAN = 0,
    ACTUATOR_PUMP,
    ACTUATOR_LAMP,
    ACTUATOR_ALARM,
    ACTUATOR_COUNT
} ActuatorType;

typedef enum
{
    ACT_STATE_OFF = 0,
    ACT_STATE_ON = 1
} ActuatorStateType;

STD_ReturnType ACT_Init(void);
STD_ReturnType ACT_Set(ActuatorType actuator, ActuatorStateType state);
STD_ReturnType ACT_SetAll(ActuatorStateType fanState,
                          ActuatorStateType pumpState,
                          ActuatorStateType lampState,
                          ActuatorStateType alarmState);
STD_ReturnType ACT_Get(ActuatorType actuator, ActuatorStateType *pState);

#endif /* ACTUATORS_DRIVER_H */
