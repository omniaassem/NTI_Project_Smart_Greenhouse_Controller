#include "Sensors_Driver.h"
#include "../../Service/Bit_Math.h"
#include "../../MCL/ADC/adc_interface.h"
#include "config.h"

STD_ReturnType Sensors_Init(void)
{
    ADC_ConfigType config =
    {
        .uint8ReferenceVoltage = SENSOR_ADC_REFERENCE,
        .uint8Prescaler      = SENSOR_ADC_PRESCALER
    };

    return ADC_Init(&config);
}

STD_ReturnType Sensors_ReadRaw(uint16_h *tempRaw,
                              uint16_h *soilRaw,
                              uint16_h *lightRaw)
{
    if ((tempRaw == NULL) || (soilRaw == NULL) || (lightRaw == NULL))
    {
        return E_NOK;
    }

    if (ADC_ReadChannelBlocking(SENSOR_TEMP_CHANNEL, tempRaw) != E_OK)
    {
        return E_NOK;
    }

    if (ADC_ReadChannelBlocking(SENSOR_SOIL_CHANNEL, soilRaw) != E_OK)
    {
        return E_NOK;
    }

    if (ADC_ReadChannelBlocking(SENSOR_LIGHT_CHANNEL, lightRaw) != E_OK)
    {
        return E_NOK;
    }

    return E_OK;
}

STD_ReturnType Sensors_ScaleTempC(uint16_h raw, uint8_h *tempC)
{
    if (tempC == NULL)
    {
        return E_NOK;
    }

    uint32_h scaled = ((uint32_h)raw * 50UL + 511UL) / 1023UL;
    *tempC = (uint8_h)scaled;

    return E_OK;
}

STD_ReturnType Sensors_ScalePct(uint16_h raw, uint8_h *percent)
{
    if (percent == NULL)
    {
        return E_NOK;
    }

    uint32_h scaled = ((uint32_h)raw * 100UL + 511UL) / 1023UL;
    *percent = (uint8_h)scaled;

    return E_OK;
}
