# 1 "Micro/GPIO/GPIO.c"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "Micro/GPIO/GPIO.c"
# 1 "c:\\users\\omnia assem\\.platformio\\packages\\toolchain-atmelavr\\lib\\gcc\\avr\\7.3.0\\include\\stddef.h" 1 3 4
# 149 "c:\\users\\omnia assem\\.platformio\\packages\\toolchain-atmelavr\\lib\\gcc\\avr\\7.3.0\\include\\stddef.h" 3 4

# 149 "c:\\users\\omnia assem\\.platformio\\packages\\toolchain-atmelavr\\lib\\gcc\\avr\\7.3.0\\include\\stddef.h" 3 4
typedef int ptrdiff_t;
# 216 "c:\\users\\omnia assem\\.platformio\\packages\\toolchain-atmelavr\\lib\\gcc\\avr\\7.3.0\\include\\stddef.h" 3 4
typedef unsigned int size_t;
# 328 "c:\\users\\omnia assem\\.platformio\\packages\\toolchain-atmelavr\\lib\\gcc\\avr\\7.3.0\\include\\stddef.h" 3 4
typedef int wchar_t;
# 2 "Micro/GPIO/GPIO.c" 2
# 1 "Micro/GPIO/../../Service/STD_Type.h" 1




# 4 "Micro/GPIO/../../Service/STD_Type.h"
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
# 3 "Micro/GPIO/GPIO.c" 2
# 1 "Micro/GPIO/../../Service/Bit_Math.h" 1
# 4 "Micro/GPIO/GPIO.c" 2
# 1 "Micro/GPIO/GPIO_Registers.h" 1
# 5 "Micro/GPIO/GPIO.c" 2
# 1 "Micro/GPIO/GPIO_Interface.h" 1
# 29 "Micro/GPIO/GPIO_Interface.h"
STD_ReturnType GPIO_SetPinDirection(uint8_t uint8Port, uint8_t uint8Pin, uint8_t uint8Direction);
STD_ReturnType GPIO_SetPortDirection(uint8_t uint8Port, uint8_t uint8Direction);
STD_ReturnType GPIO_GetPinValue(uint8_t uint8Port, uint8_t uint8Pin, uint8_t *puint8Value);
STD_ReturnType GPIO_GetPortStatus(uint8_t uint8Port, uint8_t *puint8Status);
STD_ReturnType GPIO_PinToggle(uint8_t uint8Port, uint8_t uint8Pin);
STD_ReturnType GPIO_PinSetValue(uint8_t uint8Port, uint8_t uint8Pin, uint8_t uint8Value);
STD_ReturnType GPIO_PortSetValue(uint8_t uint8Port, uint8_t uint8Value);
# 6 "Micro/GPIO/GPIO.c" 2

static volatile uint8_t *GPIO_DDPx[4] = {&(*(volatile uint8_t *) 0x3A), &(*(volatile uint8_t *) 0x37), &(*(volatile uint8_t *) 0x34), &(*(volatile uint8_t *) 0x31)};
static volatile uint8_t *GPIO_PINx[4] = {&(*(volatile uint8_t *) 0x39), &(*(volatile uint8_t *) 0x36), &(*(volatile uint8_t *) 0x33), &(*(volatile uint8_t *) 0x30)};
static volatile uint8_t *GPIO_PORTx[4] = {&(*(volatile uint8_t *) 0x3B), &(*(volatile uint8_t *) 0x38), &(*(volatile uint8_t *) 0x35), &(*(volatile uint8_t *) 0x32)};

STD_ReturnType GPIO_SetPinDirection(uint8_t uint8Port, uint8_t uint8Pin, uint8_t uint8Direction) {
    STD_ReturnType local_Status = E_OK;

    if ((uint8Port >= 4) || (uint8Pin >= 8)) {
        local_Status = E_NOK;
    }
    else {
        switch (uint8Direction) {
            case 0:
                ((*GPIO_DDPx[uint8Port]) &= ~(1 << (uint8Pin)));
                break;

            case 1:
                ((*GPIO_DDPx[uint8Port]) |= (1 << (uint8Pin)));
                break;

            default:
                local_Status = E_NOK;
                break;
        }
    }

    return local_Status;
}


STD_ReturnType GPIO_SetPortDirection(uint8_t uint8Port, uint8_t uint8Direction){
    STD_ReturnType local_Status = E_OK;

    if (uint8Port >= 4) {
        local_Status = E_NOK;
    }
    else {
        switch (uint8Direction) {
            case 0:
                *GPIO_DDPx[uint8Port] = 0x00;
                break;

            case 1:
                *GPIO_DDPx[uint8Port] = 0xFF;
                break;

            default:
                *GPIO_DDPx[uint8Port] = uint8Direction;
                break;
        }
    }

    return local_Status;
}


STD_ReturnType GPIO_GetPinValue(uint8_t uint8Port, uint8_t uint8Pin, uint8_t *puint8Value) {
    STD_ReturnType local_Status = E_OK;

    if ((uint8Port >= 4) || (uint8Pin >= 8) || (puint8Value == 
# 66 "Micro/GPIO/GPIO.c" 3 4
                                                                                                   ((void *)0)
# 66 "Micro/GPIO/GPIO.c"
                                                                                                       )) {
        local_Status = E_NOK;
    }
    else {
        *puint8Value = (((*GPIO_PINx[uint8Port]) >> (uint8Pin)) & 1);
    }

    return local_Status;
}


STD_ReturnType GPIO_GetPortStatus(uint8_t uint8Port, uint8_t *puint8Status) {
    STD_ReturnType local_Status = E_OK;

    if ((uint8Port >= 4) || (puint8Status == 
# 80 "Micro/GPIO/GPIO.c" 3 4
                                                               ((void *)0)
# 80 "Micro/GPIO/GPIO.c"
                                                                   )) {
        local_Status = E_NOK;
    }
    else {
        *puint8Status = *GPIO_PINx[uint8Port];
    }

    return local_Status;
}


STD_ReturnType GPIO_PinToggle(uint8_t uint8Port, uint8_t uint8Pin) {
    STD_ReturnType local_Status = E_OK;

    if ((uint8Port >= 4) || (uint8Pin >= 8)) {
        local_Status = E_NOK;
    }
    else {
        ((*GPIO_PORTx[uint8Port]) ^= (1 << (uint8Pin)));
    }

    return local_Status;
}


STD_ReturnType GPIO_PinSetValue(uint8_t uint8Port, uint8_t uint8Pin, uint8_t uint8Value) {
    STD_ReturnType local_Status = E_OK;

    if ((uint8Port >= 4) || (uint8Pin >= 8)) {
        local_Status = E_NOK;
    }
    else {
        switch (uint8Value) {
            case 0:
                ((*GPIO_PORTx[uint8Port]) &= ~(1 << (uint8Pin)));
                break;

            case 1:
                ((*GPIO_PORTx[uint8Port]) |= (1 << (uint8Pin)));
                break;

            default:
                local_Status = E_NOK;
                break;
        }
    }

    return local_Status;
}


STD_ReturnType GPIO_PortSetValue(uint8_t uint8Port, uint8_t uint8Value) {
    STD_ReturnType local_Status = E_OK;

    if (uint8Port >= 4) {
        local_Status = E_NOK;
    }
    else {
        *GPIO_PORTx[uint8Port] = uint8Value;
    }

    return local_Status;
}
