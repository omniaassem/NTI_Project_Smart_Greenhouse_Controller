#include "../Service/STD_Types.h"
#include "../Service/Bit_Math.h"
#include "../MCAL/Timer/timer_interface.h"
#include "../MCAL/UART/uart_interface.h"
#include "../HAL/Sensors/Sensors_Driver.h"
#include "../Micro/Scheduler_module/scheduler.h"

static void UART_PrintUnsigned(uint16_h value)
{
    char buffer[6] = {'0', '0', '0', '0', '0', '\0'};
    uint8_h index = 0U;

    if (value == 0U)
    {
        (void)UART_SendString((const uint8_h *)"0");
        return;
    }

    while ((value > 0U) && (index < 5U))
    {
        buffer[index++] = (char)('0' + (value % 10U));
        value /= 10U;
    }

    while (index > 0U)
    {
        index--;
        (void)UART_SendByte((uint8_h)buffer[index]);
    }
}

static void UART_PrintLine(const char *label, uint16_h value)
{
    (void)UART_SendString((const uint8_h *)label);
    UART_PrintUnsigned(value);
    (void)UART_SendString((const uint8_h *)"\r\n");
}

/* مهمة المجدول لقراءة وإرسال قراءات الحساسات */
static void SensorTask(void)
{
    uint16_h rawTemp  = 0U;
    uint16_h rawSoil  = 0U;
    uint16_h rawLight = 0U;
    uint8_h  tempC    = 0U;
    uint8_h  soilPct  = 0U;
    uint8_h  lightPct = 0U;

    if (Sensors_ReadRaw(&rawTemp, &rawSoil, &rawLight) == E_OK)
    {
        (void)Sensors_ScaleTempC(rawTemp, &tempC);
        (void)Sensors_ScalePct(rawSoil, &soilPct);
        (void)Sensors_ScalePct(lightPct, &lightPct);

        UART_PrintLine("Raw Temp: ", rawTemp);
        UART_PrintLine("Temp C:  ", tempC);
        UART_PrintLine("Soil %:  ", soilPct);
        UART_PrintLine("Light %: ", lightPct);
        (void)UART_SendString((const uint8_h *)"------\r\n");
    }
    else
    {
        (void)UART_SendString((const uint8_h *)"Sensor read failed\r\n");
    }
}

int sensor_demo_main(void)
{
    UART_ConfigType uartConfig =
    {
        .baudRate = UART_BAUD_9600,
        .dataSize = UART_DATA_8BITS,
        .parity   = UART_PARITY_NONE,
        .stopBits = UART_STOP_1BIT
    };

    /* ضبط التايمر0 ليعطي مقاطعة كل 1ms لاستدعاء SCH_Tick */
    Timer_ConfigType timerConfig =
    {
        .channel      = TIMER_CHANNEL_0,
        .mode         = TIMER_MODE_CTC,
        .prescaler    = TIMER_CLOCK_DIV_1024,
        .initialValue = 0U,
        .compareValue = 155U
    };

    (void)UART_Init(&uartConfig);

    if (Sensors_Init() != E_OK)
    {
        (void)UART_SendString((const uint8_h *)"Sensor init failed\r\n");
        while (1) { }
    }

    (void)SCH_Init();

    if (Timer_Init(&timerConfig) != E_OK)
    {
        (void)UART_SendString((const uint8_h *)"Timer init failed\r\n");
        while (1) { }
    }

    /* ربط الـ Callback الخاص بالتايمر بالمجدول */
    (void)Timer_SetCallBack(TIMER_CHANNEL_0,
                            TIMER_INT_COMPARE_MATCH,
                            SCH_Tick);
    (void)Timer_EnableInterrupt(TIMER_CHANNEL_0, TIMER_INT_COMPARE_MATCH);
    Timer_EnableGlobalInterrupt();

    /* إضافة مهمة الحساسات لتعمل كل 1000ms (ثانية واحدة) */
    if (SCH_AddTask(SensorTask, 1000U) != E_OK)
    {
        (void)UART_SendString((const uint8_h *)"Scheduler task add failed\r\n");
        while (1) { }
    }

    (void)UART_SendString((const uint8_h *)"Scheduler running\r\n");

    while (1)
    {
        SCH_Dispatch();
    }

    return 0;
}
