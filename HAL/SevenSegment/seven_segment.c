#include "../../Service/STD_Types.h"
#include "../../Service/Bit_Math.h"
#include "../../MCL/GPIO/gpio_interface.h"
#include "seven_segment_interface.h"

/* ================================================================================
 *  SEVEN-SEGMENT DISPLAY DRIVER - IMPLEMENTATION SKELETON
 *  ------------------------------------------------------------------------------
 *  Each body lists the ordered steps to implement the function. Replace the
 *  numbered comments with the actual GPIO calls.
 * ============================================================================== */

/*
 * Segment lookup table for DIRECT mode (common cathode; 1 = segment ON).
 * Bit order per entry: bit0=a, bit1=b, ... bit6=g (bit7=DP, unused here).
 * TODO: define this table, e.g.
 *   static const uint8_h SevenSeg_DigitTable[10] = {
 *       0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F };
 * For a common-anode display, invert each pattern (~value) before driving.
 */
static const uint8_h SevenSeg_DigitTable[10] =
{
    0x3F, /* 0 */
    0x06, /* 1 */
    0x5B, /* 2 */
    0x4F, /* 3 */
    0x66, /* 4 */
    0x6D, /* 5 */
    0x7D, /* 6 */
    0x07, /* 7 */
    0x7F, /* 8 */
    0x6F  /* 9 */
};


STD_ReturnType SevenSeg_Init(const SevenSeg_ConfigType *addConfig)
{
    uint8_h pinCount;
    uint8_h pinIndex;

    if ((addConfig == NULL) || (addConfig->dataPort >= GPIO_NUMBER_OF_PORTS))
    {
        return E_NOK;
    }

    if (addConfig->connection == SEVEN_SEG_CONNECTION_DIRECT)
    {
        pinCount = 7U;
    }
    else if (addConfig->connection == SEVEN_SEG_CONNECTION_BCD)
    {
        pinCount = 4U;
    }
    else
    {
        return E_NOK;
    }

    if ((addConfig->startPin >= GPIO_NUMBER_OF_PINS) ||
        (addConfig->startPin + pinCount > GPIO_NUMBER_OF_PINS))
    {
        return E_NOK;
    }

    for (pinIndex = 0U; pinIndex < pinCount; ++pinIndex)
    {
        if (GPIO_SetPinDirection(addConfig->dataPort,
                                 addConfig->startPin + pinIndex,
                                 GPIO_OUTPUT) != E_OK)
        {
            return E_NOK;
        }
    }

    if (SevenSeg_Clear(addConfig) != E_OK)
    {
        return E_NOK;
    }

    return E_OK;
}


STD_ReturnType SevenSeg_DisplayDigit(const SevenSeg_ConfigType *addConfig, uint8_h digit)
{
    uint8_h pinCount;
    uint8_h pinIndex;
    uint8_h value;

    if ((addConfig == NULL) || (digit > 9U))
    {
        return E_NOK;
    }

    if (addConfig->connection == SEVEN_SEG_CONNECTION_DIRECT)
    {
        pinCount = 7U;
        value    = SevenSeg_DigitTable[digit];
    }
    else if (addConfig->connection == SEVEN_SEG_CONNECTION_BCD)
    {
        pinCount = 4U;
        value    = digit;               /* decoder converts binary -> segments */
    }
    else
    {
        return E_NOK;
    }

    if (addConfig->type == SEVEN_SEG_COMMON_ANODE)
    {
        value = (uint8_h)(~value);      /* invert polarity for common anode */
    }

    for (pinIndex = 0U; pinIndex < pinCount; ++pinIndex)
    {
        if (GPIO_SetPinValue(addConfig->dataPort,
                             addConfig->startPin + pinIndex,
                             GET_BIT(value, pinIndex)) != E_OK)
        {
            return E_NOK;
        }
    }

    return E_OK;
}


STD_ReturnType SevenSeg_Clear(const SevenSeg_ConfigType *addConfig)
{
    uint8_h pinCount;
    uint8_h pinIndex;
    uint8_h offLevel;

    if ((addConfig == NULL) || (addConfig->dataPort >= GPIO_NUMBER_OF_PORTS))
    {
        return E_NOK;
    }

    if (addConfig->connection == SEVEN_SEG_CONNECTION_DIRECT)
    {
        pinCount = 7U;
    }
    else if (addConfig->connection == SEVEN_SEG_CONNECTION_BCD)
    {
        pinCount = 4U;
    }
    else
    {
        return E_NOK;
    }

    if ((addConfig->startPin >= GPIO_NUMBER_OF_PINS) ||
        (addConfig->startPin + pinCount > GPIO_NUMBER_OF_PINS))
    {
        return E_NOK;
    }

    offLevel = (addConfig->type == SEVEN_SEG_COMMON_CATHODE) ? 0U : 1U;

    for (pinIndex = 0U; pinIndex < pinCount; ++pinIndex)
    {
        if (GPIO_SetPinValue(addConfig->dataPort,
                             addConfig->startPin + pinIndex,
                             offLevel) != E_OK)
        {
            return E_NOK;
        }
    }

    return E_OK;
}


STD_ReturnType SevenSeg_EnableDigit(uint8_h enablePort, uint8_h enablePin,
                                    SevenSeg_EnableLevel activeLevel)
{
    uint8_h value;

    if ((enablePort >= GPIO_NUMBER_OF_PORTS) || (enablePin >= GPIO_NUMBER_OF_PINS))
    {
        return E_NOK;
    }

    value = (activeLevel == SEVEN_SEG_ENABLE_ACTIVE_HIGH) ? 1U : 0U;

    if (GPIO_SetPinValue(enablePort, enablePin, value) != E_OK)
    {
        return E_NOK;
    }

    return E_OK;
}


STD_ReturnType SevenSeg_DisableDigit(uint8_h enablePort, uint8_h enablePin,
                                     SevenSeg_EnableLevel activeLevel)
{
    uint8_h value;

    if ((enablePort >= GPIO_NUMBER_OF_PORTS) || (enablePin >= GPIO_NUMBER_OF_PINS))
    {
        return E_NOK;
    }

    value = (activeLevel == SEVEN_SEG_ENABLE_ACTIVE_HIGH) ? 0U : 1U;

    if (GPIO_SetPinValue(enablePort, enablePin, value) != E_OK)
    {
        return E_NOK;
    }

    return E_OK;
}
