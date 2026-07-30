#include <util/delay.h>
#include <string.h>
#include <stdio.h>

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
    (void)LCD_Aip31068_WriteStringAt(&lcd, 0, 0, (uint8_h *)"Greenhouse System");
    (void)LCD_Aip31068_WriteStringAt(&lcd, 1, 0, (uint8_h *)"Initializing...  ");
    _delay_ms(1000);

    uint16_h updateCounter = 0;

    while (1)
    {
        FSM_Run();

        CON_Process();

        RPT_SendStatus();

        /* تحديث الشاشة دورياً بقراءات الحساسات وحالة المشغلات الحقيقية */
        updateCounter++;
        if (updateCounter >= 5) // التحديث كل دورات منتظمة
        {
            updateCounter = 0;

            // جلب القيم الحقيقية من وحدة التقارير أو الدريفرات
            sint32 temp  = RPT_GetTemp();
            sint32 soil  = RPT_GetSoil();
            sint32 light = RPT_GetLight();

            uint8_h fan  = RPT_GetFanState();
            uint8_h pump = RPT_GetPumpState();
            uint8_h lamp = RPT_GetLampState();

            // --- السطر الأول: قراءات الحساسات ---
            char line1[17];
            snprintf(line1, sizeof(line1), "T:%ldC S:%ld%% L:%ld%%", (long)temp, (long)soil, (long)light);
            for (uint8_h i = strlen(line1); i < 16; i++) { line1[i] = ' '; }
            line1[16] = '\0';
            (void)LCD_Aip31068_WriteStringAt(&lcd, 0, 0, (uint8_h *)line1);

            // --- السطر الثاني: حالات المشغلات والمود ---
            char line2[17];
            snprintf(line2, sizeof(line2), "F:%s P:%s AUTO", 
                     fan ? "ON" : "OF", 
                     pump ? "ON" : "OF");
            for (uint8_h i = strlen(line2); i < 16; i++) { line2[i] = ' '; }
            line2[16] = '\0';
            (void)LCD_Aip31068_WriteStringAt(&lcd, 1, 0, (uint8_h *)line2);
        }

        _delay_ms(50);
    }
    return 0;
}