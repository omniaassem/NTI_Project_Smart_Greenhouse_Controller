#include "../Service/STD_Types.h"
#include "../Service/Bit_Math.h"
#include "../MCAL/GPIO/gpio_interface.h"
#include "../MCAL/UART/uart_interface.h"
#include "../HAL/DC_Motor/dc_motor.h"

static void DelayMs(uint16_h ms)
{
    volatile uint16_h outer;
    volatile uint16_h inner;

    for (outer = 0U; outer < ms; ++outer)
    {
        for (inner = 0U; inner < 6000U; ++inner)
        {
            __asm__ volatile ("nop");
        }
    }
}

static void SendLine(const char *message)
{
    (void)UART_SendString((const uint8_h *)message);
    (void)UART_SendString((const uint8_h *)"\r\n");
}

int main(void)
{
    UART_ConfigType uartConfig =
    {
        .baudRate = UART_BAUD_9600,
        .dataSize = UART_DATA_8BITS,
        .parity   = UART_PARITY_NONE,
        .stopBits = UART_STOP_1BIT
    };

    DC_MotorHandleType Pump = {0};
    DC_MotorHandleType Fan = {0};

    /* إعدادات طلمبة المياه (Pump) على B0 و B1 */
    Pump.in1Port         = GPIO_PORTB;
    Pump.in1Pin          = GPIO_PIN0;
    Pump.in2Port         = GPIO_PORTB;
    Pump.in2Pin          = GPIO_PIN1;
    Pump.pwmChannel      = DC_MOTOR_PWM_NONE; 
    Pump.enPort          = GPIO_PORTB;
    Pump.enPin           = GPIO_PIN3;
    Pump.invertDirection = 0U;

    /* إعدادات المروحة (Fan) على B4 و B5 */
    Fan.in1Port          = GPIO_PORTB;
    Fan.in1Pin           = GPIO_PIN4;
    Fan.in2Port          = GPIO_PORTB;
    Fan.in2Pin           = GPIO_PIN5;
    Fan.pwmChannel       = DC_MOTOR_PWM_NONE; 
    Fan.enPort           = GPIO_PORTB;
    Fan.enPin            = GPIO_PIN3;
    Fan.invertDirection  = 0U;

    (void)UART_Init(&uartConfig);

    if (DC_Motor_Init(&Pump) != E_OK)
    {
        SendLine("Greenhouse: Pump init failed");
        while (1)
        {
        }
    }

    if (DC_Motor_Init(&Fan) != E_OK)
    {
        SendLine("Greenhouse: Fan init failed");
        while (1)
        {
        }
    }

    SendLine("Greenhouse motor test started");

    while (1)
    {
        SendLine("Forward");
        (void)DC_Motor_SetSpeed(&Pump, 70U);
        (void)DC_Motor_SetSpeed(&Fan, 70U);
        (void)DC_Motor_Forward(&Pump);
        (void)DC_Motor_Forward(&Fan);
        DelayMs(2000U);

        SendLine("Stop");
        (void)DC_Motor_Stop(&Pump);
        (void)DC_Motor_Stop(&Fan);
        DelayMs(1000U);

        SendLine("Backward");
        (void)DC_Motor_SetSpeed(&Pump, 70U);
        (void)DC_Motor_SetSpeed(&Fan, 70U);
        (void)DC_Motor_Backward(&Pump);
        (void)DC_Motor_Backward(&Fan);
        DelayMs(2000U);

        SendLine("Brake");
        (void)DC_Motor_Brake(&Pump);
        (void)DC_Motor_Brake(&Fan);
        DelayMs(1000U);

        SendLine("Stop");
        (void)DC_Motor_Stop(&Pump);
        (void)DC_Motor_Stop(&Fan);
        DelayMs(1000U);
    }

    return 0;
}