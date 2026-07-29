#include "../../Service/STD_Types.h"
#include "../../Service/Bit_Math.h"
#include "adc_registers.h"
#include "adc_interface.h"

/*
 * Initializes the ADC peripheral: selects the voltage reference, selects the
 * clock prescaler, and enables the module.
 */
STD_ReturnType ADC_Init(const ADC_ConfigType *addConfig)
{
    STD_ReturnType local_Status = E_OK;

    if (addConfig == NULL)
    {
        local_Status = E_NOK;
    }
    else
    {
        /* Reference voltage (REFS1:REFS0) */
        CLR_BIT(ADC_ADMUX_REG, ADC_REFS0_BIT);
        CLR_BIT(ADC_ADMUX_REG, ADC_REFS1_BIT);
        if (GET_BIT(addConfig->uint8ReferenceVoltage, 0))
        {
            SET_BIT(ADC_ADMUX_REG, ADC_REFS0_BIT);
        }
        if (GET_BIT(addConfig->uint8ReferenceVoltage, 1))
        {
            SET_BIT(ADC_ADMUX_REG, ADC_REFS1_BIT);
        }

        /* Right-adjusted result */
        CLR_BIT(ADC_ADMUX_REG, ADC_ADLAR_BIT);

        /* Default channel 0 */
        CLR_BIT(ADC_ADMUX_REG, ADC_MUX0_BIT);
        CLR_BIT(ADC_ADMUX_REG, ADC_MUX1_BIT);
        CLR_BIT(ADC_ADMUX_REG, ADC_MUX2_BIT);
        CLR_BIT(ADC_ADMUX_REG, ADC_MUX3_BIT);
        CLR_BIT(ADC_ADMUX_REG, ADC_MUX4_BIT);

        /* Clock prescaler (ADPS2:ADPS0) */
        CLR_BIT(ADC_ADCSRA_REG, ADC_ADPS0_BIT);
        CLR_BIT(ADC_ADCSRA_REG, ADC_ADPS1_BIT);
        CLR_BIT(ADC_ADCSRA_REG, ADC_ADPS2_BIT);
        if (GET_BIT(addConfig->uint8Prescaler, 0))
        {
            SET_BIT(ADC_ADCSRA_REG, ADC_ADPS0_BIT);
        }
        if (GET_BIT(addConfig->uint8Prescaler, 1))
        {
            SET_BIT(ADC_ADCSRA_REG, ADC_ADPS1_BIT);
        }
        if (GET_BIT(addConfig->uint8Prescaler, 2))
        {
            SET_BIT(ADC_ADCSRA_REG, ADC_ADPS2_BIT);
        }

        /* Enable the ADC module */
        SET_BIT(ADC_ADCSRA_REG, ADC_ADEN_BIT);
    }

    return local_Status;
}

/*
 * Disables the ADC peripheral (powers it down / stops conversions).
 */
STD_ReturnType ADC_DeInit(void)
{
    CLR_BIT(ADC_ADCSRA_REG, ADC_ADEN_BIT);

    return E_OK;
}

/*
 * Selects the given channel and starts a single conversion (non-blocking).
 */
STD_ReturnType ADC_StartConversion(uint8_h uint8Channel)
{
    STD_ReturnType local_Status = E_OK;

    if (uint8Channel >= ADC_NUMBER_OF_CHANNELS)
    {
        local_Status = E_NOK;
    }
    else
    {
        CLR_BIT(ADC_ADMUX_REG, ADC_MUX0_BIT);
        CLR_BIT(ADC_ADMUX_REG, ADC_MUX1_BIT);
        CLR_BIT(ADC_ADMUX_REG, ADC_MUX2_BIT);
        CLR_BIT(ADC_ADMUX_REG, ADC_MUX3_BIT);
        CLR_BIT(ADC_ADMUX_REG, ADC_MUX4_BIT);
        if (GET_BIT(uint8Channel, 0))
        {
            SET_BIT(ADC_ADMUX_REG, ADC_MUX0_BIT);
        }
        if (GET_BIT(uint8Channel, 1))
        {
            SET_BIT(ADC_ADMUX_REG, ADC_MUX1_BIT);
        }
        if (GET_BIT(uint8Channel, 2))
        {
            SET_BIT(ADC_ADMUX_REG, ADC_MUX2_BIT);
        }

        SET_BIT(ADC_ADCSRA_REG, ADC_ADSC_BIT);
    }

    return local_Status;
}

/*
 * Reports whether the ADC has finished the conversion that was last started.
 */
uint8_h ADC_IsConversionComplete(void)
{
    uint8_h uint8Status = ADC_CONVERSION_DONE;

    if (GET_BIT(ADC_ADCSRA_REG, ADC_ADSC_BIT))
    {
        uint8Status = ADC_CONVERSION_BUSY;
    }

    return uint8Status;
}

/*
 * Reads the 10-bit result of the last completed conversion.
 */
STD_ReturnType ADC_ReadResult(uint16_h *puint16Result)
{
    STD_ReturnType local_Status = E_OK;

    if (puint16Result == NULL)
    {
        local_Status = E_NOK;
    }
    else
    {
        uint8_h uint8Low  = ADC_ADCL_REG;
        uint8_h uint8High = ADC_ADCH_REG;

        *puint16Result = (uint16_h)(((uint16_h)uint8High << 8) | uint8Low);
    }

    return local_Status;
}

/*
 * Blocking read: starts a conversion on the given channel, busy-waits until it
 * finishes, then returns the result.
 */
STD_ReturnType ADC_ReadChannelBlocking(uint8_h uint8Channel, uint16_h *puint16Result)
{
    STD_ReturnType local_Status = ADC_StartConversion(uint8Channel);

    if (local_Status == E_OK)
    {
        while (ADC_IsConversionComplete() == ADC_CONVERSION_BUSY)
        {
            /* wait */
        }

        local_Status = ADC_ReadResult(puint16Result);
    }

    return local_Status;
}
