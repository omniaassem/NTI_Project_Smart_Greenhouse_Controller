# 1 "HAL/SevenSegment/seven_segment.c"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "HAL/SevenSegment/seven_segment.c"
# 1 "c:\\users\\omnia assem\\.platformio\\packages\\toolchain-atmelavr\\lib\\gcc\\avr\\7.3.0\\include\\stddef.h" 1 3 4
# 149 "c:\\users\\omnia assem\\.platformio\\packages\\toolchain-atmelavr\\lib\\gcc\\avr\\7.3.0\\include\\stddef.h" 3 4

# 149 "c:\\users\\omnia assem\\.platformio\\packages\\toolchain-atmelavr\\lib\\gcc\\avr\\7.3.0\\include\\stddef.h" 3 4
typedef int ptrdiff_t;
# 216 "c:\\users\\omnia assem\\.platformio\\packages\\toolchain-atmelavr\\lib\\gcc\\avr\\7.3.0\\include\\stddef.h" 3 4
typedef unsigned int size_t;
# 328 "c:\\users\\omnia assem\\.platformio\\packages\\toolchain-atmelavr\\lib\\gcc\\avr\\7.3.0\\include\\stddef.h" 3 4
typedef int wchar_t;
# 2 "HAL/SevenSegment/seven_segment.c" 2
# 1 "HAL/SevenSegment/../../Service/STD_Type.h" 1




# 4 "HAL/SevenSegment/../../Service/STD_Type.h"
typedef unsigned char uint8_t;
typedef signed char sint8_t;
typedef unsigned short int uint16_t;
typedef signed short int sint16_t;
typedef unsigned long int uint32_t;
typedef signed long int sint32_t;
typedef float float32_t;
typedef double float64_t;


typedef enum{

    E_OK =0 ,
    E_NOK=1

}STD_ReturnType;
# 3 "HAL/SevenSegment/seven_segment.c" 2
# 1 "HAL/SevenSegment/../../Service/Bit_Math.h" 1
# 4 "HAL/SevenSegment/seven_segment.c" 2
# 1 "HAL/SevenSegment/../../Micro/GPIO/GPIO_Interface.h" 1


# 1 "e:\\nti micro\\final project\\service\\std_type.h" 1
# 4 "HAL/SevenSegment/../../Micro/GPIO/GPIO_Interface.h" 2
# 29 "HAL/SevenSegment/../../Micro/GPIO/GPIO_Interface.h"
STD_ReturnType GPIO_SetPinDirection(uint8_t uint8Port, uint8_t uint8Pin, uint8_t uint8Direction);
STD_ReturnType GPIO_SetPortDirection(uint8_t uint8Port, uint8_t uint8Direction);
STD_ReturnType GPIO_GetPinValue(uint8_t uint8Port, uint8_t uint8Pin, uint8_t *puint8Value);
STD_ReturnType GPIO_GetPortStatus(uint8_t uint8Port, uint8_t *puint8Status);
STD_ReturnType GPIO_PinToggle(uint8_t uint8Port, uint8_t uint8Pin);
STD_ReturnType GPIO_PinSetValue(uint8_t uint8Port, uint8_t uint8Pin, uint8_t uint8Value);
STD_ReturnType GPIO_PortSetValue(uint8_t uint8Port, uint8_t uint8Value);
# 5 "HAL/SevenSegment/seven_segment.c" 2
# 1 "HAL/SevenSegment/seven_segment_interface.h" 1
# 29 "HAL/SevenSegment/seven_segment_interface.h"
typedef enum
{
    SEVEN_SEG_COMMON_CATHODE = 0,
    SEVEN_SEG_COMMON_ANODE = 1
} SevenSeg_Type;







typedef enum
{
    SEVEN_SEG_CONNECTION_DIRECT = 0,
    SEVEN_SEG_CONNECTION_BCD = 1
} SevenSeg_ConnectionType;






typedef enum
{
    SEVEN_SEG_ENABLE_ACTIVE_LOW = 0,
    SEVEN_SEG_ENABLE_ACTIVE_HIGH = 1
} SevenSeg_EnableLevel;
# 69 "HAL/SevenSegment/seven_segment_interface.h"
typedef struct
{
    SevenSeg_Type type;
    SevenSeg_ConnectionType connection;
    uint8_t dataPort;
    uint8_t startPin;
} SevenSeg_ConfigType;
# 86 "HAL/SevenSegment/seven_segment_interface.h"
STD_ReturnType SevenSeg_Init(const SevenSeg_ConfigType *addConfig);
# 97 "HAL/SevenSegment/seven_segment_interface.h"
STD_ReturnType SevenSeg_DisplayDigit(const SevenSeg_ConfigType *addConfig, uint8_t digit);






