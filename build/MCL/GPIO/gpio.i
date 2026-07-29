# 1 "MCL/GPIO/gpio.c"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "MCL/GPIO/gpio.c"
# 1 "MCL/GPIO/../../Service/STD_Types.h" 1



typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned long uint32;
typedef unsigned long long uint64;

typedef signed char sint8;
typedef signed short sint16;
typedef signed long sint32;
typedef signed long long sint64;

typedef float float32;
typedef double float64;

typedef unsigned char uint8_h;
typedef unsigned short uint16_h;
typedef unsigned long uint32_h;
typedef unsigned long long uint64_h;

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;
typedef unsigned long long u64;



typedef enum
{
    E_OK = 0,
    E_NOK = 1
} STD_ReturnType;
# 2 "MCL/GPIO/gpio.c" 2
# 1 "MCL/GPIO/../../Service/Bit_Math.h" 1
# 3 "MCL/GPIO/gpio.c" 2
# 1 "MCL/GPIO/gpio_registers.h" 1
# 26 "MCL/GPIO/gpio_registers.h"
typedef enum
{
    PIN_LOW = 0,
    PIN_HIGH = 1
} GPIO_PINStatus;

typedef unsigned char GPIO_PortStatus;
# 4 "MCL/GPIO/gpio.c" 2
# 1 "MCL/GPIO/gpio_interface.h" 1
# 27 "MCL/GPIO/gpio_interface.h"
STD_ReturnType GPIO_SetPinDirection(uint8_h uint8Port, uint8_h uint8Pin, uint8_h uint8Direction);
# 38 "MCL/GPIO/gpio_interface.h"
STD_ReturnType GPIO_SetPortDirection(uint8_h uint8Port, uint8_h uint8Direction);

GPIO_PINStatus GPIO_GetPinStatus(uint8_h uint8Port, uint8_h uint8Pin);

GPIO_PortStatus GPIO_GetPortStatus(uint8_h uint8Port);

STD_ReturnType GPIO_PinToggle(uint8_h uint8Port, uint8_h uint8Pin);
STD_ReturnType GPIO_SetPinValue(uint8_h uint8Port, uint8_h uint8Pin, uint8_h uint8Value);
STD_ReturnType GPIO_SetPortValue(uint8_h uint8Port, uint8_h uint8Value);
# 5 "MCL/GPIO/gpio.c" 2

static volatile uint8_h *GPIO_DDRx[4] = {&(*(volatile u8 *)0x3A), &(*(volatile u8 *)0x37), &(*(volatile u8 *)0x34), &(*(volatile u8 *)0x31)};





STD_ReturnType GPIO_SetPinDirection(uint8_h uint8Port, uint8_h uint8Pin, uint8_h uint8Direction)
{
    STD_ReturnType local_Status = E_OK;

    if ((uint8Port >= 4) || (uint8Pin >= 8))
    {
        local_Status = E_NOK;
    }
    else
    {
        switch (uint8Direction)
        {
            case 0:
                ((*GPIO_DDRx[uint8Port]) &= ~(1U << (uint8Pin)));
                break;
            case 1:
                ((*GPIO_DDRx[uint8Port]) |= (1U << (uint8Pin)));
                break;
            default:
                local_Status = E_NOK;
                break;
        }
    }

    return local_Status;
}







STD_ReturnType GPIO_SetPortDirection(uint8_h uint8Port, uint8_h uint8Direction){
    STD_ReturnType local_Status = E_OK;

    if ((uint8Port >= 4))
    {
        local_Status = E_NOK;
    }
    else {
        switch (uint8Direction)
        {
            case 0:
                *GPIO_DDRx[uint8Port] = 0x00;
                break;
            case 1:
                *GPIO_DDRx[uint8Port] = 0xFF;
                break;
            default:
                local_Status = E_NOK;
                break;
        }
    }
    return local_Status;
}







