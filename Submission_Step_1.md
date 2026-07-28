# Submission Step 1

## Project Short Description
Smart Greenhouse Controller is an embedded systems project designed to monitor and control environmental conditions inside a greenhouse automatically. The system uses sensors to read temperature, humidity, and soil moisture, then controls actuators such as fans, pumps, and lighting to maintain suitable conditions for plant growth.

The project is implemented using a layered architecture with:
- APP layer for system logic and control flow
- HAL layer for sensor and actuator drivers
- MCAL layer for low-level hardware drivers
- LIB layer for common utilities and data structures

## Project Schematic Overview
The system consists of the following main hardware components:
- Microcontroller as the main processing unit
- Temperature and humidity sensors
- Soil moisture sensor
- LCD display for local monitoring
- EEPROM connected through SPI for data storage
- Buttons for user interaction
- Actuators such as fan, pump, and LEDs
- UART/USART interface for communication and debugging

The schematic is organized around the controller unit, where the sensors provide input signals, the controller processes them, and the actuators respond accordingly.

## Project Modules
### APP Layer
- Greenhouse_FSM: manages the state machine of the greenhouse operation
- Control_Module: handles the main control decisions
- Scheduler_Module: schedules periodic tasks
- Console_Module: manages user console interaction
- Report_Module: generates and stores reports or system status information

### HAL Layer
- Sensors_Module: reads environmental sensor values
- Actuators_Module: controls output devices
- LCD_I2C_Module: interfaces with the LCD display
- EEPROM_SPI_Module: handles non-volatile storage
- Buttons_Module: reads user inputs

### MCAL Layer
- DIO_Driver: digital input/output pin control
- ADC_Driver: analog-to-digital conversion
- Timer0_Driver: timing and delay management
- EXTI_Driver: external interrupt handling
- USART_Driver: serial communication
- SPI_Driver: SPI communication
- I2C_Driver: I2C communication

### LIB Layer
- Ring_Buffer: circular buffer utility
- BIT_MATH.h: bit manipulation macros
- STD_TYPES.h: standard data types

## Developers
- Omnia
- Rouaa

## Work Distribution
### 1. First Responsibility (Omnia) - MCAL and Services Layer
- Code preparation: review and modify the core microcontroller drivers in the MCAL folder, such as ADC, GPIO, I2C, SPI, Timer, USART, and Interrupt.
- Core support: write and develop Service or LIB files such as Bit_Math.h and STD_Types.h.
- Technical integration: ensure the Registers match the ATmega32A and prepare the functions so that the upper layers can call them properly.

### 2. Second Responsibility (Rouaa) - HAL and APP Layer
- Code preparation: review and adjust HAL drivers such as LCD, Keypad, and others.
- Project logic (Logic & FSM): build the application files, FSM, hysteresis control loops, and Scheduler inside the Logic / APP folder, and connect them to the main.c file.
- Technical integration: combine sensor readings and actuator control with the display and UART system.

### 3. Shared Task (Initial Priority)
- Pin Mapping & config.h: before continuing with the code or drawing the schematic, both members should agree on a single configuration file (config.h) to define the pin numbers.
- For example: ADC pins for sensors on PA0, PA1, PA2, and I2C pins for LCD on PC0, PC1, and so on.
- Once these connections are fixed, each person can work on their own part in a consistent and coordinated way.
