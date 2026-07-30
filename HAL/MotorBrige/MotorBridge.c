/**************************************************************************/
/* File Name: MotorBridge.c                                               */
/* Description: Motor H-Bridge Driver Implementation                      */
/**************************************************************************/

#include "STD_Types.h"
#include "Bit_Math.h"
#include "gpio_interface.h"
#include "MotorBridge.h"

/**************************************************************************/
/* Function Definitions                                                   */
/**************************************************************************/

STD_ReturnType BRIDGE_Init(void)
{
    /* Set control pins as output and initialize to LOW (Stop) */
    (void)GPIO_SetPinDirection(GPIO_PORTB, GPIO_PIN0, GPIO_OUTPUT);
    (void)GPIO_SetPinDirection(GPIO_PORTB, GPIO_PIN1, GPIO_OUTPUT);
    
    (void)GPIO_SetPinValue(GPIO_PORTB, GPIO_PIN0, PIN_LOW);
    (void)GPIO_SetPinValue(GPIO_PORTB, GPIO_PIN1, PIN_LOW);

    return E_OK;
}

void BRIDGE_Forward(void)
{
    (void)GPIO_SetPinValue(GPIO_PORTB, GPIO_PIN0, PIN_HIGH);
    (void)GPIO_SetPinValue(GPIO_PORTB, GPIO_PIN1, PIN_LOW);
}

void BRIDGE_Backward(void)
{
    (void)GPIO_SetPinValue(GPIO_PORTB, GPIO_PIN0, PIN_LOW);
    (void)GPIO_SetPinValue(GPIO_PORTB, GPIO_PIN1, PIN_HIGH);
}

void BRIDGE_ForceStop(void)
{
    (void)GPIO_SetPinValue(GPIO_PORTB, GPIO_PIN0, PIN_LOW);
    (void)GPIO_SetPinValue(GPIO_PORTB, GPIO_PIN1, PIN_LOW);
}