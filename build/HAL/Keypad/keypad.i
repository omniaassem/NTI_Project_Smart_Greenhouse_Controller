# 1 "HAL/Keypad/keypad.c"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "HAL/Keypad/keypad.c"
# 1 "c:\\users\\omnia assem\\.platformio\\packages\\toolchain-atmelavr\\lib\\gcc\\avr\\7.3.0\\include\\stddef.h" 1 3 4
# 149 "c:\\users\\omnia assem\\.platformio\\packages\\toolchain-atmelavr\\lib\\gcc\\avr\\7.3.0\\include\\stddef.h" 3 4

# 149 "c:\\users\\omnia assem\\.platformio\\packages\\toolchain-atmelavr\\lib\\gcc\\avr\\7.3.0\\include\\stddef.h" 3 4
typedef int ptrdiff_t;
# 216 "c:\\users\\omnia assem\\.platformio\\packages\\toolchain-atmelavr\\lib\\gcc\\avr\\7.3.0\\include\\stddef.h" 3 4
typedef unsigned int size_t;
# 328 "c:\\users\\omnia assem\\.platformio\\packages\\toolchain-atmelavr\\lib\\gcc\\avr\\7.3.0\\include\\stddef.h" 3 4
typedef int wchar_t;
# 2 "HAL/Keypad/keypad.c" 2
# 1 "HAL/Keypad/../../Service/STD_Type.h" 1




# 4 "HAL/Keypad/../../Service/STD_Type.h"
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
# 3 "HAL/Keypad/keypad.c" 2
# 1 "HAL/Keypad/../../Service/Bit_Math.h" 1
# 4 "HAL/Keypad/keypad.c" 2
# 1 "HAL/Keypad/../../Micro/GPIO/GPIO_Interface.h" 1


# 1 "e:\\nti micro\\final project\\service\\std_type.h" 1
# 4 "HAL/Keypad/../../Micro/GPIO/GPIO_Interface.h" 2
# 29 "HAL/Keypad/../../Micro/GPIO/GPIO_Interface.h"
STD_ReturnType GPIO_SetPinDirection(uint8_t uint8Port, uint8_t uint8Pin, uint8_t uint8Direction);
STD_ReturnType GPIO_SetPortDirection(uint8_t uint8Port, uint8_t uint8Direction);
STD_ReturnType GPIO_GetPinValue(uint8_t uint8Port, uint8_t uint8Pin, uint8_t *puint8Value);
STD_ReturnType GPIO_GetPortStatus(uint8_t uint8Port, uint8_t *puint8Status);
STD_ReturnType GPIO_PinToggle(uint8_t uint8Port, uint8_t uint8Pin);
STD_ReturnType GPIO_PinSetValue(uint8_t uint8Port, uint8_t uint8Pin, uint8_t uint8Value);
STD_ReturnType GPIO_PortSetValue(uint8_t uint8Port, uint8_t uint8Value);
# 5 "HAL/Keypad/keypad.c" 2
# 1 "HAL/Keypad/keypad_interface.h" 1
# 42 "HAL/Keypad/keypad_interface.h"
typedef struct
{
    uint8_t rowPort;
    uint8_t rowStartPin;
    uint8_t colPort;
    uint8_t colStartPin;
    uint8_t keyMap[4][4];
} Keypad_ConfigType;
# 61 "HAL/Keypad/keypad_interface.h"
STD_ReturnType Keypad_Init(const Keypad_ConfigType *addConfig);
# 71 "HAL/Keypad/keypad_interface.h"
STD_ReturnType Keypad_GetKey(const Keypad_ConfigType *addConfig, uint8_t *pKey);
# 80 "HAL/Keypad/keypad_interface.h"
STD_ReturnType Keypad_WaitForKey(const Keypad_ConfigType *addConfig, uint8_t *pKey);
# 6 "HAL/Keypad/keypad.c" 2
# 17 "HAL/Keypad/keypad.c"
static void Keypad_DelayMs(uint16_t ms)
{
    volatile uint16_t inner;
    while (ms-- > 0)
    {
        for (inner = 0; inner < 3000U; ++inner)
        {

        }
    }
}

