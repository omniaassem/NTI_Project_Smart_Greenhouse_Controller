## 1. Project Identity

| Field | Value |
| --- | --- |
| **Project code** | `PRJ-01-GREENHOUSE` |
| **Team size** | 2 students |
| **Team members** | Omnia & Roua |
| **Build window** | Days 11 – 15 (Jul 26 – Jul 30, 2026) |
| **Demo & submission** | July 30, 2026 |
| **Dominant skill** | Multi-sensor closed-loop control with hysteresis |
| **MCU** | ATmega32A @ 8 MHz |
| **Simulator** | SimulIDE 1.x |

---

## 2. Connection Diagram

### Demo media

![Smart Greenhouse Controller demo](Screenshot%202026-07-30%20222001.png)

<video controls width="100%" poster="Screenshot%202026-07-30%20222001.png">
  <source src="Screen%20Recording%202026-07-30%20220329.mp4" type="video/mp4">
  Your browser does not support the video tag.
</video>

## 3. Team Responsibilities & Work Breakdown

The project development was divided between team members based on the software architecture layers to ensure modularity and seamless integration:

 **Omnia:** Responsible for the **Application Layer (APP / Micro)** modules:
* Cooperative Scheduler (`scheduler.c / scheduler.h`)
* Finite State Machine (`greenhouse_fsm.c / greenhouse_fsm.h`)
* Hysteresis Closed-Loop Control (`control.c / control.h`)
* Command Parser & Console (`console.c / console.h`)
* Telemetry Reporting (`report.c / report.h`)


 **Roua:** Responsible for the **Hardware Abstraction Layer (HAL)** drivers:
* Sensors Driver (`sensors.c / sensors.h`)
* Actuators Driver (`actuators.c / actuators.h`)
* LCD I2C Driver (`lcd_i2c.c / lcd_i2c.h`)
* SPI EEPROM Driver (`eeprom_spi.c / eeprom_spi.h`)
* Software Debounced Buttons Driver (`buttons.c / buttons.h`)


 **Joint Work (Omnia & Roua):**
* Microcontroller Abstraction Layer (**MCAL**) drivers (`DIO`, `ADC`, `Timer`, `EXTI`, `USART`, `SPI`, `I2C`).
* System Integration, End-to-End Simulation, Testing, and Documentation.



### Development Status

| Module | Files | Status | Developer |
| --- | --- | --- | --- |
| Main firmware | `main.c` | **done** | Joint |
| MCAL drivers | `MCAL/ADC/*`, `MCAL/GPIO/*`, `MCAL/I2C/*`, `MCAL/SPI/*`, `MCAL/Timer/*`, `MCAL/UART/*`, `MCAL/Interrupt/*` | **done** | Joint |
| Hardware services | `HAL/DC_Motor/*`, `HAL/LCD_Aip31068_i2c/*`, `HAL/LCD_Hd44780/*`, `HAL/Stepper_L298P/*`, `Service/*` | **done** | Joint |
| Actuator driver | `HAL/Actuators/*` | **done** | Roua |
| Button driver | `HAL/Buttons/*` | **done** | Roua |
| EEPROM SPI driver | `HAL/EEPROM_SPI/*` | **in progress** | Roua |
| Sensor driver | `HAL/Sensors/*` | **done** | Roua |
| Micro-layer app | `Micro/*` (`scheduler`, `greenhouse_fsm`, `control`, `report`, `console`) | **done** | Omnia |

---

## 4. Description

The Smart Greenhouse Controller keeps a plant-growing enclosure inside a healthy envelope of **temperature, soil moisture and light** without human intervention.

Three analog transducers (potentiometers in SimulIDE) feed the ATmega32 ADC. A 10 ms cooperative scheduler samples them, filters the readings, and runs an independent **hysteresis controller** for each of three actuators:

| Loop | Sensor | Actuator | Purpose |
| --- | --- | --- | --- |
| Thermal | Temperature | Cooling fan | Pull the enclosure back under 35 °C |
| Irrigation | Soil moisture | Water pump | Keep the root zone between 40 % and 60 % |
| Photoperiod | Light intensity | Grow lamp | Guarantee a minimum illumination level |

