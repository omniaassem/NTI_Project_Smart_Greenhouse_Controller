#include "Micro/GPIO/GPIO_Interface.h"

int main(void)
{
    GPIO_SetPinDirection(GPIO_PORTA, GPIO_PIN1, GPIO_OUTPUT);
    GPIO_PinSetValue(GPIO_PORTA, GPIO_PIN1, GPIO_HIGH);

    while (1)
    {
    }

    return 0;
}