STD_ReturnType Keypad_Init(const Keypad_ConfigType *addConfig)
{
    if ((addConfig == 
# 31 "HAL/Keypad/keypad.c" 3 4
                     ((void *)0)
# 31 "HAL/Keypad/keypad.c"
                         ) ||
        (addConfig->rowPort >= 4) ||
        (addConfig->colPort >= 4))
    {
        return E_NOK;
    }

    for (uint8_t r = 0; r < 4; ++r)
    {
        GPIO_SetPinDirection(addConfig->rowPort,
                             addConfig->rowStartPin + r,
                             1);
        GPIO_PinSetValue(addConfig->rowPort,
                         addConfig->rowStartPin + r,
                         1);
    }

    for (uint8_t c = 0; c < 4; ++c)
    {
        GPIO_SetPinDirection(addConfig->colPort,
                             addConfig->colStartPin + c,
                             0);
        GPIO_PinSetValue(addConfig->colPort,
                         addConfig->colStartPin + c,
                         1);
    }

    return E_OK;
}

STD_ReturnType Keypad_GetKey(const Keypad_ConfigType *addConfig, uint8_t *pKey)
{
    if ((addConfig == 
# 63 "HAL/Keypad/keypad.c" 3 4
                     ((void *)0)
# 63 "HAL/Keypad/keypad.c"
                         ) || (pKey == 
# 63 "HAL/Keypad/keypad.c" 3 4
                                       ((void *)0)
# 63 "HAL/Keypad/keypad.c"
                                           ) ||
        (addConfig->rowPort >= 4) ||
        (addConfig->colPort >= 4))
    {
        return E_NOK;
    }

    *pKey = 0xFF;

    for (uint8_t r = 0; r < 4; ++r)
    {
        for (uint8_t rr = 0; rr < 4; ++rr)
        {
            GPIO_PinSetValue(addConfig->rowPort,
                             addConfig->rowStartPin + rr,
                             1);
        }

        GPIO_PinSetValue(addConfig->rowPort,
                         addConfig->rowStartPin + r,
                         0);

        for (uint8_t c = 0; c < 4; ++c)
        {
            uint8_t colStatus = 0;
            if (GPIO_GetPinValue(addConfig->colPort,
                                 addConfig->colStartPin + c,
                                 &colStatus) != E_OK)
            {
                return E_NOK;
            }

            if (colStatus == 0)
            {
                Keypad_DelayMs(20);
                if (GPIO_GetPinValue(addConfig->colPort,
                                     addConfig->colStartPin + c,
                                     &colStatus) != E_OK)
                {
                    return E_NOK;
                }

                if (colStatus == 0)
                {
                    *pKey = addConfig->keyMap[r][c];
                    while (1)
                    {
                        uint8_t releaseStatus = 0;
                        if (GPIO_GetPinValue(addConfig->colPort,
                                             addConfig->colStartPin + c,
                                             &releaseStatus) != E_OK)
                        {
                            return E_NOK;
                        }
                        if (releaseStatus != 0)
                        {
                            break;
                        }
                    }
                    return E_OK;
                }
            }
        }
    }

    return E_OK;
}

STD_ReturnType Keypad_WaitForKey(const Keypad_ConfigType *addConfig, uint8_t *pKey)
{
    if ((addConfig == 
# 133 "HAL/Keypad/keypad.c" 3 4
                     ((void *)0)
# 133 "HAL/Keypad/keypad.c"
                         ) || (pKey == 
# 133 "HAL/Keypad/keypad.c" 3 4
                                       ((void *)0)
# 133 "HAL/Keypad/keypad.c"
                                           ))
    {
        return E_NOK;
    }

    do
    {
        if (Keypad_GetKey(addConfig, pKey) == E_NOK)
        {
            return E_NOK;
        }
    }
    while (*pKey == 0xFF);

    return E_OK;
}