An I2C character LCD shows the live picture. A UART link streams a telemetry frame every 5 s and accepts a text command console. All tunable thresholds live in an SPI EEPROM, so the greenhouse restores its personality after a power cut.

The central engineering lesson is **hysteresis**: a naive `if (temp > 35) fan=ON; else fan=OFF;` chatters the relay dozens of times a second when the temperature sits on the threshold. Every loop in this project has a separate ON point and OFF point, and you must demonstrate the difference on the LCD and in the UART log.

---

## 5. Objectives

1. Drive three ADC channels from one multiplexer without cross-talk.
2. Implement hysteresis (Schmitt-trigger) control in software and prove it eliminates actuator chatter.
3. Build a 10 ms cooperative scheduler on Timer0 CTC and run five tasks at four different periods from it.
4. Persist a configuration record to external SPI EEPROM with a magic number and checksum, and restore it safely at boot.
5. Drive a 16×2 LCD over I2C through a PCF8574 expander in 4-bit mode.
6. Design a five-state finite state machine with a documented transition table and implement it as a `switch` dispatcher.
7. Define a line-oriented UART protocol and write a parser that survives malformed input.

---

## 6. Learning Outcomes

On completion the student can:

| ID | Outcome |
| --- | --- |
| LO-1 | Configure `ADMUX`/`ADCSRA` for a 125 kHz ADC clock and switch channels safely between conversions |
| LO-2 | Explain why a single threshold causes relay chatter, and size a hysteresis band from the noise floor |
| LO-3 | Convert a 10-bit ADC code into engineering units using integer arithmetic only (no `float`) |
| LO-4 | Write a non-blocking periodic task table and justify why `_delay_ms()` is banned in the super-loop |
| LO-5 | Execute a full SPI EEPROM transaction: `WREN` → `WRITE` → poll `RDSR.WIP` → `READ`-back verify |
| LO-6 | Validate persisted data with a magic word and 8-bit checksum, and fall back to defaults on corruption |
| LO-7 | Implement and defend a finite state machine against its transition table |

---

## 7. Hardware Components

| # | Component | Qty | SimulIDE part | Purpose |
| --- | --- | --- | --- | --- |
| 1 | ATmega32A | 1 | `atmega32` | Controller |
| 2 | Potentiometer 10 kΩ | 3 | `Potentiometer` | Temperature, soil moisture, light |
| 3 | Push button (NO) | 3 | `Push` | Mode, Save, Alarm-reset |
| 4 | LED + 330 Ω | 3 | `Led` | Fan, Pump, Lamp indicators |
| 5 | LED (red) + 330 Ω | 1 | `Led` | Alarm |
| 6 | Buzzer | 1 | `Buzzer` | Audible alarm |
| 7 | 16×2 LCD + PCF8574 | 1 | `Lcd` + `I2CToParallel` | Local display |
| 8 | 25LC256 SPI EEPROM | 1 | `Memory (SPI)` | Config storage |
| 9 | Serial terminal | 1 | `SerialPort` | Telemetry + console |
| 10 | Pull-up resistors 4.7 kΩ | 2 | `Resistor` | I2C SDA/SCL |

---

## 8. Pin Map

| Signal | Pin | Port bit | Direction | Notes |
| --- | --- | --- | --- | --- |
| Temperature sensor | 40 | `PA0` / ADC0 | Analog in | 0 – 1023 → 0 – 50 °C |
| Soil moisture sensor | 39 | `PA1` / ADC1 | Analog in | 0 – 1023 → 0 – 100 % |
| Light sensor | 38 | `PA2` / ADC2 | Analog in | 0 – 1023 → 0 – 100 % |
| Fan output | 1 | `PB0` | Out | Active high |
| Pump output | 2 | `PB1` | Out | Active high |
| Lamp output | 3 | `PB2` | Out | Active high |
| Alarm LED | 4 | `PB3` | Out | Active high |
| SPI `SS` | 5 | `PB4` | Out | EEPROM chip select, active **low** |
| SPI `MOSI` | 6 | `PB5` | Out |  |
| SPI `MISO` | 7 | `PB6` | In |  |
| SPI `SCK` | 8 | `PB7` | Out |  |
| I2C `SCL` | 22 | `PC0` | Out | 4.7 kΩ pull-up |
| I2C `SDA` | 23 | `PC1` | Bidir | 4.7 kΩ pull-up |
| Buzzer | 21 | `PD7` / OC2 | Out | 2 kHz PWM in bonus |
| USART `RXD` | 14 | `PD0` | In | 9600 8N1 |
| USART `TXD` | 15 | `PD1` | Out | 9600 8N1 |
| Alarm-reset button | 16 | `PD2` / INT0 | In, pull-up | Falling edge |
| Mode button | 17 | `PD3` / INT1 | In, pull-up | Falling edge |
| Save button | 18 | `PD4` | In, pull-up | Polled + debounced |

