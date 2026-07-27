#ifndef GPIO_INTERFACE_H
#define GPIO_INTERFACE_H
#include "../../Service/STD_Type.h"

#define GPIO_INPUT      0
#define GPIO_OUTPUT     1 
#define GPIO_LOW        0
#define GPIO_HIGH       1

#define GPIO_NUMBER_OF_PORTS  4

#define GPIO_PORTA      0
#define GPIO_PORTB      1
#define GPIO_PORTC      2
#define GPIO_PORTD      3

#define GPIO_NUMBER_OF_PINS  8

#define GPIO_PIN0        0  
#define GPIO_PIN1        1
#define GPIO_PIN2        2
#define GPIO_PIN3        3
#define GPIO_PIN4        4
#define GPIO_PIN5        5
#define GPIO_PIN6        6
#define GPIO_PIN7        7


STD_ReturnType GPIO_SetPinDirection(uint8_t uint8Port, uint8_t uint8Pin, uint8_t uint8Direction);
STD_ReturnType GPIO_SetPortDirection(uint8_t uint8Port, uint8_t uint8Direction);
STD_ReturnType GPIO_GetPinValue(uint8_t uint8Port, uint8_t uint8Pin, uint8_t *puint8Value);
STD_ReturnType GPIO_GetPortStatus(uint8_t uint8Port, uint8_t *puint8Status);
STD_ReturnType GPIO_PinToggle(uint8_t uint8Port, uint8_t uint8Pin);
STD_ReturnType GPIO_PinSetValue(uint8_t uint8Port, uint8_t uint8Pin, uint8_t uint8Value);
STD_ReturnType GPIO_PortSetValue(uint8_t uint8Port, uint8_t uint8Value);

#endif
