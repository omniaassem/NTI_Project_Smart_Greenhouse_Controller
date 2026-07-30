#include "Actuators_Driver.h"
#include "../../MCAL/GPIO/gpio_interface.h"

#define ACTUATOR_FAN_PIN      GPIO_PIN4  // لتتوافق مع IN3 (المتصل بـ B4 في الرسمة)
#define ACTUATOR_PUMP_PIN     GPIO_PIN0  // لتتوافق مع IN1 (المتصل بـ B0 في الرسمة)
#define ACTUATOR_LAMP_PIN     GPIO_PIN2
#define ACTUATOR_ALARM_PIN    GPIO_PIN3

static uint8_h ACT_GetPinForActuator(ActuatorType actuator, uint8_h *pPort, uint8_h *pPin)
{
    if ((pPort == NULL) || (pPin == NULL))
    {
        return 0U;
    }

    switch (actuator)
    {
        case ACTUATOR_FAN:   *pPort = GPIO_PORTB; *pPin = ACTUATOR_FAN_PIN;   return 1U;
        case ACTUATOR_PUMP:  *pPort = GPIO_PORTB; *pPin = ACTUATOR_PUMP_PIN;  return 1U;
        case ACTUATOR_LAMP:  *pPort = GPIO_PORTB; *pPin = ACTUATOR_LAMP_PIN;  return 1U;
        case ACTUATOR_ALARM:*pPort = GPIO_PORTB; *pPin = ACTUATOR_ALARM_PIN; return 1U;
        default:            return 0U;
    }
}

STD_ReturnType ACT_Init(void)
{
    if (GPIO_SetPinDirection(GPIO_PORTB, ACTUATOR_FAN_PIN, GPIO_OUTPUT) != E_OK)
    {
        return E_NOK;
    }
    if (GPIO_SetPinDirection(GPIO_PORTB, ACTUATOR_PUMP_PIN, GPIO_OUTPUT) != E_OK)
    {
        return E_NOK;
    }
    if (GPIO_SetPinDirection(GPIO_PORTB, ACTUATOR_LAMP_PIN, GPIO_OUTPUT) != E_OK)
    {
        return E_NOK;
    }
    if (GPIO_SetPinDirection(GPIO_PORTB, ACTUATOR_ALARM_PIN, GPIO_OUTPUT) != E_OK)
    {
        return E_NOK;
    }

    (void)GPIO_SetPinValue(GPIO_PORTB, ACTUATOR_FAN_PIN, PIN_LOW);
    (void)GPIO_SetPinValue(GPIO_PORTB, ACTUATOR_PUMP_PIN, PIN_LOW);
    (void)GPIO_SetPinValue(GPIO_PORTB, ACTUATOR_LAMP_PIN, PIN_LOW);
    (void)GPIO_SetPinValue(GPIO_PORTB, ACTUATOR_ALARM_PIN, PIN_LOW);

    return E_OK;
}

STD_ReturnType ACT_Set(ActuatorType actuator, ActuatorStateType state)
{
    uint8_h port = 0U;
    uint8_h pin = 0U;

    if (ACT_GetPinForActuator(actuator, &port, &pin) == 0U)
    {
        return E_NOK;
    }

    if ((state != ACT_STATE_OFF) && (state != ACT_STATE_ON))
    {
        return E_NOK;
    }

    return GPIO_SetPinValue(port, pin, (state == ACT_STATE_ON) ? PIN_HIGH : PIN_LOW);
}

STD_ReturnType ACT_SetAll(ActuatorStateType fanState,
                          ActuatorStateType pumpState,
                          ActuatorStateType lampState,
                          ActuatorStateType alarmState)
{
    if ((ACT_Set(ACTUATOR_FAN, fanState) != E_OK) ||
        (ACT_Set(ACTUATOR_PUMP, pumpState) != E_OK) ||
        (ACT_Set(ACTUATOR_LAMP, lampState) != E_OK) ||
        (ACT_Set(ACTUATOR_ALARM, alarmState) != E_OK))
    {
        return E_NOK;
    }

    return E_OK;
}

STD_ReturnType ACT_Get(ActuatorType actuator, ActuatorStateType *pState)
{
    uint8_h port = 0U;
    uint8_h pin = 0U;
    GPIO_PINStatus level;

    if (pState == NULL)
    {
        return E_NOK;
    }

    if (ACT_GetPinForActuator(actuator, &port, &pin) == 0U)
    {
        return E_NOK;
    }

    level = GPIO_GetPinStatus(port, pin);
    *pState = (level == PIN_HIGH) ? ACT_STATE_ON : ACT_STATE_OFF;

    return E_OK;
}
