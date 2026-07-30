#include "greenhouse_fsm.h"
#include "../../Service/STD_Types.h"
#include "../Control/control.h"
#include "../Console/console.h"
#include "../../HAL/Actuators/Actuators_Driver.h"
#include "../../HAL/Sensors/Sensors_Driver.h"

/* حدود الإنذار (Alarm Thresholds) */
#define CRITICAL_HIGH_TEMP_C      45u  /* درجة حرارة حرجة للإنذار */
#define CRITICAL_LOW_SOIL_PCT     10u  /* جفاف حاد في التربة للإنذار */

static FSM_State_t currentState = FSM_STATE_INIT;

/* دالة مساعدة للتحقق من وجود ظروف إنذار حادة */
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
            return 1u; /* يوجد ظروف إنذار */
        }
    }
    return 0u;
}

FSM_Status_t FSM_Init(void)
{
    /* تهيئة وحدة التحكم والمكونات الأساسية */
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
    /* معالجة أوامر الـ Console بشكل مستمر */
    (void)CON_Process();

    /* التحقق المستمر من الإنذارات الحرجة للتحويل التلقائي لحالة ALARM */
    if (currentState != FSM_STATE_ALARM && FSM_CheckAlarmConditions())
    {
        currentState = FSM_STATE_ALARM;
    }

    switch (currentState)
    {
        case FSM_STATE_AUTO:
            /* نمط التلقائي: تنفيذ حلقة التحكم بالأوتوماتيك من موديل CTRL */
            if (CTRL_Update() != CONTROL_OK)
            {
                /* في حال وجود خطأ في القراءة يتحول إلى الإنذار */
                currentState = FSM_STATE_ALARM;
            }
            break;

        case FSM_STATE_MANUAL:
            /* نمط التحكم اليدوي: يتم استقبال الأوامر وتغيير المشغلات مباشرة عبر הـ Console */
            break;

        case FSM_STATE_ALARM:
            /* نمط الإنذار: إيقاف المكونات الخطرة وتفعيل حماية النظام */
            (void)Actuators_SetPumpState(ACTUATOR_STATE_OFF);
            (void)Actuators_SetFanState(ACTUATOR_STATE_ON); /* تشغيل المروحة بأقصى طاقة للتبريد */
            (void)Actuators_SetLightState(ACTUATOR_STATE_OFF);
            
            /* إذا زالت أسباب الإنذار، يمكن العودة تلقائياً للنمط التلقائي */
            if (!FSM_CheckAlarmConditions())
            {
                currentState = FSM_STATE_AUTO;
            }
            break;

        case FSM_STATE_CONFIG:
            /* نمط الإعدادات: انتظار إدخال قيم العتبات الجديدة عبر الـ Console */
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
