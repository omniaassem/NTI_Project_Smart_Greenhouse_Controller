// /**************************************************************************/
// /* File Name: main.c                                                      */
// /* Description: Smart Greenhouse - Integrated Main & Test Code Options    */
// /**************************************************************************/

// #include "../../Service/STD_Types.h"
// #include "../../Service/Bit_Math.h"
// #include "../../Service/Ring_Buffer/ring_buffer.h"

// /* MCAL Headers */
// #include "../../MCAL/GPIO/gpio_interface.h"
// #include "../../MCAL/I2C/i2c_interface.h"
// #include "../../MCAL/UART/uart_interface.h"
// #include "../../MCAL/Timer/timer_interface.h"
// #include "../../MCAL/ADC/adc_interface.h"

// /* Configuration Header */
// #include "../src/config.h"

// /* Scheduler & Micro Layers */
// #include "../Micro/Scheduler_module/scheduler.h"
// #include "../Micro/Console_module/console.h"
// #include "../Micro/Control_module/control.h"
// #include "../Micro/Greenhouse_FSM/greenhouse_fsm.h"
// #include "../Micro/Report_module/report.h"

// /* HAL Headers */
// #include "../Actuators/Actuators_Driver.h"
// #include "../Buttons/Buttons_Driver.h"
// #include "../DC_Motor/dc_motor.h"
// #include "../Sensors/Sensors_Driver.h"
// #include "../LCD_Aip31068_i2c/lcd_aip31068_i2c.h"


// /**************************************************************************/
// /* ====================================================================== */
// /* OPTION 1: MAIN PROJECT CODE (Active by default)                        */
// /* ====================================================================== */
// /**************************************************************************/

// static void Task_ButtonPoll(void)
// {
//     (void)BTN_Poll();

//     u8 modePressed = 0u;
//     if ((BTN_WasPressed(BTN_MODE, &modePressed) == E_OK) && (modePressed != 0u))
//     {
//         ActuatorStateType fanState;
//         (void)ACT_Get(ACTUATOR_FAN, &fanState);
        
//         if (fanState == ACT_STATE_OFF)
//         {
//             (void)ACT_Set(ACTUATOR_FAN, ACT_STATE_ON);
//         }
//         else
//         {
//             (void)ACT_Set(ACTUATOR_FAN, ACT_STATE_OFF);
//         }
//     }
// }

// static void Task_ConsoleProcess(void)
// {
//     (void)CON_Process();
// }

// static void Task_FsmRun(void)
// {
//     (void)FSM_Run();
// }

// static void Task_ControlUpdate(void)
// {
//     (void)CTRL_Update();
// }

// static void Task_SensorsAndDisplay(void)
// {
//     u16 tempRaw = 0u, soilRaw = 0u, lightRaw = 0u;
//     u8 tempC = 0u, soilPct = 0u, lightPct = 0u;

//     if (Sensors_ReadRaw(&tempRaw, &soilRaw, &lightRaw) == E_OK)
//     {
//         (void)Sensors_ScaleTempC(tempRaw, &tempC);
//         (void)Sensors_ScalePct(soilRaw, &soilPct);
//         (void)Sensors_ScalePct(lightRaw, &lightPct);

//         (void)RPT_SendStatus();
//     }
// }

// static void Delay1Ms(void)
// {
//     volatile u16 inner;
//     for (inner = 0u; inner < 6000u; ++inner)
//     {
//         __asm__ __volatile__ ("nop");
//     }
// }

// int main(void)
// {
//     /* 1. Initialize Communication Buses */
//     I2C_MasterConfigType i2cCfg = { I2C_SCL_100KHZ };
//     I2C_InitMaster(&i2cCfg);

//     /* 2. Initialize Scheduler and Micro Layers */
//     (void)SCH_Init();
//     (void)CON_Init();
//     (void)CTRL_Init();
//     (void)FSM_Init();
//     (void)RPT_Init();

//     /* 3. Initialize Hardware Drivers */
//     (void)ACT_Init();
//     (void)BTN_Init();
//     (void)Sensors_Init();

//     /* 4. Register Scheduled Tasks (Max 5 tasks) */
//     (void)SCH_AddTask(Task_ButtonPoll,       10u);  /* Every 10ms  */
//     (void)SCH_AddTask(Task_ConsoleProcess,   50u);  /* Every 50ms  */
//     (void)SCH_AddTask(Task_FsmRun,           50u);  /* Every 50ms  */
//     (void)SCH_AddTask(Task_ControlUpdate,   100u);  /* Every 100ms */
//     (void)SCH_AddTask(Task_SensorsAndDisplay, 500u); /* Every 500ms */

//     /* 5. Initialize AiP31068 I2C Character LCD (2x16) */
//     LCD_Aip31068_HandleType greenhouseLcd;
//     greenhouseLcd.i2cAddress = LCD_AIP31068_DEFAULT_ADDRESS; /* 0x3E */
//     greenhouseLcd.rows       = 2u;
//     greenhouseLcd.cols       = 16u;

