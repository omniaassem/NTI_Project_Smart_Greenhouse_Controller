#include "report.h"
#include "../../Service/STD_Types.h"
#include "../../MCL/UART/uart_interface.h"
#include "../Sensors/Sensors_Driver.h"
#include "../../HAL/Actuators/Actuators_Driver.h"
#include "../FSM/greenhouse_fsm.h"
#include <stdio.h>

/* دالة مساعدة لإرسال السلاسل النصية عبر UART */
static void RPT_SendString(const char *str)
{
    while (*str != '\0')
    {
        UART_SendByte((uint8_h)*str);
        str++;
    }
}

/* تحويل حالة FSM إلى نص قابل للطباعة */
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

    Actuator_State_t fanState = ACTUATOR_STATE_OFF;
    Actuator_State_t pumpState = ACTUATOR_STATE_OFF;
    Actuator_State_t lightState = ACTUATOR_STATE_OFF;

    /* 1. قراءة بيانات الحساسات */
    if (Sensors_ReadRaw(&rawTemp, &rawSoil, &rawLight) != E_OK)
    {
        return REPORT_ERROR;
    }

    (void)Sensors_ScaleTempC(rawTemp, &tempC);
    (void)Sensors_ScalePct(rawSoil, &soilPct);
    (void)Sensors_ScalePct(rawLight, &lightPct);

    /* 2. قراءة حالات المشغلات */
    (void)Actuators_GetFanState(&fanState);
    (void)Actuators_GetPumpState(&pumpState);
    (void)Actuators_GetLightState(&lightState);

    /* 3. قراءة حالة آلة الحالات (FSM) */
    FSM_State_t fsmState = FSM_GetState();

    /* 4. تنسيق التقرير وإرساله عبر الـ UART */
    char reportBuffer[128];
    snprintf(reportBuffer, sizeof(reportBuffer),
             "\r\n--- [GREENHOUSE TELEMETRY REPORT] ---\r\n"
             "FSM Mode : %s\r\n"
             "Sensors  : Temp=%u C | Soil=%u%% | Light=%u%%\r\n"
             "Actuators: Fan=%s | Pump=%s | Light=%s\r\n"
             "-------------------------------------\r\n",
             RPT_GetStateName(fsmState),
             tempC, soilPct, lightPct,
             (fanState == ACTUATOR_STATE_ON) ? "ON" : "OFF",
             (pumpState == ACTUATOR_STATE_ON) ? "ON" : "OFF",
             (lightState == ACTUATOR_STATE_ON) ? "ON" : "OFF");

    RPT_SendString(reportBuffer);

    return REPORT_OK;
}
