#include "console.h"
#include "../../Service/STD_Types.h"
#include "../../MCL/UART/uart_interface.h"

CONSOLE_Status_t CON_Init(void)
{
    /* Initialize console command parser here */
    return CONSOLE_OK;
}

CONSOLE_Status_t CON_Process(void)
{
    /* Poll UART for new commands and dispatch them here */
    return CONSOLE_OK;
}
