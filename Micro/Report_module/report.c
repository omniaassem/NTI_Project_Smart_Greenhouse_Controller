#include "report.h"
#include "../../Service/STD_Types.h"
#include "../../MCAL/UART/uart_interface.h"
#include "../Sensors/Sensors_Driver.h"
#include "../../HAL/Actuators/Actuators_Driver.h"
#include "../Greenhouse_FSM/greenhouse_fsm.h"
#include <stdio.h>

static void RPT_SendString(const char *str)
{
    while (*str != '\0')
    {
        UART_SendByte((uint8_h)*str);
        str++;
    }
}

static const char* RPT_GetStateName(FSM_State_t state)
{
    switch (state)
    {
        case FSM_STATE_INIT:   return "INIT";
        case FSM_STATE_AUTO:   return "AUTO";
        case FSM_STATE_MANUAL: return "MANUAL";
        case FSM_STATE_ALARM:  return "ALARM";
        case FSM_STATE_CONFIG: return "CONFIG";
        default:               return "UNKNOWN";
    }
}

REPORT_Status_t RPT_Init(void)
{
    RPT_SendString("\r\n[REPORT] Telemetry Subsystem Initialized.\r\n");
    return REPORT_OK;
}

REPORT_Status_t RPT_SendStatus(void)
{
    uint16_h rawTemp = 0, rawSoil = 0, rawLight = 0;
    uint8_h tempC = 0, soilPct = 0, lightPct = 0;

    ActuatorStateType fanState = ACT_STATE_OFF;
    ActuatorStateType pumpState = ACT_STATE_OFF;
    ActuatorStateType lightState = ACT_STATE_OFF;
    ActuatorStateType alarmState = ACT_STATE_OFF;

    if (Sensors_ReadRaw(&rawTemp, &rawSoil, &rawLight) != E_OK)
    {
        return REPORT_ERROR;
    }

    (void)Sensors_ScaleTempC(rawTemp, &tempC);
    (void)Sensors_ScalePct(rawSoil, &soilPct);
    (void)Sensors_ScalePct(lightPct, &lightPct);

    (void)ACT_Get(ACTUATOR_FAN, &fanState);
    (void)ACT_Get(ACTUATOR_PUMP, &pumpState);
    (void)ACT_Get(ACTUATOR_LAMP, &lightState);
    (void)ACT_Get(ACTUATOR_ALARM, &alarmState);

    FSM_State_t fsmState = FSM_GetState();

    char reportBuffer[200];
    snprintf(reportBuffer, sizeof(reportBuffer),
             "\r\n--- [GREENHOUSE TELEMETRY REPORT] ---\r\n"
             "FSM Mode : %s\r\n"
             "Sensors  : Temp=%u C | Soil=%u%% | Light=%u%%\r\n"
             "Actuators: Fan=%s | Pump=%s | Lamp=%s | Alarm=%s\r\n"
             "-------------------------------------\r\n",
             RPT_GetStateName(fsmState),
             tempC, soilPct, lightPct,
             (fanState == ACT_STATE_ON) ? "ON" : "OFF",
             (pumpState == ACT_STATE_ON) ? "ON" : "OFF",
             (lightState == ACT_STATE_ON) ? "ON" : "OFF",
             (alarmState == ACT_STATE_ON) ? "ON" : "OFF");

    RPT_SendString(reportBuffer);

    return REPORT_OK;
}