---

## 9. Software Architecture & Module Specifications

The software is designed following a Layered Architecture pattern to ensure strict separation of concerns, high reusability, and hardware independence. Higher layers interact only with the layer directly below them, and direct register manipulation is strictly restricted to the MCAL layer.

### 9.1 Layer View

```
┌───────────────────────────────────────────────────────────────────┐
│ APP Layer (Micro/*)                                               │
│  ┌────────────┐ ┌────────────┐ ┌───────────┐ ┌─────────────────┐  │
│  │ greenhouse │ │  control   │ │  report   │ │    console      │  │
│  │    _fsm    │ │  (3 loops) │ │ (telemetry)│ │ (cmd parser)   │  │
│  └─────┬──────┘ └─────┬──────┘ └─────┬─────┘ └────────┬────────┘  │
│        └──────────────┴──── scheduler (10 ms) ────────┘           │
├───────────────────────────────────────────────────────────────────┤
│ HAL Layer                                                         │
│  sensors.c   actuators.c   lcd_i2c.c   eeprom_spi.c   buttons.c   │
├───────────────────────────────────────────────────────────────────┤
│ MCAL Layer                                                        │
│  dio.c   adc.c   timer.c   exti.c   usart.c   spi.c   i2c.c       │
├───────────────────────────────────────────────────────────────────┤
│ LIB Layer                                                         │
│  STD_TYPES.h   BIT_MATH.h   ring_buffer.c                         │
└───────────────────────────────────────────────────────────────────┘

```

### 9.2 Module Responsibilities & API Reference

| Module | Owns / Responsibilities | Key Functions & Public APIs |
|--------|─────────────────────────|----------------─────────────|
| `scheduler` | 10 ms system tick, task scheduling table, CPU overrun tracking | `SCH_Init`, `SCH_AddTask`, `SCH_Dispatch` |
| `greenhouse_fsm` | Active system mode, state transitions (AUTO, MANUAL, ALARM), alarm latching | `FSM_Init`, `FSM_Run`, `FSM_GetState` |
| `control` | Closed-loop hysteresis control for Thermal, Irrigation, and Photo loops | `CTRL_UpdateThermal`, `CTRL_UpdateIrrigation`, `CTRL_UpdatePhoto` |
| `report` | Telemetry frame formatting and periodical transmission | `RPT_SendStatus` |
| `console` | UART ASCII command line assembly and command dispatching | `CON_Init`, `CON_Poll` |
| `sensors` | Sequential ADC channels scanning, Median-of-3 filtering, and integer scaling | `SEN_Scan`, `SEN_GetTempC`, `SEN_GetSoilPct`, `SEN_GetLightPct` |
| `actuators` | Hardware-independent abstraction for Fan, Water Pump, Lamp, and Alarm LED | `ACT_Set(Actuator_t act, uint8_t state)` |
| `buttons` | 20 ms software debounce algorithm, key press edge detection | `BTN_Poll`, `BTN_WasPressed` |
| `lcd_i2c` | PCF8574 I2C protocol, non-flicker differential screen updating | `LCD_Init`, `LCD_Goto`, `LCD_Print`, `LCD_PrintNum` |
| `eeprom_spi` | SPI 25LC256 non-blocking page read/write operations, checksum validation | `EEP_Init`, `EEP_ReadBlock`, `EEP_WriteBlock` |

### 9.3 Detailed Module Functionalities

#### 1. Application Layer (APP / Micro/*)

