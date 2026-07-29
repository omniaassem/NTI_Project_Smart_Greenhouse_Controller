#include "Buttons_Driver.h"
#include "../../MCAL/GPIO/gpio_interface.h"

#define BTN_DEBOUNCE_SAMPLES    2U

typedef struct
{
    uint8_h currentState;
    uint8_h lastRawState;
    uint8_h debounceCount;
    uint8_h edgeFlag;
} ButtonDebounceType;

static ButtonDebounceType g_buttonState[BTN_COUNT];

static uint8_h BTN_GetPinForButton(ButtonType button, uint8_h *pPort, uint8_h *pPin)
{
    if ((pPort == NULL) || (pPin == NULL))
    {
        return 0U;
    }

    switch (button)
    {
        case BTN_MODE:  *pPort = GPIO_PORTD; *pPin = GPIO_PIN2; return 1U;
        case BTN_SAVE:  *pPort = GPIO_PORTD; *pPin = GPIO_PIN3; return 1U;
        case BTN_RESET: *pPort = GPIO_PORTD; *pPin = GPIO_PIN4; return 1U;
        default:        return 0U;
    }
}

STD_ReturnType BTN_Init(void)
{
    uint8_h index;

    for (index = 0U; index < BTN_COUNT; ++index)
    {
        g_buttonState[index].currentState = BTN_RELEASED;
        g_buttonState[index].lastRawState = BTN_RELEASED;
        g_buttonState[index].debounceCount = 0U;
        g_buttonState[index].edgeFlag = 0U;
    }

    if (GPIO_SetPinDirection(GPIO_PORTD, GPIO_PIN2, GPIO_INPUT) != E_OK)
    {
        return E_NOK;
    }
    if (GPIO_SetPinDirection(GPIO_PORTD, GPIO_PIN3, GPIO_INPUT) != E_OK)
    {
        return E_NOK;
    }
    if (GPIO_SetPinDirection(GPIO_PORTD, GPIO_PIN4, GPIO_INPUT) != E_OK)
    {
        return E_NOK;
    }

    (void)GPIO_SetPinValue(GPIO_PORTD, GPIO_PIN2, PIN_HIGH);
    (void)GPIO_SetPinValue(GPIO_PORTD, GPIO_PIN3, PIN_HIGH);
    (void)GPIO_SetPinValue(GPIO_PORTD, GPIO_PIN4, PIN_HIGH);

    return E_OK;
}

STD_ReturnType BTN_Poll(void)
{
    uint8_h index;

    for (index = 0U; index < BTN_COUNT; ++index)
    {
        uint8_h port = 0U;
        uint8_h pin = 0U;
        uint8_h rawState;
        ButtonDebounceType *pButton = &g_buttonState[index];

        if (BTN_GetPinForButton((ButtonType)index, &port, &pin) == 0U)
        {
            continue;
        }

        rawState = (GPIO_GetPinStatus(port, pin) == PIN_LOW) ? BTN_PRESSED : BTN_RELEASED;

        if (rawState != pButton->lastRawState)
        {
            pButton->lastRawState = rawState;
            pButton->debounceCount = 0U;
            continue;
        }

        if (pButton->debounceCount < BTN_DEBOUNCE_SAMPLES)
        {
            pButton->debounceCount++;
        }

        if (pButton->debounceCount == BTN_DEBOUNCE_SAMPLES)
        {
            if (pButton->currentState != rawState)
            {
                pButton->currentState = rawState;
                if (rawState == BTN_PRESSED)
                {
                    pButton->edgeFlag = 1U;
                }
            }
        }
    }

    return E_OK;
}

STD_ReturnType BTN_GetState(ButtonType button, ButtonStateType *pState)
{
    if (pState == NULL)
    {
        return E_NOK;
    }

    if (button >= BTN_COUNT)
    {
        return E_NOK;
    }

    *pState = (ButtonStateType)g_buttonState[button].currentState;
    return E_OK;
}

STD_ReturnType BTN_WasPressed(ButtonType button, uint8_h *pPressed)
{
    if (pPressed == NULL)
    {
        return E_NOK;
    }

    if (button >= BTN_COUNT)
    {
        return E_NOK;
    }

    *pPressed = g_buttonState[button].edgeFlag;
    g_buttonState[button].edgeFlag = 0U;

    return E_OK;
}

STD_ReturnType BTN_ClearEdge(ButtonType button)
{
    if (button >= BTN_COUNT)
    {
        return E_NOK;
    }

    g_buttonState[button].edgeFlag = 0U;
    return E_OK;
}
