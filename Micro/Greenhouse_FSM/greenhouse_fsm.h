#ifndef MICRO_GREENHOUSE_FSM_H
#define MICRO_GREENHOUSE_FSM_H

#include "../../Service/STD_Types.h"

typedef enum {
    FSM_STATE_INIT = 0,
    FSM_STATE_AUTO,
    FSM_STATE_MANUAL,
    FSM_STATE_ALARM,
    FSM_STATE_CONFIG
} FSM_State_t;

typedef enum {
    FSM_OK = 0,
    FSM_ERROR
} FSM_Status_t;

FSM_Status_t FSM_Init(void);
FSM_Status_t FSM_Run(void);
FSM_State_t FSM_GetState(void);

#endif // MICRO_GREENHOUSE_FSM_H
