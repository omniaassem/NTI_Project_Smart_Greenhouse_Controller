/**************************************************************************/
/* File Name: main.c                                                      */
/* Description: Smart Greenhouse - Full Comprehensive Test Mode (Fixed)   */
/**************************************************************************/

#include "STD_Types.h"
#include "Bit_Math.h"
#include "ring_buffer.h"

/* MCAL Headers */
#include "gpio_interface.h"
#include "i2c_interface.h"
#include "uart_interface.h"
#include "timer_interface.h"
#include "adc_interface.h"

/* Configuration Header */
#include "config.h"

/* Scheduler & Micro Layers */
#include "scheduler.h"
#include "console.h"
#include "control.h"
#include "greenhouse_fsm.h"
#include "report.h"

/* HAL Headers */
#include "Actuators_Driver.h"
#include "Buttons_Driver.h"
#include "dc_motor.h"
#include "Sensors_Driver.h"
#include "lcd_aip31068_i2c.h"

/* External declarations for scheduler tick */
extern void SCH_Tick(void);

static void Test_SendLine(const char *message)
{
    (void)UART_SendString((const u8 *)message);
    (void)UART_SendString((const u8 *)"\r\n");
}

static void Test_UART_PrintUnsigned(u16 value)
{
    char buffer[6] = {'0', '0', '0', '0', '0', '\0'};
    u8 index = 0u;

    if (value == 0u)
    {
        (void)UART_SendString((const u8 *)"0");
        return;
    }

    while ((value > 0u) && (index < 5u))
    {
        buffer[index++] = (char)('0' + (value % 10u));
        value /= 10u;
    }

    while (index > 0u)
    {
        index--;
        (void)UART_SendByte((u8)buffer[index]);
    }
}

static void Test_UART_PrintLine(const char *label, u16 value)
{
    (void)UART_SendString((const u8 *)label);
    Test_UART_PrintUnsigned(value);
    (void)UART_SendString((const u8 *)"\r\n");
}

/* 1. مهمة قراءة الحساسات وإرسالها عبر الـ UART */
static void Test_SensorTask(void)
{
    u16 tempRaw  = 0u;
    u16 soilRaw  = 0u;
    u16 lightRaw = 0u;
    u8  tempC    = 0u;
    u8  soilPct  = 0u;
    u8  lightPct = 0u;

    if (Sensors_ReadRaw(&tempRaw, &soilRaw, &lightRaw) == E_OK)
    {
        (void)Sensors_ScaleTempC(tempRaw, &tempC);
        (void)Sensors_ScalePct(soilRaw, &soilPct);
        (void)Sensors_ScalePct(lightRaw, &lightPct);

        Test_UART_PrintLine("Raw Temp: ", tempRaw);
        Test_UART_PrintLine("Temp C:   ", tempC);
        Test_UART_PrintLine("Soil %:   ", soilPct);
        Test_UART_PrintLine("Light %:  ", lightPct);
        Test_SendLine("------");
    }
    else
    {
        Test_SendLine("Sensor read failed");
    }
}

/* 2. مهمة اختبار الأكتيواتورز */
static void Test_ActuatorsTask(void)
{
    /* يمكنك تفعيل التحكم هنا حسب الدوال المعرفة في Actuators_Driver.h */
}

int main(void)
{
    UART_ConfigType uartConfig =
    {
        .baudRate = UART_BAUD_9600,
        .dataSize = UART_DATA_8BITS,
        .parity   = UART_PARITY_NONE,
        .stopBits = UART_STOP_1BIT
    };

    Timer_ConfigType timerConfig =
    {
        .channel      = TIMER_CHANNEL_0,
        .mode         = TIMER_MODE_CTC,
        .prescaler    = TIMER_CLOCK_DIV_1024,
        .initialValue = 0u
    };

    (void)UART_Init(&uartConfig);
    (void)SCH_Init();

    if (Sensors_Init() != E_OK)
    {
        Test_SendLine("Sensor init failed");
        while (1);
    }

    if (Timer_Init(&timerConfig) != E_OK)
    {
        Test_SendLine("Timer init failed");
        while (1);
    }

    (void)Timer_SetCallBack(TIMER_CHANNEL_0, TIMER_INT_COMPARE_MATCH, SCH_Tick);
    (void)Timer_EnableInterrupt(TIMER_CHANNEL_0, TIMER_INT_COMPARE_MATCH);
    Timer_EnableGlobalInterrupt();

    (void)SCH_AddTask(Test_SensorTask, 1000u);
    (void)SCH_AddTask(Test_ActuatorsTask, 500u);

    Test_SendLine("Full Comprehensive Test Mode Running...");

    while (1)
    {
        SCH_Dispatch();
    }

    return 0;
}