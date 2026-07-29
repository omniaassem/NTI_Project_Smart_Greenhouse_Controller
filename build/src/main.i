# 1 "src/main.c"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "src/main.c"
# 1 "src/../Service/STD_Types.h" 1



typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned long uint32;
typedef unsigned long long uint64;

typedef signed char sint8;
typedef signed short sint16;
typedef signed long sint32;
typedef signed long long sint64;

typedef float float32;
typedef double float64;

typedef unsigned char uint8_h;
typedef unsigned short uint16_h;
typedef unsigned long uint32_h;
typedef unsigned long long uint64_h;

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;
typedef unsigned long long u64;



typedef enum
{
    E_OK = 0,
    E_NOK = 1
} STD_ReturnType;
# 2 "src/main.c" 2
# 1 "src/../Service/Bit_Math.h" 1
# 3 "src/main.c" 2
# 1 "src/../MCL/GPIO/gpio_interface.h" 1



# 1 "src/../MCL/GPIO/../../Service/STD_Types.h" 1
# 5 "src/../MCL/GPIO/gpio_interface.h" 2
# 1 "src/../MCL/GPIO/gpio_registers.h" 1
# 26 "src/../MCL/GPIO/gpio_registers.h"
typedef enum
{
    PIN_LOW = 0,
    PIN_HIGH = 1
} GPIO_PINStatus;

typedef unsigned char GPIO_PortStatus;
# 6 "src/../MCL/GPIO/gpio_interface.h" 2
# 27 "src/../MCL/GPIO/gpio_interface.h"
STD_ReturnType GPIO_SetPinDirection(uint8_h uint8Port, uint8_h uint8Pin, uint8_h uint8Direction);
# 38 "src/../MCL/GPIO/gpio_interface.h"
STD_ReturnType GPIO_SetPortDirection(uint8_h uint8Port, uint8_h uint8Direction);

GPIO_PINStatus GPIO_GetPinStatus(uint8_h uint8Port, uint8_h uint8Pin);

GPIO_PortStatus GPIO_GetPortStatus(uint8_h uint8Port);

STD_ReturnType GPIO_PinToggle(uint8_h uint8Port, uint8_h uint8Pin);
STD_ReturnType GPIO_SetPinValue(uint8_h uint8Port, uint8_h uint8Pin, uint8_h uint8Value);
STD_ReturnType GPIO_SetPortValue(uint8_h uint8Port, uint8_h uint8Value);
# 4 "src/main.c" 2
# 1 "src/../MCL/UART/uart_interface.h" 1



# 1 "src/../MCL/UART/../../Service/STD_Types.h" 1
# 5 "src/../MCL/UART/uart_interface.h" 2
# 1 "src/../MCL/UART/uart_registers.h" 1
# 6 "src/../MCL/UART/uart_interface.h" 2
# 41 "src/../MCL/UART/uart_interface.h"
typedef enum
{
    UART_DATA_5BITS = 0,
    UART_DATA_6BITS = 1,
    UART_DATA_7BITS = 2,
    UART_DATA_8BITS = 3,
    UART_DATA_9BITS = 7
} UART_DataSizeType;





typedef enum
{
    UART_PARITY_NONE = 0,
    UART_PARITY_EVEN = 2,
    UART_PARITY_ODD = 3
} UART_ParityType;





typedef enum
{
    UART_STOP_1BIT = 0,
    UART_STOP_2BIT = 1
} UART_StopBitType;
# 79 "src/../MCL/UART/uart_interface.h"
typedef struct
{
    uint32_h baudRate;
    UART_DataSizeType dataSize;
    UART_ParityType parity;
    UART_StopBitType stopBits;
} UART_ConfigType;






typedef void (*UART_RxCallBackType)(uint8_h receivedByte);
# 105 "src/../MCL/UART/uart_interface.h"
STD_ReturnType UART_Init(const UART_ConfigType *addConfig);





STD_ReturnType UART_DeInit(void);







STD_ReturnType UART_SendByte(uint8_h uint8Data);







STD_ReturnType UART_ReceiveByte(uint8_h *puint8Data);







STD_ReturnType UART_ReceiveByteNonBlocking(uint8_h *puint8Data);






STD_ReturnType UART_SendString(const uint8_h *pString);
# 152 "src/../MCL/UART/uart_interface.h"
STD_ReturnType UART_ReceiveString(uint8_h *buffer, uint16_h maxLength, uint8_h terminator);