* **Cooperative Scheduler (`scheduler.c`):**
* `SCH_Init(void)`: Configures Timer0 CTC mode to generate a precise 10 ms system tick interrupt.
* `SCH_AddTask(TaskPtr, Period, Offset)`: Registers a task with its execution period and offset to stagger execution and eliminate CPU spikes.
* `SCH_Dispatch(void)`: Main dispatcher loop called in `main()`, executing ready tasks non-blockingly.


* **Finite State Machine (`greenhouse_fsm.c`):**
* `FSM_Init(void)`: Initializes system state to `ST_INIT` and forces all actuators into safe OFF states.
* `FSM_Run(void)`: Centralized state dispatcher using a single `switch(currentState)` pass. Manages state transitions (`ST_AUTO`, `ST_MANUAL`, `ST_ALARM`, `ST_CONFIG`).
* `FSM_GetState(void)`: Returns current active state for display and telemetry updates.


* **Hysteresis Closed-Loop Control (`control.c`):**
* `CTRL_UpdateThermal(tempC)`: Drives the cooling fan with a hysteresis band ($tempOnC = 35^\circ\text{C}$, $tempOffC = 32^\circ\text{C}$) to prevent relay chatter.
* `CTRL_UpdateIrrigation(soilPct)`: Drives the water pump between $soilOnPct (40\%)$ and $soilOffPct (60\%)$, enforcing a 60-second dry-run safety timeout.
* `CTRL_UpdatePhoto(lightPct)`: Controls the grow lamp using thresholds $lightOnPct (25\%)$ and $lightOffPct (40\%)$.


* **Console & Telemetry (`console.c`, `report.c`):**
* `CON_Init(void)` / `CON_Poll(void)`: Assembles incoming UART characters non-blockingly, parses command strings (`SET`, `MODE`, `RESET`), and enforces boundary rules.
* `RPT_SendStatus(void)`: Constructs and transmits the standardized machine telemetry frame (`$GH,T=..,S=..*XX`) every 5 seconds.



#### 2. Hardware Abstraction Layer (HAL/*)

* **Sensors Driver (`HAL/Sensors/*`):**
* `SEN_Scan(void)`: Triggers sequential conversion for the 3 analog channels.
* `SEN_GetTempC()`, `SEN_GetSoilPct()`, `SEN_GetLightPct()`: Applies a Median-of-3 filter to reject noise, scaling ADC raw values into physical engineering units using fixed-point integer arithmetic only (no float operations).


* **Actuator Driver (`HAL/Actuators/*`):**
* `ACT_Set(Actuator_t act, uint8_t state)`: Controls the active states of Fan, Pump, Lamp, and Alarm LED.


* **Button Driver (`HAL/Buttons/*`):**
* `BTN_Poll(void)`: Implements a 20 ms software debounce algorithm in the scheduler context to filter physical switch contact bouncing.


* **SPI EEPROM Driver (`HAL/EEPROM_SPI/*`):**
* `EEP_ReadBlock(addr, dest, len)` / `EEP_WriteBlock(addr, src, len)`: Executes EEPROM transactions ($\text{WREN} \rightarrow \text{WRITE} \rightarrow \text{poll RDSR.WIP} \rightarrow \text{READ}$ back) across multiple 10 ms ticks without stalling the main loop. Validates data using a Magic Word (`0xA5C3`) and 8-bit checksum.


* **I2C LCD Driver (`HAL/LCD_Aip31068_i2c/*` or `HAL/LCD_Hd44780/*`):**
* `LCD_Init()`, `LCD_Print()`, `LCD_Goto()`: Interfaces a 16x2 character display through a PCF8574 I2C expander. Repaints only modified characters every 500 ms to eliminate display flicker.



#### 3. Microcontroller Abstraction Layer (MCAL/*)

* **MCAL/ADC:** Configures single 10-bit conversions with AVCC reference and 125 kHz prescaler clock. Handles channel switching safely.
* **MCAL/Timer:** Configures Timer0 CTC mode ($\text{OCR0} = 77$, 1024 prescaler) for 10 ms ticks. Configures Timer2 Fast PWM for audible buzzer frequencies.
* **MCAL/GPIO:** Handles low-level port direction and pin read/write registers.
* **MCAL/USART:** Sets up 9600 8N1 serial communication using an interrupt-driven RX Ring Buffer to ensure zero lost bytes.
* **MCAL/SPI & MCAL/I2C:** Implements Master SPI Mode 0 and hardware I2C TWI engine at 100 kHz.
* **MCAL/Interrupt:** Manages External Interrupts (INT0/INT1) configured for falling edge detection.

