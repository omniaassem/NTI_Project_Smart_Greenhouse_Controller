# 1 "main.c"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "main.c"
# 1 "Micro/GPIO/GPIO_Interface.h" 1


# 1 "Micro/GPIO/../../Service/STD_Type.h" 1



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
# 4 "Micro/GPIO/GPIO_Interface.h" 2
# 29 "Micro/GPIO/GPIO_Interface.h"
STD_ReturnType GPIO_SetPinDirection(uint8_t uint8Port, uint8_t uint8Pin, uint8_t uint8Direction);
STD_ReturnType GPIO_SetPortDirection(uint8_t uint8Port, uint8_t uint8Direction);
STD_ReturnType GPIO_GetPinValue(uint8_t uint8Port, uint8_t uint8Pin, uint8_t *puint8Value);
STD_ReturnType GPIO_GetPortStatus(uint8_t uint8Port, uint8_t *puint8Status);
STD_ReturnType GPIO_PinToggle(uint8_t uint8Port, uint8_t uint8Pin);
STD_ReturnType GPIO_PinSetValue(uint8_t uint8Port, uint8_t uint8Pin, uint8_t uint8Value);
STD_ReturnType GPIO_PortSetValue(uint8_t uint8Port, uint8_t uint8Value);
# 2 "main.c" 2

int main(void)
{
    GPIO_SetPinDirection(0, 1, 1);
    GPIO_PinSetValue(0, 1, 1);

    while (1)
    {
    }

    return 0;
}
