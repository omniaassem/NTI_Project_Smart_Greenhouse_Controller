#ifndef MICRO_REPORT_H
#define MICRO_REPORT_H

#include "../../Service/STD_Types.h"

typedef enum {
    REPORT_OK = 0,
    REPORT_ERROR
} REPORT_Status_t;

REPORT_Status_t RPT_Init(void);
REPORT_Status_t RPT_SendStatus(void);

#endif // MICRO_REPORT_H