### 9.4 Data Flow Diagram

```
 Pots ──▶ ADC ──▶ sensors.c ──▶ Median-3 Filter ──▶ Integer Scaling ──▶ SysData
                                                                           │
                        ┌──────────────────────────────────────────────────┤
                        ▼                                                  ▼
                    control.c  ──▶ actuators.c ──▶ Relays/LEDs          report.c ──▶ USART TX
                        │
                        ▼
                 greenhouse_fsm ──▶ lcd_i2c ──▶ LCD 16x2
                        ▲
             buttons.c ─┘        console.c ◀── USART RX Ring Buffer
                                    │
                                    ▼
                              eeprom_spi ──▶ SPI 25LC256

```

### 9.5 Interactive Hardware Components

* **Audible Alarm (Buzzer — PD7 / OC2):**
* **Role:** Provides acoustic alerts during critical faults (e.g., over-temperature $> 45^\circ\text{C}$ or severe soil dryness $< 15\%$).
* **Operation:** Toggled in a pulsed pattern (100 ms ON / 900 ms OFF) using Timer2 Fast PWM (2 kHz tone) while a latched alarm is active.


* **User Control Switches:**
* **Mode Button (PD3 / INT1):** Toggles operating modes between AUTO and MANUAL via EXTI1 interrupt.
* **Alarm Reset Button (PD2 / INT0):** Clears the latched alarm state provided the fault condition is resolved. If the fault persists, it retains the alarm and responds with `ERR ACTIVE`.
* **Save Button (PD4):** Manually writes system configuration parameters to external SPI EEPROM.
* **Factory Reset Combination:** Holding Save + Alarm Reset simultaneously for 3 seconds initiates a factory reset countdown on the LCD and restores compiled-in default configurations.



### 9.6 Concurrency & Execution Rules

* **ISR Restrictions:** ISRs may only set volatile flags, push incoming bytes into the USART RX ring buffer, or increment the system tick counter. No LCD operations, EEPROM operations, or blocking UART TX transmissions are permitted inside ISR context.
* **Shared Data Integrity:** `SysData` is updated exclusively by a single task (`TASK_Sample`). Any multi-byte field read operations performed outside the tick context must be wrapped in `ATOMIC_BLOCK` or `cli()`/`sei()` pairs.
* **Non-Blocking Scheduler Execution:** The scheduler task dispatcher executes exclusively within `main()` context; the Timer0 tick ISR only updates `g_tickFlag`.

---

## 10. Data Dictionary

Everything the firmware must hold. Put the types in `APP/config.h` and `APP/types.h`.

### 10.1 Runtime data — `SysData_t`

```c
typedef struct {
    uint16_t adcRaw[3];      /* ADC0..ADC2, 0..1023, post-filter        */
    uint8_t  tempC;          /* 0..50  °C                                */
    uint8_t  soilPct;        /* 0..100 %                                 */
    uint8_t  lightPct;       /* 0..100 %                                 */
    uint8_t  fanOn   : 1;
    uint8_t  pumpOn  : 1;
    uint8_t  lampOn  : 1;
    uint8_t  alarmOn : 1;
    uint8_t  reserved: 4;
    uint8_t  mode;           /* Mode_t                                   */
    uint16_t upTimeSec;      /* seconds since boot, wraps at 65535       */
} SysData_t;

```

### 10.2 Persisted configuration — `Config_t`

```c
#define CFG_MAGIC   0xA5C3u      /* identifies a valid record            */
#define CFG_VERSION 0x01u

typedef struct {
    uint16_t magic;          /* CFG_MAGIC                                */
    uint8_t  version;        /* CFG_VERSION                              */
    uint8_t  tempOnC;        /* fan ON  above this   (default 35)        */
    uint8_t  tempOffC;       /* fan OFF below this   (default 32)        */
    uint8_t  soilOnPct;      /* pump ON  below this  (default 40)        */
    uint8_t  soilOffPct;     /* pump OFF above this  (default 60)        */
    uint8_t  lightOnPct;     /* lamp ON  below this  (default 25)        */
    uint8_t  lightOffPct;    /* lamp OFF above this  (default 40)        */
    uint8_t  tempAlarmC;     /* alarm above this     (default 45)        */
    uint8_t  soilAlarmPct;   /* alarm below this     (default 15)        */
    uint8_t  mode;           /* last mode: 0 = AUTO, 1 = MANUAL          */
    uint8_t  checksum;       /* 8-bit sum of all preceding bytes, ×(−1)  */
} Config_t;                  /* sizeof == 13 bytes (verify with sizeof!) */

```

