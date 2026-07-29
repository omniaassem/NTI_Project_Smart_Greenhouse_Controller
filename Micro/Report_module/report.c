#include "report.h"
#include "../../Service/STD_Types.h"
#include "../../MCL/UART/uart_interface.h"

REPORT_Status_t RPT_Init(void)
{
    /* Initialize telemetry/reporting subsystem here */
    return REPORT_OK;
}

REPORT_Status_t RPT_SendStatus(void)
{
    /* Format and send report data over UART here */
    return REPORT_OK;
}
