#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include "../../Service/STD_TYPES.h"
#include <stdint.h> 

typedef void (*SCH_TaskFunc_t)(void);

typedef enum {
    SCH_OK = 0,
    SCH_ERROR
} SCH_Status_t;

#define SCH_MAX_TASKS 5u

SCH_Status_t SCH_Init(void);
SCH_Status_t SCH_AddTask(SCH_TaskFunc_t task, uint16_t periodMs);
void SCH_Dispatch(void);
void SCH_Tick(void);

#endif /* SCHEDULER_H_ */