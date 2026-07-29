#ifndef SCHEDULER_MODULE_H
#define SCHEDULER_MODULE_H

#include "../../Service/STD_Types.h"

typedef void (*Scheduler_TaskFunc_t)(void);

STD_ReturnType SCH_Init(void);
STD_ReturnType SCH_AddTask(Scheduler_TaskFunc_t task, uint16_h periodMs);
void SCH_Tick(void);
void SCH_Dispatch(void);

#endif /* SCHEDULER_MODULE_H */
