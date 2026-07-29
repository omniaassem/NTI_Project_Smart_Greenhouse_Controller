#ifndef MICRO_CONTROL_H
#define MICRO_CONTROL_H

#include "../../Service/STD_Types.h"

typedef enum {
    CONTROL_OK = 0,
    CONTROL_ERROR
} CONTROL_Status_t;

CONTROL_Status_t CTRL_Init(void);
CONTROL_Status_t CTRL_Update(void);

#endif // MICRO_CONTROL_H
