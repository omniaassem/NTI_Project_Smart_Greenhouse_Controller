#include "Scheduler_Module.h"
#include "../../MCAL/Timer/timer_interface.h"

#define SCHEDULER_TICK_MS     10U
#define SCHEDULER_MAX_TASKS   8U

typedef struct
{
    Scheduler_TaskFunc_t task;
    uint16_h            periodTicks;
    uint16_h            remainingTicks;
    uint8_h             active;
} Scheduler_TaskType;

static volatile uint16_h g_schedulerTickCount = 0U;
static Scheduler_TaskType g_schedulerTasks[SCHEDULER_MAX_TASKS];

STD_ReturnType SCH_Init(void)
{
    for (uint8_h index = 0U; index < SCHEDULER_MAX_TASKS; index++)
    {
        g_schedulerTasks[index].task          = NULL;
        g_schedulerTasks[index].periodTicks   = 0U;
        g_schedulerTasks[index].remainingTicks = 0U;
        g_schedulerTasks[index].active        = 0U;
    }

    return E_OK;
}

STD_ReturnType SCH_AddTask(Scheduler_TaskFunc_t task, uint16_h periodMs)
{
    if ((task == NULL) || (periodMs == 0U))
    {
        return E_NOK;
    }

    uint16_h periodTicks = (periodMs + SCHEDULER_TICK_MS - 1U) / SCHEDULER_TICK_MS;
    if (periodTicks == 0U)
    {
        periodTicks = 1U;
    }

    for (uint8_h index = 0U; index < SCHEDULER_MAX_TASKS; index++)
    {
        if (g_schedulerTasks[index].active == 0U)
        {
            g_schedulerTasks[index].task           = task;
            g_schedulerTasks[index].periodTicks    = periodTicks;
            g_schedulerTasks[index].remainingTicks = periodTicks;
            g_schedulerTasks[index].active         = 1U;
            return E_OK;
        }
    }

    return E_NOK;
}

void SCH_Tick(void)
{
    g_schedulerTickCount++;
}

void SCH_Dispatch(void)
{
    if (g_schedulerTickCount == 0U)
    {
        return;
    }

    uint16_h localTicks;
    Timer_DisableGlobalInterrupt();
    localTicks = g_schedulerTickCount;
    g_schedulerTickCount = 0U;
    Timer_EnableGlobalInterrupt();

    while (localTicks-- > 0U)
    {
        for (uint8_h index = 0U; index < SCHEDULER_MAX_TASKS; index++)
        {
            if (g_schedulerTasks[index].active == 0U)
            {
                continue;
            }

            if (g_schedulerTasks[index].remainingTicks > 0U)
            {
                g_schedulerTasks[index].remainingTicks--;
            }

            if (g_schedulerTasks[index].remainingTicks == 0U)
            {
                g_schedulerTasks[index].task();
                g_schedulerTasks[index].remainingTicks = g_schedulerTasks[index].periodTicks;
            }
        }
    }
}
