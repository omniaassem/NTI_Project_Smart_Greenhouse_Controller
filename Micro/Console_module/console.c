#include "console.h"
#include "../../Service/STD_Types.h"
#include "../../MCL/UART/uart_interface.h"
#include "../Sensors/Sensors_Driver.h"
#include <string.h>
#include <stdio.h>

#define CONSOLE_RX_BUFFER_SIZE  64

static char g_rxBuffer[CONSOLE_RX_BUFFER_SIZE];
static uint8_h g_rxIndex = 0;

/* دالة مساعدة لإرسال سلسلة نصية عبر الـ UART */
static void CON_SendString(const char *str)
{
    while (*str != '\0')
    {
        UART_SendByte((uint8_h)*str);
        str++;
    }
}

/* دالة تحليل وتنفيذ الأوامر المستقبلة */
static void CON_ParseAndExecute(char *cmdBuffer)
{
    /* أمر قراءة الحساسات وعرض القراءات */
    if (strcmp(cmdBuffer, "read") == 0 || strcmp(cmdBuffer, "READ") == 0)
    {
        uint16_h tempRaw = 0, soilRaw = 0, lightRaw = 0;
        uint8_h tempC = 0, soilPct = 0, lightPct = 0;

        if (Sensors_ReadRaw(&tempRaw, &soilRaw, &lightRaw) == E_OK)
        {
            (void)Sensors_ScaleTempC(tempRaw, &tempC);
            (void)Sensors_ScalePct(soilRaw, &soilPct);
            (void)Sensors_ScalePct(lightRaw, &lightPct);

            char outBuf[80];
            snprintf(outBuf, sizeof(outBuf),
                     "\r\n--- Sensor Readings ---\r\nTemp: %u C\r\nSoil Moisture: %u %%\r\nLight Level: %u %%\r\n",
                     tempC, soilPct, lightPct);
            CON_SendString(outBuf);
        }
        else
        {
            CON_SendString("\r\nError: Failed to read sensors!\r\n");
        }
    }
    /* أمر المساعدة */
    else if (strcmp(cmdBuffer, "help") == 0 || strcmp(cmdBuffer, "HELP") == 0)
    {
        CON_SendString("\r\nAvailable Commands:\r\n");
        CON_SendString("  read - Read all sensors (Temp, Soil, Light)\r\n");
        CON_SendString("  help - Display this help message\r\n");
    }
    else if (cmdBuffer[0] != '\0')
    {
        CON_SendString("\r\nUnknown command. Type 'help' for available commands.\r\n");
    }

    CON_SendString("\r\nConsole> ");
}

CONSOLE_Status_t CON_Init(void)
{
    memset(g_rxBuffer, 0, CONSOLE_RX_BUFFER_SIZE);
    g_rxIndex = 0;

    CON_SendString("\r\n=================================\r\n");
    CON_SendString("   Embedded System CLI Ready     \r\n");
    CON_SendString("=================================\r\n");
    CON_SendString("Console> ");

    return CONSOLE_OK;
}

CONSOLE_Status_t CON_Process(void)
{
    uint8_h receivedByte = 0;

    /* استقبال البايتات من الـ UART بشكل غير حاجب */
    if (UART_ReceiveByteNonBlocking(&receivedByte) == E_OK)
    {
        /* عند ضغط Enter (\r أو \n) */
        if (receivedByte == '\r' || receivedByte == '\n')
        {
            g_rxBuffer[g_rxIndex] = '\0';
            CON_ParseAndExecute(g_rxBuffer);
            g_rxIndex = 0;
        }
        /* التعامل مع Backspace لإلغاء الحرف الاخير */
        else if ((receivedByte == '\b' || receivedByte == 0x7F) && (g_rxIndex > 0))
        {
            g_rxIndex--;
            CON_SendString("\b \b");
        }
        /* تجميع الأحرف داخل الـ Buffer */
        else if (g_rxIndex < (CONSOLE_RX_BUFFER_SIZE - 1))
        {
            if (receivedByte >= 32 && receivedByte <= 126) /* الأحرف القابلة للطباعة */
            {
                g_rxBuffer[g_rxIndex++] = (char)receivedByte;
                UART_SendByte(receivedByte); /* Echo للحرف على الشاشة */
            }
        }
    }

    return CONSOLE_OK;
}