//     (void)LCD_Aip31068_Init(&greenhouseLcd);
//     (void)LCD_Aip31068_WriteStringAt(&greenhouseLcd, 0u, 0u, (const u8 *)"Smart Greenhouse");
//     (void)LCD_Aip31068_WriteStringAt(&greenhouseLcd, 1u, 0u, (const u8 *)"System Online   ");
    
//     for (u8 d = 0u; d < 100u; d++) { Delay1Ms(); }
//     (void)LCD_Aip31068_Clear(&greenhouseLcd);

//     /* 6. Configure and Initialize DC Motors (Pump / Fan using H-Bridge) */
//     DC_MotorHandleType greenhousePump = {0};
//     greenhousePump.in1Port         = GPIO_PORTC;
//     greenhousePump.in1Pin          = GPIO_PIN0;
//     greenhousePump.in2Port         = GPIO_PORTC;
//     greenhousePump.in2Pin          = GPIO_PIN1;
//     greenhousePump.pwmChannel      = DC_MOTOR_PWM_OC1A;
//     greenhousePump.invertDirection = 0u;

//     (void)DC_Motor_Init(&greenhousePump);
//     (void)DC_Motor_SetSpeed(&greenhousePump, 75u);

//     /* 7. Main Super-Loop */
//     while (1)
//     {
//         SCH_Dispatch();
//         Delay1Ms();
//         SCH_Tick();
//     }

//     return 0;
// }


/**************************************************************************/
/* ====================================================================== */
/* OPTION 2: COMPREHENSIVE TEST CODE (Commented out as a whole)           */
/* To use this: Un-comment this block and comment out the Option 1 main() */
/* ====================================================================== */
/**************************************************************************/

static void Test_SendLine(const char *message)
{
    (void)UART_SendString((const u8 *)message);
    (void)UART_SendString((const u8 *)"\r\n");
}

static void Test_UART_PrintUnsigned(u16 value)
{
    char buffer[6] = {'0', '0', '0', '0', '0', '\0'};
    u8 index = 0u;

    if (value == 0u)
    {
        (void)UART_SendString((const u8 *)"0");
        return;
    }

    while ((value > 0u) && (index < 5u))
    {
        buffer[index++] = (char)('0' + (value % 10u));
        value /= 10u;
    }

    while (index > 0u)
    {
        index--;
        (void)UART_SendByte((u8)buffer[index]);
    }
}

static void Test_UART_PrintLine(const char *label, u16 value)
{
    (void)UART_SendString((const u8 *)label);
    Test_UART_PrintUnsigned(value);
    (void)UART_SendString((const u8 *)"\r\n");
}

static void Test_SensorTask(void)
{
    u16 rawTemp  = 0u;
    u16 rawSoil  = 0u;
    u16 rawLight = 0u;
    u8  tempC    = 0u;
    u8  soilPct  = 0u;
    u8  lightPct = 0u;

    if (Sensors_ReadRaw(&rawTemp, &rawSoil, &rawLight) == E_OK)
    {
        (void)Sensors_ScaleTempC(rawTemp, &tempC);
        (void)Sensors_ScalePct(rawSoil, &soilPct);
        (void)Sensors_ScalePct(lightLight, &lightPct); // or lightRaw

        Test_UART_PrintLine("Raw Temp: ", rawTemp);
        Test_UART_PrintLine("Temp C:   ", tempC);
        Test_UART_PrintLine("Soil %:   ", soilPct);
        Test_UART_PrintLine("Light %:  ", lightPct);
        Test_SendLine("------");
    }
    else
    {
        Test_SendLine("Sensor read failed");
    }
}

int test_main(void)
{
    // Step 1: Configure UART and Timer for Testing
    UART_ConfigType uartConfig =
    {
        .baudRate = UART_BAUD_9600,
        .dataSize = UART_DATA_8BITS,
        .parity   = UART_PARITY_NONE,
        .stopBits = UART_STOP_1BIT
    };

    Timer_ConfigType timerConfig =
    {
        .channel      = TIMER_CHANNEL_0,
        .mode         = TIMER_MODE_CTC,
        .prescaler    = TIMER_CLOCK_DIV_1024,
        .initialValue = 0u,
        .compareValue = 155u
    };

    (void)UART_Init(&uartConfig);
    (void)SCH_Init();

    // Step 2: Initialize Sensors and Timer Interrupts
    if (Sensors_Init() != E_OK)
    {
        Test_SendLine("Sensor init failed");
        while (1);
    }

    if (Timer_Init(&timerConfig) != E_OK)
    {
        Test_SendLine("Timer init failed");
        while (1);
    }

    (void)Timer_SetCallBack(TIMER_CHANNEL_0, TIMER_INT_COMPARE_MATCH, SCH_Tick);
    (void)Timer_EnableInterrupt(TIMER_CHANNEL_0, TIMER_INT_COMPARE_MATCH);
    Timer_EnableGlobalInterrupt();

    // Step 3: Add Sensor Task to Scheduler
    if (SCH_AddTask(Test_SensorTask, 1000u) != E_OK)
    {
        Test_SendLine("Scheduler task add failed");
        while (1);
    }

    Test_SendLine("Comprehensive Test Mode Running...");

    // Step 4: Test Loop Dispatching
    while (1)
    {
        SCH_Dispatch();
    }

    return 0;
}
