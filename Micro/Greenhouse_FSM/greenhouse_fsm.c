#include "greenhouse_fsm.h"
#include "../../Service/STD_Types.h"
#include "control.h"
#include "console.h"
#include "../../HAL/Actuators/Actuators_Driver.h"
#include "../../HAL/Sensors/Sensors_Driver.h"

#include <avr/io.h>
#include <util/delay.h>

/* تعريف أطراف الأزرار بناءً على الرسمة (Active-Low) على PORTD */
#define BTN_ALARM_PIN   PD2
#define BTN_MODE_PIN    PD3
#define BTN_SAVE_PIN    PD4

#define CRITICAL_HIGH_TEMP_C      45u
#define CRITICAL_LOW_SOIL_PCT     10u

static FSM_State_t currentState = FSM_STATE_INIT;

/* متغيرات لمتابعة حالة الأزرار ومنع التكرار (Debounce & Edge Detection) */
static uint8_h lastModeBtnState = 1;
static uint8_h lastAlarmBtnState = 1;
static uint8_h lastSaveBtnState = 1;

/* متغيرات لحفظ حالة التشغيل اليدوي (Toggle States) */
static ActuatorStateType manualAlarmState = ACT_STATE_OFF;
static ActuatorStateType manualLampState = ACT_STATE_OFF;

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

    /* تهيئة أطراف الأزرار كمدخلات Inputs على PORTD مع تفعيل الـ Pull-up */
    DDRD &= ~((1 << BTN_ALARM_PIN) | (1 << BTN_MODE_PIN) | (1 << BTN_SAVE_PIN));
    PORTD |= (1 << BTN_ALARM_PIN) | (1 << BTN_MODE_PIN) | (1 << BTN_SAVE_PIN);

    currentState = FSM_STATE_AUTO;
    return FSM_OK;
}

FSM_Status_t FSM_Run(void)
{
    (void)CON_Process();

    /* قراءة زر الـ Mode للتبديل بين Auto و Manual */
    uint8_h currentModeBtnState = (PIND & (1 << BTN_MODE_PIN)) ? 1 : 0;
    if (currentModeBtnState == 0 && lastModeBtnState == 1)
    {
        if (currentState == FSM_STATE_AUTO) {
            currentState = FSM_STATE_MANUAL;
        } else if (currentState == FSM_STATE_MANUAL) {
            currentState = FSM_STATE_AUTO;
        }
        _delay_ms(50);
    }
    lastModeBtnState = currentModeBtnState;

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
            /* التحكم اليدوي بنظام التبديل (Toggle) للـ Alarm على B3 */
            {
                uint8_h currentAlarmBtn = (PIND & (1 << BTN_ALARM_PIN)) ? 1 : 0;
                if (currentAlarmBtn == 0 && lastAlarmBtnState == 1)
                {
                    manualAlarmState = (manualAlarmState == ACT_STATE_ON) ? ACT_STATE_OFF : ACT_STATE_ON;
                    _delay_ms(50);
                }
                lastAlarmBtnState = currentAlarmBtn;
                (void)ACT_Set(ACTUATOR_ALARM, manualAlarmState);
            }

            /* التحكم اليدوي بنظام التبديل (Toggle) للـ Lamp على B2 */
            {
                uint8_h currentSaveBtn = (PIND & (1 << BTN_SAVE_PIN)) ? 1 : 0;
                if (currentSaveBtn == 0 && lastSaveBtnState == 1)
                {
                    manualLampState = (manualLampState == ACT_STATE_ON) ? ACT_STATE_OFF : ACT_STATE_ON;
                    _delay_ms(50);
                }
                lastSaveBtnState = currentSaveBtn;
                (void)ACT_Set(ACTUATOR_LAMP, manualLampState);
            }
            break;

        case FSM_STATE_ALARM:
            (void)ACT_Set(ACTUATOR_PUMP, ACT_STATE_OFF);
            (void)ACT_Set(ACTUATOR_FAN, ACT_STATE_ON);
            (void)ACT_Set(ACTUATOR_LAMP, ACT_STATE_OFF);
            (void)ACT_Set(ACTUATOR_ALARM, ACT_STATE_ON);
            
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