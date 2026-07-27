#ifndef GPIO_INTERFACE_H
#define GPIO_INTERFACE_H

#include "../../Service/STD_Types.h"
#include "gpio_registers.h"

#define GPIO_INPUT               0
#define GPIO_OUTPUT              1

/* ---------------- Ports ---------------- */
#define GPIO_PORTA   0
#define GPIO_PORTB   1
#define GPIO_PORTC   2
#define GPIO_PORTD   3

/* ---------------- Pins ---------------- */
#define GPIO_PIN0    0
#define GPIO_PIN1    1
#define GPIO_PIN2    2
#define GPIO_PIN3    3
#define GPIO_PIN4    4
#define GPIO_PIN5    5
#define GPIO_PIN6    6
#define GPIO_PIN7    7


STD_ReturnType GPIO_SetPinDirection(uint8_h uint8Port, uint8_h uint8Pin, uint8_h uint8Direction);
/**
 * @brief dfsjkgnbhjsikldf
 * 
 * @param uint8Port 
 * @param uint8Direction    INPUT/OUTPUT
 * @return STD_ReturnType   E_OK/E_NOK
 */

 // sdfggdfgg

STD_ReturnType  GPIO_SetPortDirection(uint8_h uint8Port, uint8_h uint8Direction);

GPIO_PINStatus  GPIO_GetPinStatus(uint8_h uint8Port, uint8_h uint8Pin);

GPIO_PortStatus GPIO_GetPortStatus(uint8_h uint8Port);                   

STD_ReturnType  GPIO_PinToggle(uint8_h uint8Port, uint8_h uint8Pin);
STD_ReturnType  GPIO_SetPinValue(uint8_h uint8Port, uint8_h uint8Pin, uint8_h uint8Value);
STD_ReturnType  GPIO_SetPortValue(uint8_h uint8Port, uint8_h uint8Value);

#endif // GPIO_INTERFACE_H


