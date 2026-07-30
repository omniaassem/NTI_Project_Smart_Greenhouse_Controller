/**
 * @file main.c
 * @brief Complete and final integration firmware for Project-2
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

/* Application and Service Headers */
#include "config.h"
#include "STD_Types.h"

/* MCAL Layer */
#include "timer_interface.h"
#include "spi_interface.h"
#include "interrupt_interface.h"
#include "adc_interface.h"
#include "i2c_interface.h"
#include "uart_interface.h"

/* HAL Layer */
#include "Actuators_Driver.h"
#include "Buttons_Driver.h"
#include "dc_motor.h"
#include "lcd_aip31068_i2c.h"
#include "MotorBridge.h"
#include "Sensors_Driver.h"

/* Micro Layer */
#include "greenhouse_fsm.h"
#include "control.h"
#include "report.h"
#include "console.h"

/* --- Task Implementations --- */
static void Task_Buttons(void) {
    BTN_Poll();
}

static void Task_FSM(void) {
    FSM_Run();
}

static void Task_Sample(void) {
}

static void Task_Control(void) {
    CTRL_Update();
}

static void Task_LCD(void) {
}

static void Task_Report(void) {
    RPT_SendStatus();
}

static void Task_Console(void) {
    CON_Process(); /* تم التصحيح لتطابق CON_Process المعرفة في console.h */
}

int main(void) {
    /* 1. Initialize MCAL Peripherals */
    ADC_Init(NULL);
    Timer_Init(NULL);
    EXTI_Init(NULL);
    UART_Init(NULL);
    SPI_Init(NULL);

    /* 2. Initialize Application Layers */
    FSM_Init();
    ACT_Init();

    /* Enable Global Interrupts */
    sei();

    uint16_t tickCounter = 0;

    /* Super-Loop Dispatcher */
    while (1) {
        tickCounter++;

        Task_Buttons();
        Task_FSM();

        if ((tickCounter % 2) == 0) {
            Task_Console();
        }

        if ((tickCounter % 10) == 1) {      
            Task_Sample();
        }
        if ((tickCounter % 20) == 3) {      
            Task_Control();
        }
        if ((tickCounter % 50) == 5) {      
            Task_LCD();
        }
        if ((tickCounter % 500) == 7) {     
            Task_Report();
        }
    }

    return 0;
}