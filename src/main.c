#include "../Service/STD_Types.h"
#include "../Service/Bit_Math.h"
#include "../MCL/GPIO/gpio_interface.h"
#include "../MCL/GPIO/gpio_registers.h"
#include "../MCL/UART/uart_interface.h"

/*
 * UART test application.
 *
 * Wiring:
 *   LED   -> PORTA PIN0 (through a series resistor to GND, active high)
 *   UART  -> PD0 (RXD) / PD1 (TXD), 9600 baud, 8N1
 *
 * Behaviour:
 *   - Receiving '1' turns the LED ON,  receiving '0' turns it OFF.
 *   - After every change the board prints the LED state back on the UART:
 *         "1" while the LED is on, "0" while it is off.
 *   - Any other character is rejected with a short hint.
 */

#define LED_PORT        GPIO_PORTA
#define LED_PIN         GPIO_PIN0

#define LED_ON_CHAR     '1'
#define LED_OFF_CHAR    '0'

/* Sends the current LED state as a single character line: "1" or "0". */
static void PrintLedState(uint8_h uint8State)
{
    (void)UART_SendByte(uint8State ? LED_ON_CHAR : LED_OFF_CHAR);
    (void)UART_SendString((const uint8_h *)"\r\n");
}

int main(void)
{
    UART_ConfigType uartConfig =
    {
        UART_BAUD_9600,     /* baudRate : 9600 bps                */
        UART_DATA_8BITS,    /* dataSize : 8 data bits             */
        UART_PARITY_NONE,   /* parity   : no parity bit           */
        UART_STOP_1BIT      /* stopBits : 1 stop bit  -> 8N1      */
    };

    uint8_h ledState = 0U;   /* 0 = off, 1 = on */
    uint8_h rxByte   = 0U;

    /* LED pin as output, starting in the OFF state. */
    (void)GPIO_SetPinDirection(LED_PORT, LED_PIN, GPIO_OUTPUT);
    (void)GPIO_SetPinValue(LED_PORT, LED_PIN, PIN_LOW);

    (void)UART_Init(&uartConfig);

    (void)UART_SendString((const uint8_h *)"LED control ready: send 1 = ON, 0 = OFF\r\n");
    PrintLedState(ledState);   /* report the initial state */

    while (1)
    {
        /* Blocks until a character arrives from the terminal. */
        if (UART_ReceiveByte(&rxByte) != E_OK)
        {
            continue;
        }

        switch (rxByte)
        {
            case LED_ON_CHAR:
                ledState = 1U;
                (void)GPIO_SetPinValue(LED_PORT, LED_PIN, PIN_HIGH);
                PrintLedState(ledState);
                break;

            case LED_OFF_CHAR:
                ledState = 0U;
                (void)GPIO_SetPinValue(LED_PORT, LED_PIN, PIN_LOW);
                PrintLedState(ledState);
                break;

            case '\r':
            case '\n':
                /* Ignore the line endings a terminal appends after the digit. */
                break;

            default:
                (void)UART_SendString((const uint8_h *)"Send 1 or 0\r\n");
                break;
        }
    }

    return 0;
}
