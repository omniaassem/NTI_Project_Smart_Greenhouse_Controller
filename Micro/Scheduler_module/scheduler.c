#include "scheduler.h"

static struct {
    SCH_TaskFunc_t task;
    uint16_t periodMs;
    uint16_t elapsedMs;
    uint8_t active;
} taskTable[SCH_MAX_TASKS];

static uint8_t taskCount = 0u;

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

void SCH_Dispatch(void)
{
    for (uint8_t i = 0u; i < taskCount; ++i)
    {
        if ((taskTable[i].active != 0u) &&
            (taskTable[i].elapsedMs >= taskTable[i].periodMs))
        {
            taskTable[i].task();
            taskTable[i].elapsedMs = 0u;
        }
    }
}

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
