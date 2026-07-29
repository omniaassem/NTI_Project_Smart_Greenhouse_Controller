#include "../Service/STD_Types.h"
#include "../Service/Bit_Math.h"
#include "../MCL/GPIO/gpio_interface.h"
#include "../MCL/UART/uart_interface.h"
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

    DC_MotorHandleType motorLeft = {0};
    DC_MotorHandleType motorRight = {0};

    motorLeft.in1Port      = GPIO_PORTC;
    motorLeft.in1Pin       = GPIO_PIN0;
    motorLeft.in2Port      = GPIO_PORTC;
    motorLeft.in2Pin       = GPIO_PIN1;
    motorLeft.pwmChannel   = DC_MOTOR_PWM_OC1A;
    motorLeft.invertDirection = 0U;

    motorRight.in1Port     = GPIO_PORTC;
    motorRight.in1Pin      = GPIO_PIN2;
    motorRight.in2Port     = GPIO_PORTC;
    motorRight.in2Pin      = GPIO_PIN3;
    motorRight.pwmChannel  = DC_MOTOR_PWM_OC1B;
    motorRight.invertDirection = 0U;

    (void)UART_Init(&uartConfig);

    if (DC_Motor_Init(&motorLeft) != E_OK)
    {
        SendLine("Motor test: left init failed");
        while (1)
        {
        }
    }

    if (DC_Motor_Init(&motorRight) != E_OK)
    {
        SendLine("Motor test: right init failed");
        while (1)
        {
        }
    }

    SendLine("Motor test started");

    while (1)
    {
        SendLine("Forward");
        (void)DC_Motor_SetSpeed(&motorLeft, 70U);
        (void)DC_Motor_SetSpeed(&motorRight, 70U);
        (void)DC_Motor_Forward(&motorLeft);
        (void)DC_Motor_Forward(&motorRight);
        DelayMs(2000U);

        SendLine("Stop");
        (void)DC_Motor_Stop(&motorLeft);
        (void)DC_Motor_Stop(&motorRight);
        DelayMs(1000U);

        SendLine("Backward");
        (void)DC_Motor_SetSpeed(&motorLeft, 70U);
        (void)DC_Motor_SetSpeed(&motorRight, 70U);
        (void)DC_Motor_Backward(&motorLeft);
        (void)DC_Motor_Backward(&motorRight);
        DelayMs(2000U);

        SendLine("Brake");
        (void)DC_Motor_Brake(&motorLeft);
        (void)DC_Motor_Brake(&motorRight);
        DelayMs(1000U);

        SendLine("Stop");
        (void)DC_Motor_Stop(&motorLeft);
        (void)DC_Motor_Stop(&motorRight);
        DelayMs(1000U);
    }

    return 0;
}
