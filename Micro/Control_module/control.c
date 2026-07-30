#include "control.h"
#include "../../Service/STD_TYPES.h"
#include "../../HAL/Actuators/Actuators_Driver.h"
#include "../../HAL/Sensors/Sensors_Driver.h"

#define TEMP_HIGH_THRESHOLD_C     30u
#define TEMP_LOW_THRESHOLD_C      26u

#define SOIL_LOW_THRESHOLD_PCT    30u
#define SOIL_HIGH_THRESHOLD_PCT   60u

#define LIGHT_LOW_THRESHOLD_PCT   40u
#define LIGHT_HIGH_THRESHOLD_PCT  70u

CONTROL_Status_t CTRL_Init(void)
{
    if (ACT_Init() != E_OK)
    {
        return CONTROL_ERROR;
    }

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

    if (Sensors_ReadRaw(&rawTemp, &rawSoil, &rawLight) != E_OK)
    {
        return CONTROL_ERROR;
    }

    (void)Sensors_ScaleTempC(rawTemp, &tempC);
    (void)Sensors_ScalePct(rawSoil, &soilPct);
    (void)Sensors_ScalePct(rawLight, &lightPct);

    if (tempC >= TEMP_HIGH_THRESHOLD_C)
    {
        (void)ACT_Set(ACTUATOR_FAN, ACT_STATE_ON);
    }
    else if (tempC <= TEMP_LOW_THRESHOLD_C)
    {
        (void)ACT_Set(ACTUATOR_FAN, ACT_STATE_OFF);
    }

    if (soilPct <= SOIL_LOW_THRESHOLD_PCT)
    {
        (void)ACT_Set(ACTUATOR_PUMP, ACT_STATE_ON);
    }
    else if (soilPct >= SOIL_HIGH_THRESHOLD_PCT)
    {
        (void)ACT_Set(ACTUATOR_PUMP, ACT_STATE_OFF);
    }

    if (lightPct <= LIGHT_LOW_THRESHOLD_PCT)
    {
        (void)ACT_Set(ACTUATOR_LAMP, ACT_STATE_ON);
    }
    else if (lightPct >= LIGHT_HIGH_THRESHOLD_PCT)
    {
        (void)ACT_Set(ACTUATOR_LAMP, ACT_STATE_OFF);
    }

    return CONTROL_OK;
}