GPIO_PINStatus GPIO_GetPinStatus(uint8_h uint8Port, uint8_h uint8Pin)
{
    if ((uint8Port >= 4) || (uint8Pin >= 8))
    {
        return PIN_LOW;
    }

    volatile uint8_h *pinReg = ((void *)0);
    switch (uint8Port)
    {
        case 0: pinReg = &(*(volatile u8 *)0x39); break;
        case 1: pinReg = &(*(volatile u8 *)0x36); break;
        case 2: pinReg = &(*(volatile u8 *)0x33); break;
        case 3: pinReg = &(*(volatile u8 *)0x30); break;
        default: return PIN_LOW;
    }

    return ((((*pinReg) >> (uint8Pin)) & 1U) ? PIN_HIGH : PIN_LOW);
}





GPIO_PortStatus GPIO_GetPortStatus(uint8_h uint8Port)
{
    if (uint8Port >= 4)
    {
        return (GPIO_PortStatus)0;
    }

    switch (uint8Port)
    {
        case 0: return (GPIO_PortStatus)(*(volatile u8 *)0x39);
        case 1: return (GPIO_PortStatus)(*(volatile u8 *)0x36);
        case 2: return (GPIO_PortStatus)(*(volatile u8 *)0x33);
        case 3: return (GPIO_PortStatus)(*(volatile u8 *)0x30);
        default: return (GPIO_PortStatus)0;
    }
}




STD_ReturnType GPIO_PinToggle(uint8_h uint8Port, uint8_h uint8Pin)
{
    STD_ReturnType local_Status = E_OK;

    if ((uint8Port >= 4) || (uint8Pin >= 8))
    {
        local_Status = E_NOK;
    }
    else
    {
        volatile uint8_h *portReg = ((void *)0);
        switch (uint8Port)
        {
            case 0: portReg = &(*(volatile u8 *)0x3B); break;
            case 1: portReg = &(*(volatile u8 *)0x38); break;
            case 2: portReg = &(*(volatile u8 *)0x35); break;
            case 3: portReg = &(*(volatile u8 *)0x32); break;
            default: local_Status = E_NOK; break;
        }

        if (local_Status == E_OK)
        {
            ((*portReg) ^= (1U << (uint8Pin)));
        }
    }

    return local_Status;
}





STD_ReturnType GPIO_SetPinValue(uint8_h uint8Port, uint8_h uint8Pin, uint8_h uint8Value)
{
    STD_ReturnType local_Status = E_OK;

    if ((uint8Port >= 4) || (uint8Pin >= 8))
    {
        local_Status = E_NOK;
    }
    else
    {
        volatile uint8_h *portReg = ((void *)0);
        switch (uint8Port)
        {
            case 0: portReg = &(*(volatile u8 *)0x3B); break;
            case 1: portReg = &(*(volatile u8 *)0x38); break;
            case 2: portReg = &(*(volatile u8 *)0x35); break;
            case 3: portReg = &(*(volatile u8 *)0x32); break;
            default: local_Status = E_NOK; break;
        }

        if (local_Status == E_OK)
        {
            if (uint8Value == 0)
            {
                ((*portReg) &= ~(1U << (uint8Pin)));
            }
            else
            {
                ((*portReg) |= (1U << (uint8Pin)));
            }
        }
    }

    return local_Status;
}




STD_ReturnType GPIO_SetPortValue(uint8_h uint8Port, uint8_h uint8Value)
{
    STD_ReturnType local_Status = E_OK;

    if (uint8Port >= 4)
    {
        local_Status = E_NOK;
    }
    else
    {
        volatile uint8_h *portReg = ((void *)0);
        switch (uint8Port)
        {
            case 0: portReg = &(*(volatile u8 *)0x3B); break;
            case 1: portReg = &(*(volatile u8 *)0x38); break;
            case 2: portReg = &(*(volatile u8 *)0x35); break;
            case 3: portReg = &(*(volatile u8 *)0x32); break;
            default: local_Status = E_NOK; break;
        }

        if (local_Status == E_OK)
        {
            *portReg = (uint8_h)uint8Value;
        }
    }

    return local_Status;
}
