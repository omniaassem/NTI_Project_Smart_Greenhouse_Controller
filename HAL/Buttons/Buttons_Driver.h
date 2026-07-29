#ifndef BUTTONS_DRIVER_H
#define BUTTONS_DRIVER_H

#include "../../Service/STD_Types.h"

typedef enum
{
    BTN_MODE = 0,
    BTN_SAVE,
    BTN_RESET,
    BTN_COUNT
} ButtonType;

typedef enum
{
    BTN_RELEASED = 0,
    BTN_PRESSED = 1
} ButtonStateType;

STD_ReturnType BTN_Init(void);
STD_ReturnType BTN_Poll(void);
STD_ReturnType BTN_GetState(ButtonType button, ButtonStateType *pState);
STD_ReturnType BTN_WasPressed(ButtonType button, uint8_h *pPressed);
STD_ReturnType BTN_ClearEdge(ButtonType button);

#endif /* BUTTONS_DRIVER_H */