---

## 11. System Flow & State Machine

### 11.1 Flowchart Summary

1. Power-on hardware initialization (MCAL drivers).
2. Load configuration from SPI EEPROM (Validate Magic Number & Checksum).
3. Fallback to default constants if EEPROM fails.
4. Execute non-blocking super-loop managed by the 10 ms Cooperative Scheduler.

### 11.2 State Transition Table

| # | From | Event / guard | To | Actions |
| --- | --- | --- | --- | --- |
| T1 | `ST_INIT` | Peripheral init done ∧ config loaded | `ST_AUTO` | Clear all actuators, log `EVT BOOT` |
| T2 | `ST_AUTO` | Mode button pressed | `ST_MANUAL` | Freeze actuator states, log `EVT MODE MANUAL` |
| T3 | `ST_MANUAL` | Mode button pressed | `ST_AUTO` | Resume loops, log `EVT MODE AUTO` |
| T4 | `ST_AUTO` ∨ `ST_MANUAL` | `temp > tempAlarmC` ∨ `soil < soilAlarmPct` | `ST_ALARM` | Latch alarm, remember previous state, force safe outputs |
| T5 | `ST_ALARM` | Reset button ∨ `RESET` cmd, **and** condition cleared | previous state | Clear latch, silence buzzer, log `EVT ALARM CLR` |
| T6 | `ST_ALARM` | Reset button, condition still true | `ST_ALARM` | Reply `ERR ACTIVE`, no state change |
| T7 | `ST_AUTO` | `SET` command received | `ST_CONFIG` | Snapshot config for rollback |
| T8 | `ST_CONFIG` | `SAVE` cmd ∨ Save button | `ST_AUTO` | Write EEPROM, log result |
| T9 | `ST_CONFIG` | 30 s with no command | `ST_AUTO` | Roll back to snapshot, log `EVT CFG TIMEOUT` |
| T10 | any | Save + Reset held 3 s | `ST_INIT` | Factory reset, rewrite EEPROM |
| T11 | `ST_AUTO` ∨ `ST_MANUAL` | Sensor implausible 5 s | same | Raise sensor-fault flag, force that loop safe |

---

## 12. Task Scheduling

Cooperative, non-preemptive. Timer0 CTC ISR sets `g_tick`; `main()` dispatches.

| ID | Task | Period | Offset | Budget | Work | Assigned Developer |
| --- | --- | --- | --- | --- | --- | --- |
| T-1 | `Task_Buttons` | 10 ms | 0 | 100 µs | Debounce, edge detect | Roua |
| T-2 | `Task_FSM` | 10 ms | 0 | 200 µs | One `switch` pass, timers | Omnia Assem |
| T-3 | `Task_Sample` | 100 ms | 1 | 1 ms | 3× ADC + median + scale | Roua |
| T-4 | `Task_Control` | 200 ms | 3 | 300 µs | 3 hysteresis loops | Omnia Assem |
| T-5 | `Task_LCD` | 500 ms | 5 | 4 ms | Repaint changed cells | Roua |
| T-6 | `Task_Report` | 5 s | 7 | 2 ms | Build + queue telemetry | Omnia Assem |
| T-7 | `Task_Console` | 20 ms | 2 | 500 µs | Parse one complete line | Omnia Assem |
| T-8 | `Task_EEPROM` | on event | — | 50 ms | Save / load, `WIP` polled | Roua |

---

## 13. Deliverables

1. Source code cleanly layered per §9.1.
2. `Simulation/greenhouse.sim1` working in SimulIDE 1.x.
3. Full documentation and test suite report (`Docs/test_report.md`).
4. Project demonstration video and live defense.

---

*Prepared by Omnia & Roua*