STD_ReturnType UART_SetRxCallBack(UART_RxCallBackType callBack);
# 5 "src/main.c" 2
# 1 "src/../HAL/DC_Motor/dc_motor.h" 1



# 1 "src/../HAL/DC_Motor/../../Service/STD_Types.h" 1
# 5 "src/../HAL/DC_Motor/dc_motor.h" 2
# 1 "src/../HAL/DC_Motor/../../MCL/GPIO/gpio_interface.h" 1
# 6 "src/../HAL/DC_Motor/dc_motor.h" 2
# 86 "src/../HAL/DC_Motor/dc_motor.h"
typedef enum
{
    DC_MOTOR_PWM_NONE = 0,
    DC_MOTOR_PWM_OC0 = 1,
    DC_MOTOR_PWM_OC1A = 2,
    DC_MOTOR_PWM_OC1B = 3,
    DC_MOTOR_PWM_OC2 = 4
} DC_MotorPwmChannelType;






typedef enum
{
    DC_MOTOR_DIR_FORWARD = 0,
    DC_MOTOR_DIR_BACKWARD = 1
} DC_MotorDirectionType;
# 115 "src/../HAL/DC_Motor/dc_motor.h"
typedef enum
{
    DC_MOTOR_STATE_STOP = 0,
    DC_MOTOR_STATE_FORWARD = 1,
    DC_MOTOR_STATE_BACKWARD = 2,
    DC_MOTOR_STATE_BRAKE = 3
} DC_MotorStateType;
# 143 "src/../HAL/DC_Motor/dc_motor.h"
typedef struct
{

    uint8_h in1Port; uint8_h in1Pin;
    uint8_h in2Port; uint8_h in2Pin;
    uint8_h enPort; uint8_h enPin;
    DC_MotorPwmChannelType pwmChannel;
    uint8_h invertDirection;


    uint8_h initialized;
    uint8_h speedPercent;
    DC_MotorStateType state;
} DC_MotorHandleType;
# 174 "src/../HAL/DC_Motor/dc_motor.h"
STD_ReturnType DC_Motor_Init(DC_MotorHandleType *handle);
# 189 "src/../HAL/DC_Motor/dc_motor.h"
STD_ReturnType DC_Motor_SetSpeed(DC_MotorHandleType *handle, uint8_h speedPercent);
# 200 "src/../HAL/DC_Motor/dc_motor.h"
STD_ReturnType DC_Motor_Forward(DC_MotorHandleType *handle);






STD_ReturnType DC_Motor_Backward(DC_MotorHandleType *handle);
# 216 "src/../HAL/DC_Motor/dc_motor.h"
STD_ReturnType DC_Motor_SetDirection(DC_MotorHandleType *handle, DC_MotorDirectionType dir);
# 225 "src/../HAL/DC_Motor/dc_motor.h"
STD_ReturnType DC_Motor_Stop(DC_MotorHandleType *handle);
# 236 "src/../HAL/DC_Motor/dc_motor.h"
STD_ReturnType DC_Motor_Brake(DC_MotorHandleType *handle);
# 245 "src/../HAL/DC_Motor/dc_motor.h"
STD_ReturnType DC_Motor_GetState(const DC_MotorHandleType *handle, DC_MotorStateType *pState);







STD_ReturnType DC_Motor_GetSpeed(const DC_MotorHandleType *handle, uint8_h *pSpeed);
# 264 "src/../HAL/DC_Motor/dc_motor.h"
STD_ReturnType DC_Motor_DeInit(DC_MotorHandleType *handle);
# 6 "src/main.c" 2

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
        .baudRate = 9600UL,
        .dataSize = UART_DATA_8BITS,
        .parity = UART_PARITY_NONE,
        .stopBits = UART_STOP_1BIT
    };

    DC_MotorHandleType motorLeft = {0};
    DC_MotorHandleType motorRight = {0};

    motorLeft.in1Port = 2;
    motorLeft.in1Pin = 0;
    motorLeft.in2Port = 2;
    motorLeft.in2Pin = 1;
    motorLeft.pwmChannel = DC_MOTOR_PWM_OC1A;
    motorLeft.invertDirection = 0U;

    motorRight.in1Port = 2;
    motorRight.in1Pin = 2;
    motorRight.in2Port = 2;
    motorRight.in2Pin = 3;
    motorRight.pwmChannel = DC_MOTOR_PWM_OC1B;
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
