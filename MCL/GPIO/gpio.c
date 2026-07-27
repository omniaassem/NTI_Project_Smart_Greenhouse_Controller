#include "../../Service/STD_Types.h"
#include "../../Service/Bit_Math.h"
#include "gpio_registers.h"
#include "gpio_interface.h"

static volatile uint8_h *GPIO_DDRx[GPIO_NUMBER_OF_PORTS] = {&GPIO_DDRA, &GPIO_DDRB, &GPIO_DDRC, &GPIO_DDRD};

/*
 * Sets the direction of a specific GPIO pin on the requested port.
 * uint8Direction can be GPIO_INPUT or GPIO_OUTPUT.
 */
STD_ReturnType GPIO_SetPinDirection(uint8_h uint8Port, uint8_h uint8Pin, uint8_h uint8Direction)
{
    STD_ReturnType local_Status = E_OK;

    if ((uint8Port >= GPIO_NUMBER_OF_PORTS) || (uint8Pin >= GPIO_NUMBER_OF_PINS))
    {
        local_Status = E_NOK;
    }
    else
    {
        switch (uint8Direction)
        {
            case GPIO_INPUT:
                CLR_BIT(*GPIO_DDRx[uint8Port], uint8Pin);
                break;
            case GPIO_OUTPUT:
                SET_BIT(*GPIO_DDRx[uint8Port], uint8Pin);
                break;
            default:
                local_Status = E_NOK;
                break;
        }
    }

    return local_Status;
}


/*************************************************************************** */
/*
 * Sets the direction for all pins of the requested GPIO port.
 * GPIO_INPUT configures the port as input, GPIO_OUTPUT configures it as output.
 */
STD_ReturnType GPIO_SetPortDirection(uint8_h uint8Port, uint8_h uint8Direction){
    STD_ReturnType local_Status = E_OK;

    if ((uint8Port >= GPIO_NUMBER_OF_PORTS))
    {
        local_Status = E_NOK;
    }
    else {
        switch (uint8Direction)
        {
            case GPIO_INPUT:
                *GPIO_DDRx[uint8Port] = 0x00; 
                break;
            case GPIO_OUTPUT:
                *GPIO_DDRx[uint8Port] = 0xFF;
                break;
            default:
                local_Status = E_NOK;
                break;
        }
    }
    return local_Status;
}

/*************************************************************************************************************** */

/*
 * Read the logic level of a specific GPIO pin and return it as GPIO_PINStatus.
 * Returns PIN_LOW when parameters are invalid or when the pin reads low.
 */
GPIO_PINStatus GPIO_GetPinStatus(uint8_h uint8Port, uint8_h uint8Pin)
{
    if ((uint8Port >= GPIO_NUMBER_OF_PORTS) || (uint8Pin >= GPIO_NUMBER_OF_PINS))
    {
        return PIN_LOW;
    }

    volatile uint8_h *pinReg = NULL;
    switch (uint8Port)
    {
        case GPIO_PORTA: pinReg = &GPIO_PINA; break;
        case GPIO_PORTB: pinReg = &GPIO_PINB; break;
        case GPIO_PORTC: pinReg = &GPIO_PINC; break;
        case GPIO_PORTD: pinReg = &GPIO_PIND; break;
        default: return PIN_LOW;
    }

    return (GET_BIT(*pinReg, uint8Pin) ? PIN_HIGH : PIN_LOW);
}

/*
 * Read and return the input register value for the requested GPIO port.
 * Returns 0 when the port index is invalid.
 */
GPIO_PortStatus GPIO_GetPortStatus(uint8_h uint8Port)
{
    if (uint8Port >= GPIO_NUMBER_OF_PORTS)
    {
        return (GPIO_PortStatus)0;
    }

    switch (uint8Port)
    {
        case GPIO_PORTA: return (GPIO_PortStatus)GPIO_PINA;
        case GPIO_PORTB: return (GPIO_PortStatus)GPIO_PINB;
        case GPIO_PORTC: return (GPIO_PortStatus)GPIO_PINC;
        case GPIO_PORTD: return (GPIO_PortStatus)GPIO_PIND;
        default: return (GPIO_PortStatus)0;
    }
}

/*
 * Toggles the output state of a specific GPIO pin on the requested port.
 */
STD_ReturnType GPIO_PinToggle(uint8_h uint8Port, uint8_h uint8Pin)
{
    STD_ReturnType local_Status = E_OK;

    if ((uint8Port >= GPIO_NUMBER_OF_PORTS) || (uint8Pin >= GPIO_NUMBER_OF_PINS))
    {
        local_Status = E_NOK;
    }
    else
    {
        volatile uint8_h *portReg = NULL;
        switch (uint8Port)
        {
            case GPIO_PORTA: portReg = &GPIO_PORTA_REG; break;
            case GPIO_PORTB: portReg = &GPIO_PORTB_REG; break;
            case GPIO_PORTC: portReg = &GPIO_PORTC_REG; break;
            case GPIO_PORTD: portReg = &GPIO_PORTD_REG; break;
            default: local_Status = E_NOK; break;
        }

        if (local_Status == E_OK)
        {
            TOG_BIT(*portReg, uint8Pin);
        }
    }

    return local_Status;
}

/*
 * Sets the output value for a specific GPIO pin on the requested port.
 * uint8Value of 0 clears the pin; any other value sets the pin.
 */
STD_ReturnType GPIO_SetPinValue(uint8_h uint8Port, uint8_h uint8Pin, uint8_h uint8Value)
{
    STD_ReturnType local_Status = E_OK;

    if ((uint8Port >= GPIO_NUMBER_OF_PORTS) || (uint8Pin >= GPIO_NUMBER_OF_PINS))
    {
        local_Status = E_NOK;
    }
    else
    {
        volatile uint8_h *portReg = NULL;
        switch (uint8Port)
        {
            case GPIO_PORTA: portReg = &GPIO_PORTA_REG; break;
            case GPIO_PORTB: portReg = &GPIO_PORTB_REG; break;
            case GPIO_PORTC: portReg = &GPIO_PORTC_REG; break;
            case GPIO_PORTD: portReg = &GPIO_PORTD_REG; break;
            default: local_Status = E_NOK; break;
        }

        if (local_Status == E_OK)
        {
            if (uint8Value == 0)
            {
                CLR_BIT(*portReg, uint8Pin);
            }
            else
            {
                SET_BIT(*portReg, uint8Pin);
            }
        }
    }

    return local_Status;
}

/*
 * Writes an 8-bit value to the entire output register of the requested GPIO port.
 */
STD_ReturnType GPIO_SetPortValue(uint8_h uint8Port, uint8_h uint8Value)
{
    STD_ReturnType local_Status = E_OK;

    if (uint8Port >= GPIO_NUMBER_OF_PORTS)
    {
        local_Status = E_NOK;
    }
    else
    {
        volatile uint8_h *portReg = NULL;
        switch (uint8Port)
        {
            case GPIO_PORTA: portReg = &GPIO_PORTA_REG; break;
            case GPIO_PORTB: portReg = &GPIO_PORTB_REG; break;
            case GPIO_PORTC: portReg = &GPIO_PORTC_REG; break;
            case GPIO_PORTD: portReg = &GPIO_PORTD_REG; break;
            default: local_Status = E_NOK; break;
        }

        if (local_Status == E_OK)
        {
            *portReg = (uint8_h)uint8Value;
        }
    }

    return local_Status;
}
/*************************************************************************************************************** */