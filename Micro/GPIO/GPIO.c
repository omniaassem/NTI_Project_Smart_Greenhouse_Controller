#include <stddef.h>
#include "../../Service/STD_Type.h"
#include "../../Service/Bit_Math.h"
#include "GPIO_Registers.h"
#include "GPIO_Interface.h"

static volatile uint8_t *GPIO_DDPx[GPIO_NUMBER_OF_PORTS] = {&GPIO_DDRA, &GPIO_DDRB, &GPIO_DDRC, &GPIO_DDRD};
static volatile uint8_t *GPIO_PINx[GPIO_NUMBER_OF_PORTS] = {&GPIO_PINA, &GPIO_PINB, &GPIO_PINC, &GPIO_PIND};
static volatile uint8_t *GPIO_PORTx[GPIO_NUMBER_OF_PORTS] = {&GPIO_PORTA_REG, &GPIO_PORTB_REG, &GPIO_PORTC_REG, &GPIO_PORTD_REG};

STD_ReturnType GPIO_SetPinDirection(uint8_t uint8Port, uint8_t uint8Pin, uint8_t uint8Direction) {
    STD_ReturnType local_Status = E_OK;

    if ((uint8Port >= GPIO_NUMBER_OF_PORTS) || (uint8Pin >= GPIO_NUMBER_OF_PINS)) {
        local_Status = E_NOK;
    }
    else {
        switch (uint8Direction) {
            case GPIO_INPUT:
                CLR_BIT(*GPIO_DDPx[uint8Port], uint8Pin);
                break;

            case GPIO_OUTPUT:
                SET_BIT(*GPIO_DDPx[uint8Port], uint8Pin);
                break; 

            default:
                local_Status = E_NOK;
                break;
        }
    }

    return local_Status;
}

/************************************************************ */
STD_ReturnType GPIO_SetPortDirection(uint8_t uint8Port, uint8_t uint8Direction){
    STD_ReturnType local_Status = E_OK;

    if (uint8Port >= GPIO_NUMBER_OF_PORTS) {
        local_Status = E_NOK;
    }
    else {
        switch (uint8Direction) {
            case GPIO_INPUT:
                *GPIO_DDPx[uint8Port] = 0x00;
                break;

            case GPIO_OUTPUT:
                *GPIO_DDPx[uint8Port] = 0xFF;
                break;

            default:
                *GPIO_DDPx[uint8Port] = uint8Direction;
                break;
        }
    }

    return local_Status;
}

/************************************************************************* */
STD_ReturnType GPIO_GetPinValue(uint8_t uint8Port, uint8_t uint8Pin, uint8_t *puint8Value) {
    STD_ReturnType local_Status = E_OK;

    if ((uint8Port >= GPIO_NUMBER_OF_PORTS) || (uint8Pin >= GPIO_NUMBER_OF_PINS) || (puint8Value == NULL)) {
        local_Status = E_NOK;
    }
    else {
        *puint8Value = GET_BIT(*GPIO_PINx[uint8Port], uint8Pin);
    }

    return local_Status;
}

/************************************************************************** */
STD_ReturnType GPIO_GetPortStatus(uint8_t uint8Port, uint8_t *puint8Status) {
    STD_ReturnType local_Status = E_OK;

    if ((uint8Port >= GPIO_NUMBER_OF_PORTS) || (puint8Status == NULL)) {
        local_Status = E_NOK;
    }
    else {
        *puint8Status = *GPIO_PINx[uint8Port];
    }

    return local_Status;
}

/*************************************************************************** */
STD_ReturnType GPIO_PinToggle(uint8_t uint8Port, uint8_t uint8Pin) {
    STD_ReturnType local_Status = E_OK;

    if ((uint8Port >= GPIO_NUMBER_OF_PORTS) || (uint8Pin >= GPIO_NUMBER_OF_PINS)) {
        local_Status = E_NOK;
    }
    else {
        TOG_BIT(*GPIO_PORTx[uint8Port], uint8Pin);
    }

    return local_Status;
}

/************************************************************************** */
STD_ReturnType GPIO_PinSetValue(uint8_t uint8Port, uint8_t uint8Pin, uint8_t uint8Value) {
    STD_ReturnType local_Status = E_OK;

    if ((uint8Port >= GPIO_NUMBER_OF_PORTS) || (uint8Pin >= GPIO_NUMBER_OF_PINS)) {
        local_Status = E_NOK;
    }
    else {
        switch (uint8Value) {
            case GPIO_LOW:
                CLR_BIT(*GPIO_PORTx[uint8Port], uint8Pin);
                break;

            case GPIO_HIGH:
                SET_BIT(*GPIO_PORTx[uint8Port], uint8Pin);
                break;

            default:
                local_Status = E_NOK;
                break;
        }
    }

    return local_Status;
}

/*********************************************************************** */
STD_ReturnType GPIO_PortSetValue(uint8_t uint8Port, uint8_t uint8Value) {
    STD_ReturnType local_Status = E_OK;

    if (uint8Port >= GPIO_NUMBER_OF_PORTS) {
        local_Status = E_NOK;
    }
    else {
        *GPIO_PORTx[uint8Port] = uint8Value;
    }

    return local_Status;
}