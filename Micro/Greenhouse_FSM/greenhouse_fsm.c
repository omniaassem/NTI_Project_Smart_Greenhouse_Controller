#include "greenhouse_fsm.h"
#include "../../Service/STD_Types.h"
#include "control.h"
#include "console.h"
#include "../../HAL/Actuators/Actuators_Driver.h"
#include "../../HAL/Sensors/Sensors_Driver.h"

#define CRITICAL_HIGH_TEMP_C      45u
#define CRITICAL_LOW_SOIL_PCT     10u

static FSM_State_t currentState = FSM_STATE_INIT;

static uint8_h FSM_CheckAlarmConditions(void)
{
    uint16_h rawTemp = 0, rawSoil = 0, rawLight = 0;
    uint8_h tempC = 0, soilPct = 0, lightPct = 0;

    if (Sensors_ReadRaw(&rawTemp, &rawSoil, &rawLight) == E_OK)
    {
        (void)Sensors_ScaleTempC(rawTemp, &tempC);
        (void)Sensors_ScalePct(rawSoil, &soilPct);

        if (tempC >= CRITICAL_HIGH_TEMP_C || soilPct <= CRITICAL_LOW_SOIL_PCT)
        {
            return 1u;
        }
    }
    return 0u;
}

FSM_Status_t FSM_Init(void)
{
    if (CTRL_Init() != CONTROL_OK)
    {
        return FSM_ERROR;
    }

    if (CON_Init() != CONSOLE_OK)
    {
        return FSM_ERROR;
    }

    currentState = FSM_STATE_AUTO;
    return FSM_OK;
}

FSM_Status_t FSM_Run(void)
{
    (void)CON_Process();

    if (currentState != FSM_STATE_ALARM && FSM_CheckAlarmConditions())
    {
        currentState = FSM_STATE_ALARM;
    }

    switch (currentState)
    {
        case FSM_STATE_AUTO:
            if (CTRL_Update() != CONTROL_OK)
            {
                currentState = FSM_STATE_ALARM;
            }
            break;

        case FSM_STATE_MANUAL:
            break;

        case FSM_STATE_ALARM:
            (void)ACT_Set(ACTUATOR_PUMP, ACT_STATE_OFF);
            (void)ACT_Set(ACTUATOR_FAN, ACT_STATE_ON);
            (void)ACT_Set(ACTUATOR_LAMP, ACT_STATE_OFF);
            
            if (!FSM_CheckAlarmConditions())
            {
                currentState = FSM_STATE_AUTO;
            }
            break;

        case FSM_STATE_CONFIG:
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

FSM_Status_t FSM_SetState(FSM_State_t newState)
{
    if (newState > FSM_STATE_CONFIG)
    {
        return FSM_ERROR;
    }
    currentState = newState;
    return FSM_OK;
}