#include "greenhouse_fsm.h"
#include "../../Service/STD_Types.h"

static FSM_State_t currentState = FSM_STATE_INIT;

FSM_Status_t FSM_Init(void)
{
    currentState = FSM_STATE_AUTO;
    return FSM_OK;
}

FSM_Status_t FSM_Run(void)
{
    switch (currentState)
    {
        case FSM_STATE_AUTO:
            /* Automatic greenhouse control logic */
            break;

        case FSM_STATE_MANUAL:
            /* Manual control logic */
            break;

        case FSM_STATE_ALARM:
            /* Alarm handling logic */
            break;

        case FSM_STATE_CONFIG:
            /* Configuration mode logic */
            break;

        default:
            currentState = FSM_STATE_AUTO;
            break;
    }
    return FSM_OK;
}

FSM_State_t FSM_GetState(void)
{
    return currentState;
}
