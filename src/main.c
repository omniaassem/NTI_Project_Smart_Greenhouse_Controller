/**************************************************************************/
/* File Name: main.c                                                      */
/* Description: Smart Greenhouse - Full System Integration Complete Code  */
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

static void App_SendLine(const char *message)
{
    (void)UART_SendString((const u8 *)message);
    (void)UART_SendString((const u8 *)"\r\n");
}

static void App_UART_PrintUnsigned(u16 value)
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

static void App_UART_PrintLine(const char *label, u16 value)
{
    (void)UART_SendString((const u8 *)label);
    App_UART_PrintUnsigned(value);
    (void)UART_SendString((const u8 *)"\r\n");
}

/* 1. مهمة قراءة الحساسات وإرسالها وعرضها */
static void App_SensorTask(void)
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

        App_UART_PrintLine("Raw Temp: ", tempRaw);
        App_UART_PrintLine("Temp C:   ", tempC);
        App_UART_PrintLine("Soil %:   ", soilPct);
        App_UART_PrintLine("Light %:  ", lightPct);
        App_SendLine("------");
    }
    else
    {
        App_SendLine("Sensor read failed");
    }
}

/* 2. مهمة التحكم التلقائي في المحركات والأكتيواتورز بناءً على القراءات */
static void App_ControlTask(void)
{
    u16 tempRaw  = 0u;
    u16 soilRaw  = 0u;
    u16 lightRaw = 0u;
    u8  tempC    = 0u;
    u8  soilPct  = 0u;

    if (Sensors_ReadRaw(&tempRaw, &soilRaw, &lightRaw) == E_OK)
    {
        (void)Sensors_ScaleTempC(tempRaw, &tempC);
        (void)Sensors_ScalePct(soilRaw, &soilPct);

        /* التحكم التلقائي في المروحة بناءً على الحرارة */
        if (tempC > 30u)
        {
            /* تشغيل المروحة إذا زادت الحرارة عن 30 */
            DC_Motor_On(); // أو دالة تشغيل المروحة المتاحة لديك
        }
        else
        {
            /* إيقاف المروحة في الوضع الطبيعي */
            DC_Motor_Off();
        }

        /* التحكم التلقائي في المضخة أو الجرس عند انخفاض رطوبة التربة */
        if (soilPct < 30u)
        {
            // تشغيل المضخة أو التنبيه
        }
    }
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

    DC_MotorHandleType motorFanConfig;
    DC_MotorHandleType motorPumpConfig;

    /* التهيئة الأساسية */
    (void)UART_Init(&uartConfig);
    (void)SCH_Init();

    if (Sensors_Init() != E_OK)
    {
        App_SendLine("Sensor init failed");
        while (1);
    }

    if (Timer_Init(&timerConfig) != E_OK)
    {
        App_SendLine("Timer init failed");
        while (1);
    }

    /* تهيئة المحركات */
    (void)DC_Motor_Init(&motorFanConfig);
    (void)DC_Motor_Init(&motorPumpConfig);

    /* ضبط التايمر وجدولة المهام */
    (void)Timer_SetCallBack(TIMER_CHANNEL_0, TIMER_INT_COMPARE_MATCH, SCH_Tick);
    (void)Timer_EnableInterrupt(TIMER_CHANNEL_0, TIMER_INT_COMPARE_MATCH);
    Timer_EnableGlobalInterrupt();

    (void)SCH_AddTask(App_SensorTask, 1000u);   /* تتفذ كل ثانية */
    (void)SCH_AddTask(App_ControlTask, 200u);   /* تتفذ كل 200 ملي ثانية للتحكم السريع */

    App_SendLine("Full System Integration Running...");

    while (1)
    {
        SCH_Dispatch();
    }

    return 0;
}