STD_ReturnType SevenSeg_Clear(const SevenSeg_ConfigType *addConfig);
# 113 "HAL/SevenSegment/seven_segment_interface.h"
STD_ReturnType SevenSeg_EnableDigit(uint8_t enablePort, uint8_t enablePin,
                                    SevenSeg_EnableLevel activeLevel);
# 123 "HAL/SevenSegment/seven_segment_interface.h"
STD_ReturnType SevenSeg_DisableDigit(uint8_t enablePort, uint8_t enablePin,
                                     SevenSeg_EnableLevel activeLevel);
# 6 "HAL/SevenSegment/seven_segment.c" 2
# 23 "HAL/SevenSegment/seven_segment.c"
static const uint8_t SevenSeg_DigitTable[10] = {
    0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F
};

STD_ReturnType SevenSeg_Init(const SevenSeg_ConfigType *addConfig)
{
    if ((addConfig == 
# 29 "HAL/SevenSegment/seven_segment.c" 3 4
                     ((void *)0)
# 29 "HAL/SevenSegment/seven_segment.c"
                         ) || (addConfig->dataPort >= 4))
    {
        return E_NOK;
    }

    if (addConfig->connection == SEVEN_SEG_CONNECTION_DIRECT)
    {
        for (uint8_t i = 0; i < 7; ++i)
        {
            GPIO_SetPinDirection(addConfig->dataPort, addConfig->startPin + i, 1);
        }
    }
    else
    {
        for (uint8_t i = 0; i < 4; ++i)
        {
            GPIO_SetPinDirection(addConfig->dataPort, addConfig->startPin + i, 1);
        }
    }

    SevenSeg_Clear(addConfig);
    return E_OK;
}

STD_ReturnType SevenSeg_DisplayDigit(const SevenSeg_ConfigType *addConfig, uint8_t digit)
{
    if ((addConfig == 
# 55 "HAL/SevenSegment/seven_segment.c" 3 4
                     ((void *)0)
# 55 "HAL/SevenSegment/seven_segment.c"
                         ) || (digit > 9) || (addConfig->dataPort >= 4))
    {
        return E_NOK;
    }

    uint8_t value;
    if (addConfig->connection == SEVEN_SEG_CONNECTION_DIRECT)
    {
        value = SevenSeg_DigitTable[digit];
    }
    else
    {
        value = digit;
    }

    if (addConfig->type == SEVEN_SEG_COMMON_ANODE)
    {
        value = ~value;
    }

    if (addConfig->connection == SEVEN_SEG_CONNECTION_DIRECT)
    {
        for (uint8_t i = 0; i < 7; ++i)
        {
            GPIO_PinSetValue(addConfig->dataPort,
                             addConfig->startPin + i,
                             (((value) >> (i)) & 1));
        }
    }
    else
    {
        for (uint8_t i = 0; i < 4; ++i)
        {
            GPIO_PinSetValue(addConfig->dataPort,
                             addConfig->startPin + i,
                             (((value) >> (i)) & 1));
        }
    }

    return E_OK;
}

STD_ReturnType SevenSeg_Clear(const SevenSeg_ConfigType *addConfig)
{
    if ((addConfig == 
# 99 "HAL/SevenSegment/seven_segment.c" 3 4
                     ((void *)0)
# 99 "HAL/SevenSegment/seven_segment.c"
                         ) || (addConfig->dataPort >= 4))
    {
        return E_NOK;
    }

    uint8_t offLevel = (addConfig->type == SEVEN_SEG_COMMON_CATHODE) ? 0 : 1;
    uint8_t pinCount = (addConfig->connection == SEVEN_SEG_CONNECTION_DIRECT) ? 7 : 4;

    for (uint8_t i = 0; i < pinCount; ++i)
    {
        GPIO_PinSetValue(addConfig->dataPort, addConfig->startPin + i, offLevel);
    }

    return E_OK;
}

STD_ReturnType SevenSeg_EnableDigit(uint8_t enablePort, uint8_t enablePin,
                                    SevenSeg_EnableLevel activeLevel)
{
    if ((enablePort >= 4) || (enablePin >= 8))
    {
        return E_NOK;
    }

    uint8_t value = (activeLevel == SEVEN_SEG_ENABLE_ACTIVE_HIGH) ? 1 : 0;
    GPIO_PinSetValue(enablePort, enablePin, value);
    return E_OK;
}

STD_ReturnType SevenSeg_DisableDigit(uint8_t enablePort, uint8_t enablePin,
                                     SevenSeg_EnableLevel activeLevel)
{
    if ((enablePort >= 4) || (enablePin >= 8))
    {
        return E_NOK;
    }

    uint8_t value = (activeLevel == SEVEN_SEG_ENABLE_ACTIVE_HIGH) ? 0 : 1;
    GPIO_PinSetValue(enablePort, enablePin, value);
    return E_OK;
}
