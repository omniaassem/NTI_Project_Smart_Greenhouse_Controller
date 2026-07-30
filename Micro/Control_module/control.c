#include "control.h"
#include "../../Service/STD_TYPES.h"
#include "../../HAL/Actuators/Actuators_Driver.h"
#include "../../HAL/Sensors/Sensors_Driver.h"

/* العتبات المحددة للتحكم (Thresholds with Hysteresis) */
#define TEMP_HIGH_THRESHOLD_C     30u  /* تشغيل المروحة/التبريد عند تجاوز 30°C */
#define TEMP_LOW_THRESHOLD_C      26u  /* إيقاف التبريد عند الانخفاض تحت 26°C */

#define SOIL_LOW_THRESHOLD_PCT    30u  /* تشغيل مضخة الري عند انخفاض الرطوبة عن 30% */
#define SOIL_HIGH_THRESHOLD_PCT   60u  /* إيقاف المضخة عند وصول الرطوبة إلى 60% */

#define LIGHT_LOW_THRESHOLD_PCT   40u  /* تشغيل الإضاءة الاصطناعية عند انخفاض الضوء عن 40% */
#define LIGHT_HIGH_THRESHOLD_PCT  70u  /* إيقاف الإضاءة عند ارتفاع الضوء إلى 70% */

CONTROL_Status_t CTRL_Init(void)
{
    /* تهيئة وحدة المشغلات (Actuators) */
    if (Actuators_Init() != E_OK)
    {
        return CONTROL_ERROR;
    }

    /* تهيئة وحدة الحساسات (Sensors) */
    if (Sensors_Init() != E_OK)
    {
        return CONTROL_ERROR;
    }

    return CONTROL_OK;
}

CONTROL_Status_t CTRL_Update(void)
{
    u16 rawTemp = 0u;
    u16 rawSoil = 0u;
    u16 rawLight = 0u;

    u8 tempC = 0u;
    u8 soilPct = 0u;
    u8 lightPct = 0u;

    /* 1. قراءة القيم الخام من الحساسات */
    if (Sensors_ReadRaw(&rawTemp, &rawSoil, &rawLight) != E_OK)
    {
        return CONTROL_ERROR;
    }

    /* 2. تحويل القيم الخام إلى وحدات فيزيائية ونسب مئوية */
    (void)Sensors_ScaleTempC(rawTemp, &tempC);
    (void)Sensors_ScalePct(rawSoil, &soilPct);
    (void)Sensors_ScalePct(rawLight, &lightPct);

    /* 3. تطبيق منطق التحكم بالحرارة (Fan / Cooling Control) */
    if (tempC >= TEMP_HIGH_THRESHOLD_C)
    {
        (void)Actuators_SetFanState(ACTUATOR_STATE_ON);
    }
    else if (tempC <= TEMP_LOW_THRESHOLD_C)
    {
        (void)Actuators_SetFanState(ACTUATOR_STATE_OFF);
    }

    /* 4. تطبيق منطق التحكم برطوبة التربة (Pump / Irrigation Control) */
    if (soilPct <= SOIL_LOW_THRESHOLD_PCT)
    {
        (void)Actuators_SetPumpState(ACTUATOR_STATE_ON);
    }
    else if (soilPct >= SOIL_HIGH_THRESHOLD_PCT)
    {
        (void)Actuators_SetPumpState(ACTUATOR_STATE_OFF);
    }

    /* 5. تطبيق منطق التحكم بالإضاءة (Lighting Control) */
    if (lightPct <= LIGHT_LOW_THRESHOLD_PCT)
    {
        (void)Actuators_SetLightState(ACTUATOR_STATE_ON);
    }
    else if (lightPct >= LIGHT_HIGH_THRESHOLD_PCT)
    {
        (void)Actuators_SetLightState(ACTUATOR_STATE_OFF);
    }

    return CONTROL_OK;
}
