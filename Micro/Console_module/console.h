#ifndef MICRO_CONSOLE_H
#define MICRO_CONSOLE_H

#include "../../Service/STD_Types.h"

typedef enum {
    CONSOLE_OK = 0,
    CONSOLE_ERROR
} CONSOLE_Status_t;

CONSOLE_Status_t CON_Init(void);
CONSOLE_Status_t CON_Process(void);

#endif // MICRO_CONSOLE_H
