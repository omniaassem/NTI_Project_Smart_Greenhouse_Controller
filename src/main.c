#include <util/delay.h>

#include "STD_Types.h"
#include "Bit_Math.h"

#include "uart_interface.h"
#include "i2c_interface.h"

#include "Sensors_Driver.h"
#include "Actuators_Driver.h"
#include "lcd_aip31068_i2c.h"

#include "console.h"
#include "greenhouse_fsm.h"
#include "report.h"

int main(void)
{
    /* UART */
    UART_ConfigType uartConfig =
    {
        .baudRate = UART_BAUD_9600,
        .dataSize = UART_DATA_8BITS,
        .parity   = UART_PARITY_NONE,
        .stopBits = UART_STOP_1BIT
    };

    (void)UART_Init(&uartConfig);

    /* I2C */
    I2C_MasterConfigType i2cConfig =
    {
        .sclFrequency = 100000UL
    };

    (void)I2C_InitMaster(&i2cConfig);

    /* LCD */
    LCD_Aip31068_HandleType lcd =
    {
        .i2cAddress = 0x3E,
        .rows = 2,
        .cols = 16
    };

    (void)LCD_Aip31068_Init(&lcd);

    /* Drivers */
    (void)Sensors_Init();
    (void)ACT_Init();

    /* Middleware */
    (void)FSM_Init();
    (void)CON_Init();
    (void)RPT_Init();

    (void)LCD_Aip31068_Clear(&lcd);
    (void)LCD_Aip31068_WriteStringAt(&lcd, 0, 0, (uint8_h *)"Greenhouse");
    (void)LCD_Aip31068_WriteStringAt(&lcd, 1, 0, (uint8_h *)"System Ready");
while (1)
{
    FSM_Run();

    CON_Process();

    RPT_SendStatus();

    _delay_ms(1000);
}
    return 0;
}