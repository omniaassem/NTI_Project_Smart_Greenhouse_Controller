# 1 "src/main.c"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "src/main.c"
# 1 "src/../Service/Bit_Math.h" 1
# 2 "src/main.c" 2
# 1 "src/../MCL/GPIO/gpio_interface.h" 1



# 1 "src/../MCL/GPIO/../../Service/STD_Types.h" 1



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
# 5 "src/../MCL/GPIO/gpio_interface.h" 2
# 26 "src/../MCL/GPIO/gpio_interface.h"
STD_ReturnType GPIO_SetPinDirection(uint8_h uint8Port, uint8_h uint8Pin, uint8_h uint8Direction);
# 37 "src/../MCL/GPIO/gpio_interface.h"
STD_ReturnType GPIO_SetPortDirection(uint8_h uint8Port, uint8_h uint8Direction);

GPIO_PINStatus GPIO_GetPinStatus(uint8_h uint8Port, uint8_h uint8Pin);

GPIO_PortStatus GPIO_GetPortStatus(uint8_h uint8Port);

STD_ReturnType GPIO_PinToggle(uint8_h uint8Port, uint8_h uint8Pin);
STD_ReturnType GPIO_SetPinValue(uint8_h uint8Port, uint8_h uint8Pin, uint8_h uint8Value);
STD_ReturnType GPIO_SetPortValue(uint8_h uint8Port, uint8_h uint8Pin);
# 3 "src/main.c" 2
# 1 "src/../MCL/GPIO/gpio_registers.h" 1
# 26 "src/../MCL/GPIO/gpio_registers.h"
typedef enum
{
    PIN_LOW = 0,
    PIN_HIGH = 1
} GPIO_PINStatus;

typedef unsigned char GPIO_PortStatus;
# 4 "src/main.c" 2



int main(){
    GPIO_SetPortDirection(0,1);
    (*(volatile u8 *)0x3B) = 0b01010101;
}
# 45 "src/main.c"
static void delay(volatile unsigned long count)
{
    while (count--) {
        __asm__("nop");
    }
}
