#include "Service/STD_Types.h"
#include "Service/Bit_Math.h"
#include "MCAL/GPIO/gpio_interface.h"
#include "MCAL/ADC/adc_interface.h"
#include "MCAL/UART/uart_interface.h"
#include "MCAL/Timer/timer_interface.h"

#include "HAL/Actuators/Actuators_Driver.h"
#include "HAL/Sensors/Sensors_Driver.h"
#include "HAL/LCD/lcd_aip31068_i2c.h"

#include "Micro/Scheduler_module/scheduler.h"
#include "APP/FSM/greenhouse_fsm.h"
#include "APP/Control/control.h"
#include "APP/Console/console.h"
#include "APP/Report/report.h"

/* ------------------------------------------------------------------
   1. المهام المجدولة (Scheduled Tasks)
   ------------------------------------------------------------------ */

/**
 * @brief مهمة تحديث آلة الحالات المنتهية وحلقة التحكم والكونسول
 * @note تتكرر كل 50ms لضمان الاستجابة السريعة للمدخلات والتحكم
 */
static void Task_UpdateSystem(void)
{
    (void)FSM_Run();
}

/**
 * @brief مهمة إرسال تقرير المراقبة الدوري عبر الـ UART
 * @note تتكرر كل 5000ms (5 ثوانٍ) لعرض حالة الحساسات والمشغلات والنظام
 */
static void Task_SendReport(void)
{
    (void)RPT_SendStatus();
}

/* ------------------------------------------------------------------
   2. الدالة الرئيسية (Main Function)
   ------------------------------------------------------------------ */
int main(void)
{
    /* إعدادات وحدة الـ UART */
    UART_ConfigType uartConfig =
    {
        .baudRate = UART_BAUD_9600,
        .dataSize = UART_DATA_8BITS,
        .parity   = UART_PARITY_NONE,
        .stopBits = UART_STOP_1BIT
    };

    /* إعدادات التايمر 0 ليعطي مقاطعة CTC كل 1ms بدقة لمجدول المهام */
    Timer_ConfigType timerConfig =
    {
        .channel      = TIMER_CHANNEL_0,
        .mode         = TIMER_MODE_CTC,
        .prescaler    = TIMER_CLOCK_DIV_1024,
        .initialValue = 0U,
        .compareValue = 155U /* 1ms at 16MHz clock */
    };

    /* --- أ. تهيئة طبقة الـ MCAL (المتحكم والاتصال) --- */
    (void)UART_Init(&uartConfig);

    if (Timer_Init(&timerConfig) != E_OK)
    {
        while (1) { /* خطأ حرج: فشل تهيئة التايمر */ }
    }

    /* --- ب. تهيئة طبقات التطبيق والـ FSM (آلة الحالات) --- */
    if (FSM_Init() != FSM_OK)
    {
        while (1) { /* خطأ حرج: فشل تهيئة النظام والتطبيق */ }
    }

    (void)RPT_Init();

    /* --- جـ. تهيئة المجدول الزمني وتسجيل المهام --- */
    (void)SCH_Init();

    /* إضافة مهمة النظام الرئيسية لتكرارها كل 50ms */
    (void)SCH_AddTask(Task_UpdateSystem, 50U);

    /* إضافة مهمة التقرير الدوري عبر الـ UART كل 5000ms */
    (void)SCH_AddTask(Task_SendReport, 5000U);

    /* --- د. ربط مقاطعة التايمر بالـ SCH_Tick وتفعيل المقاطعات العامة --- */
    (void)Timer_SetCallBack(TIMER_CHANNEL_0,
                            TIMER_INT_COMPARE_MATCH,
                            SCH_Tick);
                            
    (void)Timer_EnableInterrupt(TIMER_CHANNEL_0, TIMER_INT_COMPARE_MATCH);
    Timer_EnableGlobalInterrupt();

    /* --- هـ. الحلقة الرئيسية (Super Loop) --- */
    while (1)
    {
        /* إيقاد المجدول التعاوني لمعالجة وتنفيذ المهام التي حان وقتها */
        SCH_Dispatch();
    }

    return 0;
}
