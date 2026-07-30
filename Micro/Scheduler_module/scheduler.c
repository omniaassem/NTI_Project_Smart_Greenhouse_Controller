#include "scheduler.h"

/* هيكل بيانات المهمة */
typedef struct {
    SCH_TaskFunc_t task;
    uint16_t periodMs;
    volatile uint16_t elapsedMs; /* volatile لمنع التخزين المؤقت في المكملات بسب الاستدعاء داخل الـ ISR */
    uint8_t active;
} SCH_Task_t;

static SCH_Task_t taskTable[SCH_MAX_TASKS];
static uint8_t taskCount = 0u;

/**
 * @brief تهيئة المجدول وتصفير كافة المهام
 */
SCH_Status_t SCH_Init(void)
{
    for (uint8_t i = 0u; i < SCH_MAX_TASKS; ++i)
    {
        taskTable[i].task = 0;
        taskTable[i].periodMs = 0u;
        taskTable[i].elapsedMs = 0u;
        taskTable[i].active = 0u;
    }
    taskCount = 0u;
    return SCH_OK;
}

/**
 * @brief إضافة مهمة جديدة للمجدول
 * @param task مؤشر للدالة المراد تنفيذها
 * @param periodMs معدل تكرار المهمة بالمللي ثانية
 */
SCH_Status_t SCH_AddTask(SCH_TaskFunc_t task, uint16_t periodMs)
{
    if ((task == 0) || (taskCount >= SCH_MAX_TASKS))
    {
        return SCH_ERROR;
    }

    taskTable[taskCount].task = task;
    taskTable[taskCount].periodMs = periodMs;
    taskTable[taskCount].elapsedMs = 0u;
    taskTable[taskCount].active = 1u;
    taskCount++;

    return SCH_OK;
}

/**
 * @brief تنفيذ المهام التي حان وقتها (تُستدعى باستمرار داخل الـ Super-Loop في main)
 */
void SCH_Dispatch(void)
{
    for (uint8_t i = 0u; i < taskCount; ++i)
    {
        if ((taskTable[i].active != 0u) &&
            (taskTable[i].elapsedMs >= taskTable[i].periodMs))
        {
            /* تنفيذ المهمة */
            taskTable[i].task();
            
            /* تصفير العداد لإعادة الدورة */
            taskTable[i].elapsedMs = 0u;
        }
    }
}

/**
 * @brief التكة الزمنية للمجدول (تُستدعى كل 1ms داخل مقاطعة تايمر العتاد Timer ISR)
 */
void SCH_Tick(void)
{
    for (uint8_t i = 0u; i < taskCount; ++i)
    {
        if (taskTable[i].active != 0u)
        {
            taskTable[i].elapsedMs++;
        }
    }
}
