# Project 01 — Smart Greenhouse Controller

> Part of the **Embedded Systems Projects Book** — see the
> [book README](../README.md) for the shared platform baseline, layer rules and
> common rubric. Everything in this file is *in addition to* those rules.

---

## 1. Project Identity

| Field | Value |
|-------|-------|
| **Project code** | `PRJ-01-GREENHOUSE` |
| **Team size** | 2 – 3 students |
| **Build window** | Days 11 – 15 (Jul 26 – Jul 30, 2026) |
| **Demo & submission** | July 30, 2026 |
| **Dominant skill** | Multi-sensor closed-loop control with hysteresis |
| **MCU** | ATmega32A @ 8 MHz |
| **Simulator** | SimulIDE 1.x |

---

## 2. Connection Diagram

![Smart Greenhouse Connection Diagram](photo_2026-07-29_19-24-53.jpg)

## Development Status

| Module | Files | Status |
|--------|-------|--------|
| Main firmware | `main.c` | **done** |
| MCAL drivers | `MCAL/ADC/*`, `MCAL/GPIO/*`, `MCAL/I2C/*`, `MCAL/SPI/*`, `MCAL/Timer/*`, `MCAL/UART/*`, `MCAL/Interrupt/*` | **done** |
| Hardware services | `HAL/DC_Motor/*`, `HAL/LCD_Aip31068_i2c/*`, `HAL/LCD_Hd44780/*`, `HAL/Stepper_L298P/*`, `Service/*` | **done** |
| Actuator driver | `HAL/Actuators/*` | **done** |
| Button driver | `HAL/Buttons/*` | **done** |
| EEPROM SPI driver | `HAL/EEPROM_SPI/*` | **in progress** |
| Sensor driver | `HAL/Sensors/*` | **in progress** |
| Micro-layer app | `Micro/*` | **done** |

## 3. Description

The Smart Greenhouse Controller keeps a plant-growing enclosure inside a healthy
envelope of **temperature, soil moisture and light** without human intervention.

Three analog transducers (potentiometers in SimulIDE) feed the ATmega32 ADC. A
10 ms cooperative scheduler samples them, filters the readings, and runs an
independent **hysteresis controller** for each of three actuators:

| Loop | Sensor | Actuator | Purpose |
|------|--------|----------|---------|
| Thermal | Temperature | Cooling fan | Pull the enclosure back under 35 °C |
| Irrigation | Soil moisture | Water pump | Keep the root zone between 40 % and 60 % |
| Photoperiod | Light intensity | Grow lamp | Guarantee a minimum illumination level |

An I2C character LCD shows the live picture. A UART link streams a telemetry
frame every 5 s and accepts a text command console. All tunable thresholds live
in an SPI EEPROM, so the greenhouse restores its personality after a power cut.

The central engineering lesson is **hysteresis**: a naive `if (temp > 35) fan=ON;
else fan=OFF;` chatters the relay dozens of times a second when the temperature
sits on the threshold. Every loop in this project has a separate ON point and OFF
point, and you must demonstrate the difference on the LCD and in the UART log.

---

## 3. Objectives

1. Drive three ADC channels from one multiplexer without cross-talk.
2. Implement hysteresis (Schmitt-trigger) control in software and prove it
   eliminates actuator chatter.
3. Build a 10 ms cooperative scheduler on Timer0 CTC and run five tasks at four
   different periods from it.
4. Persist a configuration record to external SPI EEPROM with a magic number and
   checksum, and restore it safely at boot.
5. Drive a 16×2 LCD over I2C through a PCF8574 expander in 4-bit mode.
6. Design a five-state finite state machine with a documented transition table
   and implement it as a `switch` dispatcher.
7. Define a line-oriented UART protocol and write a parser that survives
   malformed input.

---

## 4. Learning Outcomes

On completion the student can:

| ID | Outcome |
|----|---------|
| LO-1 | Configure `ADMUX`/`ADCSRA` for a 125 kHz ADC clock and switch channels safely between conversions |
| LO-2 | Explain why a single threshold causes relay chatter, and size a hysteresis band from the noise floor |
| LO-3 | Convert a 10-bit ADC code into engineering units using integer arithmetic only (no `float`) |
| LO-4 | Write a non-blocking periodic task table and justify why `_delay_ms()` is banned in the super-loop |
| LO-5 | Execute a full SPI EEPROM transaction: `WREN` → `WRITE` → poll `RDSR.WIP` → `READ`-back verify |
| LO-6 | Validate persisted data with a magic word and 8-bit checksum, and fall back to defaults on corruption |
| LO-7 | Implement and defend a finite state machine against its transition table |

---

## 5. Estimated Duration

| Phase | Hours | Course day |
|-------|:-----:|-----------|
| Requirements analysis & pin freeze | 3 | Day 11 |
| Architecture & state machine design | 4 | Day 11 |
| GPIO + ADC bring-up | 6 | Day 12 |
| Timer, scheduler, control loops | 6 | Day 13 |
| LCD, EEPROM, UART integration | 7 | Day 14 |
| Testing & debugging | 4 | Day 15 |
| Documentation, report, video | 4 | Day 15 + evening |
| **Total** | **34 h** | |

---

## 6. Hardware Components

| # | Component | Qty | SimulIDE part | Purpose |
|---|-----------|:---:|---------------|---------|
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

## 7. Pin Map

| Signal | Pin | Port bit | Direction | Notes |
|--------|-----|----------|-----------|-------|
| Temperature sensor | 40 | `PA0` / ADC0 | Analog in | 0 – 1023 → 0 – 50 °C |
| Soil moisture sensor | 39 | `PA1` / ADC1 | Analog in | 0 – 1023 → 0 – 100 % |
| Light sensor | 38 | `PA2` / ADC2 | Analog in | 0 – 1023 → 0 – 100 % |
| Fan output | 1 | `PB0` | Out | Active high |
| Pump output | 2 | `PB1` | Out | Active high |
| Lamp output | 3 | `PB2` | Out | Active high |
| Alarm LED | 4 | `PB3` | Out | Active high |
| SPI `SS` | 5 | `PB4` | Out | EEPROM chip select, active **low** |
| SPI `MOSI` | 6 | `PB5` | Out | |
| SPI `MISO` | 7 | `PB6` | In | |
| SPI `SCK` | 8 | `PB7` | Out | |
| I2C `SCL` | 22 | `PC0` | Out | 4.7 kΩ pull-up |
| I2C `SDA` | 23 | `PC1` | Bidir | 4.7 kΩ pull-up |
| Buzzer | 21 | `PD7` / OC2 | Out | 2 kHz PWM in bonus |
| USART `RXD` | 14 | `PD0` | In | 9600 8N1 |
| USART `TXD` | 15 | `PD1` | Out | 9600 8N1 |
| Alarm-reset button | 16 | `PD2` / INT0 | In, pull-up | Falling edge |
| Mode button | 17 | `PD3` / INT1 | In, pull-up | Falling edge |
| Save button | 18 | `PD4` | In, pull-up | Polled + debounced |

**Debounce rule:** every button — interrupt-driven or polled — must be debounced
in software for **20 ms**. The ISR sets a flag; the debounce timer runs in the
scheduler, never in the ISR.

---

## 8. Peripherals Used

| Peripheral | Configuration | Role |
|------------|---------------|------|
| **GPIO** | `PB0..PB3` out, `PD2/PD3/PD4` in with pull-up | Actuators & buttons |
| **ADC** | Free-running off; single conversion, prescaler 64, AVCC ref, right adjust | 3 sensor channels |
| **Timer0** | CTC, prescaler 1024, `OCR0 = 77`, `OCIE0` on | 10 ms system tick |
| **Timer2** | Fast PWM (bonus) | Buzzer tone |
| **INT0 / INT1** | Falling edge (`ISC01=1`, `ISC11=1`) | Alarm reset, mode change |
| **USART** | 9600 8N1, RX interrupt on, TX polled | Telemetry + console |
| **SPI** | Master, Mode 0, f<sub>osc</sub>/16 | 25LC256 EEPROM |
| **I2C (TWI)** | Master, 100 kHz, `TWBR = 32` | PCF8574 → LCD |

---

## 9. Software Architecture

### 9.1 Layer view

```
┌───────────────────────────────────────────────────────────────────┐
│ APP                                                               │
│  ┌────────────┐ ┌────────────┐ ┌───────────┐ ┌─────────────────┐  │
│  │ greenhouse │ │  control   │ │  report   │ │    console      │  │
│  │    _fsm    │ │  (3 loops) │ │ (telemetry)│ │ (cmd parser)   │  │
│  └─────┬──────┘ └─────┬──────┘ └─────┬─────┘ └────────┬────────┘  │
│        └──────────────┴──── scheduler (10 ms) ────────┘           │
├───────────────────────────────────────────────────────────────────┤
│ HAL                                                               │
│  sensors.c   actuators.c   lcd_i2c.c   eeprom_spi.c   buttons.c   │
├───────────────────────────────────────────────────────────────────┤
│ MCAL                                                              │
│  dio.c   adc.c   timer.c   exti.c   usart.c   spi.c   i2c.c       │
├───────────────────────────────────────────────────────────────────┤
│ LIB    STD_TYPES.h   BIT_MATH.h   ring_buffer.c                   │
└───────────────────────────────────────────────────────────────────┘
```

### 9.2 Module responsibilities

| Module | Owns | Public API (suggested) |
|--------|------|------------------------|
| `scheduler` | The 10 ms tick, task table, overrun counter | `SCH_Init`, `SCH_AddTask`, `SCH_Dispatch` |
| `greenhouse_fsm` | Current mode, transitions, alarm latch | `FSM_Init`, `FSM_Run`, `FSM_GetState` |
| `control` | Three hysteresis loops | `CTRL_UpdateThermal`, `CTRL_UpdateIrrigation`, `CTRL_UpdatePhoto` |
| `report` | Telemetry frame formatting | `RPT_SendStatus` |
| `console` | UART line assembly + command dispatch | `CON_Init`, `CON_Poll` |
| `sensors` | ADC channel scan + median filter + scaling | `SEN_Scan`, `SEN_GetTempC`, `SEN_GetSoilPct`, `SEN_GetLightPct` |
| `actuators` | Fan/pump/lamp/alarm abstraction | `ACT_Set(actuator, state)` |
| `buttons` | 20 ms debounce, edge flags | `BTN_Poll`, `BTN_WasPressed` |
| `lcd_i2c` | PCF8574 nibble protocol, screen painting | `LCD_Init`, `LCD_Goto`, `LCD_Print`, `LCD_PrintNum` |
| `eeprom_spi` | 25LC256 transactions, checksum | `EEP_Init`, `EEP_ReadBlock`, `EEP_WriteBlock` |

### 9.3 Data flow

```
 pots ──▶ ADC ──▶ sensors.c ──▶ median-3 ──▶ scaling ──▶ SysData
                                                            │
                        ┌───────────────────────────────────┤
                        ▼                                   ▼
                    control.c  ──▶ actuators.c ──▶ LEDs   report.c ──▶ USART
                        │
                        ▼
                 greenhouse_fsm ──▶ lcd_i2c ──▶ LCD
                        ▲
             buttons.c ─┘        console.c ◀── USART RX ring buffer
                                    │
                                    ▼
                              eeprom_spi ──▶ 25LC256
```

### 9.4 Concurrency contract

- ISRs may **only** set `volatile` flags, push bytes into the RX ring buffer, or
  increment the tick counter. No LCD, no EEPROM, no UART TX inside an ISR.
- `SysData` is written by one task (`TASK_Sample`) and read by all others. Reads
  of multi-byte fields from outside the tick context must be wrapped in
  `ATOMIC_BLOCK` or a cli/sei pair.
- The scheduler dispatcher runs in `main()`; the tick ISR only sets
  `g_tickFlag`.

---

## 10. Data Dictionary (required data)

Everything the firmware must hold. Put the types in `APP/config.h` and
`APP/types.h`.

### 10.1 Runtime data — `DD-01 SysData_t`

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

### 10.2 Persisted configuration — `DD-02 Config_t`

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

### 10.3 Enumerations — `DD-03`

```c
typedef enum { ST_INIT = 0, ST_AUTO, ST_MANUAL, ST_ALARM, ST_CONFIG } State_t;
typedef enum { ACT_FAN = 0, ACT_PUMP, ACT_LAMP, ACT_ALARM }           Actuator_t;
typedef enum { SEN_TEMP = 0, SEN_SOIL, SEN_LIGHT }                    Sensor_t;
```

### 10.4 Derived constants — `DD-04`

| Constant | Value | Derivation |
|----------|-------|-----------|
| `TEMP_SCALE` | `(raw * 50) / 1023` | 0 – 1023 → 0 – 50 °C |
| `PCT_SCALE` | `(raw * 100) / 1023` | 0 – 1023 → 0 – 100 % |
| `TICK_MS` | 10 | Timer0 CTC period |
| `DEBOUNCE_TICKS` | 2 | 20 ms |
| `HOLD_RESET_TICKS` | 300 | 3 s factory reset hold |
| `MEDIAN_WINDOW` | 3 | Samples per channel before filtering |

> **Integer-only rule (NFR-05).** `(raw * 50)` peaks at 51 150 which overflows
> `uint16_t`… no it does not (max 65 535), but `(raw * 100)` peaks at 102 300 and
> **does**. Use `uint32_t` for the intermediate product or rearrange the maths.
> This is a deliberate trap — catch it in code review.

---

## 11. System Specifications

### 11.1 Temperature bands

| Band | Range | Behaviour |
|------|-------|-----------|
| Normal | 20 – 35 °C | Fan off |
| High | 36 – 45 °C | Fan on |
| Critical | 46 – 50 °C | Fan on + alarm |
| Under-range | < 20 °C | Fan off, "COLD" hint on LCD |

### 11.2 Soil moisture bands

| Band | Range | Behaviour |
|------|-------|-----------|
| Critical dry | 0 – 14 % | Pump on + alarm |
| Dry | 15 – 39 % | Pump on |
| Normal | 40 – 80 % | Pump off |
| Wet | 81 – 100 % | Pump off, "WET" hint |

### 11.3 Light bands

| Band | Range | Behaviour |
|------|-------|-----------|
| Low | 0 – 25 % | Lamp on |
| Normal | 26 – 70 % | Lamp state held (hysteresis zone) |
| High | 71 – 100 % | Lamp off |

### 11.4 Hysteresis summary

| Loop | ON threshold | OFF threshold | Band |
|------|:------------:|:-------------:|:----:|
| Fan | > 35 °C | < 32 °C | 3 °C |
| Pump | < 40 % | > 60 % | 20 % |
| Lamp | < 25 % | > 40 % | 15 % |

---

## 12. Inputs & Outputs

### 12.1 Inputs

| ID | Name | Channel | Type | Range | Sample rate |
|----|------|---------|------|-------|-------------|
| IN-1 | Temperature | ADC0 | Analog | 0 – 50 °C | 10 Hz |
| IN-2 | Soil moisture | ADC1 | Analog | 0 – 100 % | 10 Hz |
| IN-3 | Light intensity | ADC2 | Analog | 0 – 100 % | 10 Hz |
| IN-4 | Alarm reset | `PD2`/INT0 | Digital, edge | Pressed / released | Interrupt |
| IN-5 | Mode toggle | `PD3`/INT1 | Digital, edge | Pressed / released | Interrupt |
| IN-6 | Save | `PD4` | Digital, polled | Pressed / released | 100 Hz |
| IN-7 | Console commands | USART RX | ASCII line | ≤ 24 chars + `\n` | Interrupt |

### 12.2 Outputs

| ID | Name | Pin | Type | Meaning |
|----|------|-----|------|---------|
| OUT-1 | Fan | `PB0` | Digital | LED on = cooling |
| OUT-2 | Pump | `PB1` | Digital | LED on = irrigating |
| OUT-3 | Grow lamp | `PB2` | Digital | LED on = lighting |
| OUT-4 | Alarm LED | `PB3` | Digital | Steady = alarm latched |
| OUT-5 | Buzzer | `PD7` | Digital / PWM | 1 Hz beep while alarm active |
| OUT-6 | LCD | I2C | 16×2 text | Live status |
| OUT-7 | Telemetry | USART TX | ASCII frame | Every 5 s |

---

## 13. Functional Requirements

### FR-01 — Temperature acquisition

The system **shall** sample ADC0 every **100 ms** and publish a filtered
temperature in whole degrees Celsius.

**Acceptance criteria**
- ADC conversion completes in ≤ 120 µs (prescaler 64 → 104 µs measured).
- Resolution 1 °C; valid range 0 – 50 °C.
- A **median-of-3** filter is applied before scaling.
- The published value never changes by more than 5 °C between consecutive
  samples unless the input actually stepped (proves the filter is active).

### FR-02 — Soil moisture acquisition

The system **shall** sample ADC1 every **100 ms** and publish soil moisture as
0 – 100 %, median-of-3 filtered, resolution 1 %.

### FR-03 — Light acquisition

The system **shall** sample ADC2 every **100 ms** and publish light intensity as
0 – 100 %, median-of-3 filtered, resolution 1 %.

**Acceptance criteria (FR-01…03 combined)**
- The three channels are read in one scan; `ADMUX` is changed only while
  `ADSC == 0`.
- Total scan time ≤ 1 ms, so the sampling task never overruns its 100 ms slot.

### FR-04 — Cooling fan control (hysteresis)

In `AUTO` mode the system **shall** drive the fan as:

```
if (tempC > cfg.tempOnC)   → fan ON        (default 35 °C)
if (tempC < cfg.tempOffC)  → fan OFF       (default 32 °C)
otherwise                  → hold previous state
```

**Acceptance criteria**
- Sweeping the pot slowly through 32 → 35 → 32 °C produces exactly **one** ON
  edge and **one** OFF edge.
- Holding the input at exactly 33 °C for 30 s produces **zero** state changes.
- `tempOnC − tempOffC ≥ 2` is enforced; a console attempt to violate it is
  rejected with `ERR RANGE`.

### FR-05 — Water pump control (hysteresis)

In `AUTO` mode: pump **ON** when `soilPct < cfg.soilOnPct` (default 40 %),
**OFF** when `soilPct > cfg.soilOffPct` (default 60 %), hold otherwise.

**Acceptance criteria**
- One ON edge and one OFF edge per slow sweep across the band.
- The pump **shall not** run for more than **60 s** continuously; on expiry it
  is forced off and `PUMP_TIMEOUT` is logged (dry-run protection).

### FR-06 — Grow lamp control (hysteresis)

In `AUTO` mode: lamp **ON** when `lightPct < cfg.lightOnPct` (default 25 %),
**OFF** when `lightPct > cfg.lightOffPct` (default 40 %), hold otherwise.

### FR-07 — Alarm generation and latching

The alarm **shall** activate when either condition is true:

```
tempC   > cfg.tempAlarmC    (default 45 °C)
soilPct < cfg.soilAlarmPct  (default 15 %)
```

**Acceptance criteria**
- The alarm is **latched**: it stays active after the condition clears.
- Alarm LED steady on; buzzer beeps 100 ms on / 900 ms off.
- The latch clears only via the reset button (IN-4) or the `RESET` command, and
  only if the triggering condition is no longer true. Attempting to clear while
  still faulted leaves the alarm on and prints `ERR ACTIVE`.
- Entering `ALARM` forces fan and pump to the safe state for the active fault
  (fan ON for over-temperature, pump ON for critical dryness).

### FR-08 — LCD display

The LCD **shall** refresh every **500 ms** with this layout:

```
Line 1: T:31C S:55% L:68%
Line 2: F:1 P:0 L:1 AUTO
```

**Acceptance criteria**
- Only changed characters are rewritten (no full clear each refresh) — a full
  `LCD_Clear()` every cycle produces visible flicker and loses this mark.
- In `ALARM` state line 2 is replaced by `** ALARM: <cause> **` alternating with
  the normal line every 1 s.
- In `CONFIG` state the LCD shows the parameter being edited and its value.

### FR-09 — UART telemetry

Every **5 s** the system **shall** transmit one telemetry frame at 9600 8N1 (see
§18.1 for the exact format).

**Acceptance criteria**
- Frame is emitted within ±100 ms of the 5 s boundary.
- Transmission is non-blocking: the 5 s task must not stall the 10 ms tick.
  (Either a TX ring buffer with `UDRE` interrupt, or ≤ 60 chars of polled TX
  spread over consecutive ticks.)
- Zero framing errors over a 5-minute capture.

### FR-10 — Save configuration

Pressing **Save** (IN-6) **shall** write the current `Config_t` to SPI EEPROM at
address `0x0000`.

**Acceptance criteria**
- Sequence is `WREN (0x06)` → `WRITE (0x02) + addrH + addrL + data…` → poll
  `RDSR (0x05)` until `WIP == 0`.
- Checksum is recomputed before the write.
- After writing, the record is read back and compared byte-for-byte.
- LCD shows `SAVED OK` or `SAVE FAIL` for 1 s; UART logs `EVT SAVE OK` /
  `EVT SAVE FAIL`.
- Total save operation ≤ 50 ms and must not block the tick.

### FR-11 — Restore configuration at boot

At startup the system **shall** read `Config_t` from EEPROM `0x0000` and adopt it
only if `magic == CFG_MAGIC`, `version == CFG_VERSION` and the checksum is
correct.

**Acceptance criteria**
- On any validation failure the firmware loads the compiled-in defaults, writes
  them back to EEPROM, and logs `EVT CFG DEFAULT`.
- Boot-to-first-LCD-frame ≤ 500 ms.
- Power-cycling the simulation preserves a previously saved threshold.

### FR-12 — Manual mode

Pressing **Mode** (IN-5) **shall** toggle between `AUTO` and `MANUAL`.

**Acceptance criteria**
- In `MANUAL` all three automatic loops are suspended; actuator states are frozen
  at their values on entry.
- In `MANUAL` the console commands `FAN ON|OFF`, `PUMP ON|OFF`, `LAMP ON|OFF`
  take effect; in `AUTO` they are rejected with `ERR MODE`.
- The alarm check (FR-07) remains active in `MANUAL` — safety is never disabled
  by an operating mode.
- The active mode is shown on LCD line 2 and in the telemetry frame.

### FR-13 — Factory reset

Holding **Save** and **Alarm-reset** together for **3 s** **shall** restore
compiled-in defaults, write them to EEPROM, and reboot the state machine.

**Acceptance criteria**
- The 3 s window is measured by the scheduler (300 ticks), not `_delay_ms`.
- LCD counts down `RESET IN 3 / 2 / 1`; releasing either button aborts.
- On completion: `EVT FACTORY RESET` on UART, all actuators off, state `ST_INIT`.

### FR-14 — Console command set

The system **shall** accept the commands in §18.2 over UART.

**Acceptance criteria**
- Commands are case-insensitive, terminated by `\r`, `\n` or `\r\n`.
- Lines longer than 24 characters are discarded with `ERR LONG` — no buffer
  overrun, ever.
- An unknown verb returns `ERR CMD`; a bad argument returns `ERR ARG`.
- A `SET` that violates a hysteresis or range rule returns `ERR RANGE` and
  changes nothing.
- Every command produces exactly one response line.

### FR-15 — Pump run-time accounting

The system **shall** accumulate total pump run-time in seconds and report it in
the `STATUS` response.

**Acceptance criteria**
- Counter increments only while the pump output is high.
- Counter saturates at 65 535 s rather than wrapping.
- Reported as `PUMPSEC=<n>`.

---

## 14. Non-Functional Requirements

| ID | Requirement |
|----|-------------|
| **NFR-01** | Compiles with `avr-gcc -std=c99 -Wall -Wextra -Os` with **zero warnings**. |
| **NFR-02** | No blocking delay longer than **10 ms** anywhere in the super-loop. `_delay_ms()` is permitted only inside `*_Init()` functions. |
| **NFR-03** | Scheduler tick jitter ≤ ±1 ms; a task overrun must increment a counter, not be silently dropped. |
| **NFR-04** | Peak CPU utilisation ≤ 60 %, measured by toggling a spare pin high inside the dispatcher and low in the idle path. |
| **NFR-05** | No floating-point arithmetic anywhere. Fixed-point / integer only. |
| **NFR-06** | No magic numbers. Every threshold, period and pin number is a `#define`, `enum` or `const` in `config.h`. |
| **NFR-07** | The layer rule holds: only `MCAL/*.c` touches hardware registers. |
| **NFR-08** | ISRs are ≤ 10 lines and contain no loops, no I2C, no SPI, no UART TX. |
| **NFR-09** | All shared variables written by an ISR are `volatile`; multi-byte accesses from the main context are atomic. |
| **NFR-10** | Fixed-width types from `<stdint.h>` throughout; no bare `int` in data structures. |
| **NFR-11** | Static RAM usage ≤ 1 KB of the 2 KB available (check with `avr-size`). No `malloc`. |
| **NFR-12** | Every function ≤ 40 lines, one job per function, prototypes in the matching header. |
| **NFR-13** | UART RX must never lose a byte at 9600 bps — use an interrupt-driven ring buffer of ≥ 32 bytes. |
| **NFR-14** | Any user input (button, console) that arrives in an unexpected state is ignored safely; the firmware never resets unexpectedly. |
| **NFR-15** | Actuator outputs are driven low within 100 ms of entering `ST_INIT`. |

---

## 15. Operating Modes

| Mode | Entered by | Automatic loops | Console actuator cmds | Alarm check |
|------|-----------|:---------------:|:---------------------:|:-----------:|
| `INIT` | Power-on / reset | — | Rejected | — |
| `AUTO` | Default after init | Active | Rejected (`ERR MODE`) | Active |
| `MANUAL` | Mode button / `MODE MANUAL` | Suspended | Accepted | Active |
| `ALARM` | Alarm condition from `AUTO` or `MANUAL` | Forced safe | Rejected | Latched |
| `CONFIG` | `SET` command or bonus LCD menu | Frozen | Rejected | Active |

---

## 16. System Flow

```
            ┌──────────────┐
            │  Power ON    │
            └──────┬───────┘
                   ▼
            ┌──────────────────────────────┐
            │ MCAL init: DIO, ADC, Timer0, │
            │ EXTI, USART, SPI, I2C        │
            └──────┬───────────────────────┘
                   ▼
            ┌──────────────────────────────┐
            │ Read Config_t from EEPROM    │
            └──────┬───────────────────────┘
                   ▼
            ┌───────────────┐   invalid    ┌────────────────────┐
            │ magic+CRC OK? ├─────────────▶│ Load defaults,     │
            └──────┬────────┘              │ write back, log    │
                   │ valid                 └─────────┬──────────┘
                   ▼◀────────────────────────────────┘
            ┌──────────────────────────────┐
            │ LCD splash 1 s, actuators    │
            │ forced OFF, sei()            │
            └──────┬───────────────────────┘
                   ▼
        ╔══════════════════════════════════════════╗
        ║        SUPER-LOOP (dispatch on tick)     ║
        ║                                          ║
        ║  every  10 ms → buttons, FSM             ║
        ║  every 100 ms → sample 3 ADC channels    ║
        ║  every 200 ms → run 3 control loops      ║
        ║  every 500 ms → repaint LCD              ║
        ║  every   5 s  → send telemetry frame     ║
        ║  on demand    → console, EEPROM save     ║
        ╚══════════════════════════════════════════╝
```

---

## 17. State Machine

### 17.1 Diagram

```
                       ┌──────────────┐
             power on  │   ST_INIT    │
            ──────────▶│  self-test   │
                       └──────┬───────┘
                              │ init complete
                              ▼
        ┌──────────────────────────────────────────────┐
        │                  ST_AUTO                     │
        │   3 hysteresis loops running                 │
        └───┬───────────────┬──────────────────┬───────┘
            │ MODE btn      │ alarm cond.      │ SET cmd
            ▼               ▼                  ▼
   ┌────────────────┐  ┌──────────────┐  ┌──────────────┐
   │   ST_MANUAL    │  │   ST_ALARM   │  │  ST_CONFIG   │
   │ loops frozen   │  │ latched,     │  │ edit params  │
   │ console drives │  │ safe outputs │  │ then SAVE    │
   └───┬────────┬───┘  └──────┬───────┘  └──────┬───────┘
       │        │ alarm cond. │ RESET &&        │ SAVE / timeout 30 s
       │        └────────────▶│ !cond           │
       │ MODE btn             ▼                 │
       └───────────────▶ (back to previous non-alarm state)
                              │                 │
                              └────────┬────────┘
                                       ▼
                                    ST_AUTO
```

### 17.2 Transition table

| # | From | Event / guard | To | Actions |
|---|------|---------------|----|---------|
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

> **Rule:** the state machine has exactly one `switch (currentState)` in the
> whole codebase. Scattered `if (mode == …)` tests across modules lose the
> Application-logic marks.

---

## 18. UART Protocol

**Link:** 9600 bps, 8 data bits, no parity, 1 stop bit, no flow control.
**Line ending:** device transmits `\r\n`; it accepts `\r`, `\n` or `\r\n`.

### 18.1 Telemetry frame (device → host, every 5 s)

```
$GH,T=31,S=55,L=68,F=1,P=0,M=1,A=0,MODE=AUTO,UP=1250*7C
```

| Field | Meaning | Range |
|-------|---------|-------|
| `$GH` | Frame header | fixed |
| `T` | Temperature °C | 0 – 50 |
| `S` | Soil moisture % | 0 – 100 |
| `L` | Light % | 0 – 100 |
| `F` | Fan state | 0 / 1 |
| `P` | Pump state | 0 / 1 |
| `M` | Lamp state | 0 / 1 |
| `A` | Alarm latched | 0 / 1 |
| `MODE` | `AUTO` \| `MANUAL` \| `ALARM` \| `CONFIG` | |
| `UP` | Uptime, seconds | 0 – 65535 |
| `*7C` | XOR checksum of everything between `$` and `*`, 2 hex digits | |

A human-readable multi-line form is acceptable **in addition to**, never instead
of, the machine frame — automated marking parses the frame above.

### 18.2 Command set (host → device)

| Command | Response | Effect |
|---------|----------|--------|
| `STATUS` | one telemetry frame | Force an immediate report |
| `TEMP?` | `TEMP=31` | Read one value |
| `SOIL?` | `SOIL=55` | |
| `LIGHT?` | `LIGHT=68` | |
| `CFG?` | `CFG=35,32,40,60,25,40,45,15` | Dump all thresholds in `Config_t` order |
| `SET TEMPON <n>` | `OK` / `ERR RANGE` | 10 ≤ n ≤ 50, must exceed `TEMPOFF` + 2 |
| `SET TEMPOFF <n>` | `OK` / `ERR RANGE` | |
| `SET SOILON <n>` | `OK` / `ERR RANGE` | 5 ≤ n ≤ 95 |
| `SET SOILOFF <n>` | `OK` / `ERR RANGE` | |
| `SET LIGHTON <n>` | `OK` / `ERR RANGE` | |
| `SET LIGHTOFF <n>` | `OK` / `ERR RANGE` | |
| `MODE AUTO` | `OK` | Leave manual |
| `MODE MANUAL` | `OK` | Enter manual |
| `FAN ON` / `FAN OFF` | `OK` / `ERR MODE` | Manual mode only |
| `PUMP ON` / `PUMP OFF` | `OK` / `ERR MODE` | |
| `LAMP ON` / `LAMP OFF` | `OK` / `ERR MODE` | |
| `SAVE` | `OK` / `ERR EEPROM` | Persist config |
| `LOAD` | `OK` / `ERR CRC` | Reload from EEPROM |
| `RESET` | `OK` / `ERR ACTIVE` | Clear alarm latch |
| `DEFAULTS` | `OK` | Factory defaults (does **not** auto-save) |
| `HELP` | command list | |

**Error vocabulary:** `ERR CMD`, `ERR ARG`, `ERR RANGE`, `ERR MODE`,
`ERR ACTIVE`, `ERR LONG`, `ERR EEPROM`, `ERR CRC`.

### 18.3 Asynchronous events (device → host)

Events are emitted the moment they happen, independently of the 5 s cadence:

```
!EVT,BOOT
!EVT,FAN,ON
!EVT,PUMP,OFF
!EVT,ALARM,TEMP,47
!EVT,ALARM,CLR
!EVT,SAVE,OK
!EVT,SENSOR,FAULT,1
!EVT,PUMP,TIMEOUT
```

---

## 19. EEPROM Data Layout

**Device:** 25LC256 (32 KB), SPI Mode 0, `SS` on `PB4`.
**Page size:** 64 bytes — a write must never cross a page boundary.

### 19.1 Memory map

| Address | Size | Field | Type | Default |
|---------|:----:|-------|------|:-------:|
| `0x0000` | 2 | `magic` | `uint16_t` | `0xA5C3` |
| `0x0002` | 1 | `version` | `uint8_t` | `0x01` |
| `0x0003` | 1 | `tempOnC` | `uint8_t` | 35 |
| `0x0004` | 1 | `tempOffC` | `uint8_t` | 32 |
| `0x0005` | 1 | `soilOnPct` | `uint8_t` | 40 |
| `0x0006` | 1 | `soilOffPct` | `uint8_t` | 60 |
| `0x0007` | 1 | `lightOnPct` | `uint8_t` | 25 |
| `0x0008` | 1 | `lightOffPct` | `uint8_t` | 40 |
| `0x0009` | 1 | `tempAlarmC` | `uint8_t` | 45 |
| `0x000A` | 1 | `soilAlarmPct` | `uint8_t` | 15 |
| `0x000B` | 1 | `mode` | `uint8_t` | 0 (AUTO) |
| `0x000C` | 1 | `checksum` | `uint8_t` | computed |
| `0x000D` – `0x001F` | 19 | reserved (write `0xFF`) | — | — |
| `0x0020` | 2 | `pumpTotalSec` | `uint16_t` | 0 |
| `0x0022` | 2 | `bootCount` | `uint16_t` | 0 |
| `0x0024` | 1 | `lastAlarmCause` | `uint8_t` | 0 |
| `0x0025` – `0x003F` | 27 | reserved | — | — |

### 19.2 Checksum

```c
uint8_t cfg_checksum(const Config_t *c)
{
    const uint8_t *p = (const uint8_t *)c;
    uint8_t sum = 0;
    for (uint8_t i = 0; i < sizeof(Config_t) - 1u; i++) {
        sum = (uint8_t)(sum + p[i]);
    }
    return (uint8_t)(0u - sum);          /* stored so that total sum == 0 */
}
```
Validation: read the record, sum **all** `sizeof(Config_t)` bytes; the result
must be `0`.

### 19.3 25LC256 command reference

| Op | Opcode | Sequence |
|----|:------:|----------|
| `WREN` | `0x06` | assert `SS`, send `0x06`, release `SS` |
| `WRDI` | `0x04` | disable writes |
| `RDSR` | `0x05` | send `0x05`, read 1 byte; bit0 = `WIP` |
| `WRSR` | `0x01` | send `0x01` + status byte |
| `READ` | `0x03` | send `0x03`, addrH, addrL, then clock out N bytes |
| `WRITE` | `0x02` | `WREN` first; `0x02`, addrH, addrL, then N ≤ 64 bytes |

Write cycle time ≈ 5 ms — poll `WIP` in the scheduler, do not busy-wait.

---

## 20. Task Scheduling

Cooperative, non-preemptive. Timer0 CTC ISR sets `g_tick`; `main()` dispatches.

| ID | Task | Period | Offset | Budget | Work |
|----|------|:------:|:------:|:------:|------|
| T-1 | `Task_Buttons` | 10 ms | 0 | 100 µs | Debounce, edge detect |
| T-2 | `Task_FSM` | 10 ms | 0 | 200 µs | One `switch` pass, timers |
| T-3 | `Task_Sample` | 100 ms | 1 | 1 ms | 3× ADC + median + scale |
| T-4 | `Task_Control` | 200 ms | 3 | 300 µs | 3 hysteresis loops |
| T-5 | `Task_LCD` | 500 ms | 5 | 4 ms | Repaint changed cells |
| T-6 | `Task_Report` | 5 s | 7 | 2 ms | Build + queue telemetry |
| T-7 | `Task_Console` | 20 ms | 2 | 500 µs | Parse one complete line |
| T-8 | `Task_EEPROM` | on event | — | 50 ms* | Save / load, `WIP` polled |

\* The EEPROM task is a small state machine of its own: `IDLE → WREN → WRITE →
WAIT_WIP → VERIFY → DONE`, one step per 10 ms tick. It must **not** block.

**Offsets** stagger heavy tasks so they never land on the same tick. Sum of all
work in the worst tick must stay under 10 ms — show the arithmetic in your
report.

---

## 21. Testing Requirements

Fill in `Docs/test_report.md` with one row per case, including the observed
value and a screenshot reference.

| ID | Test | Method | Pass criterion |
|----|------|--------|----------------|
| TC-01 | Boot with blank EEPROM | Erase EEPROM, power on | Defaults loaded, `!EVT,CFG,DEFAULT` sent, LCD live < 500 ms |
| TC-02 | Boot with valid config | Save 38 °C, power cycle | 38 °C restored, shown by `CFG?` |
| TC-03 | Boot with corrupted config | Flip one EEPROM byte | Defaults loaded, no crash |
| TC-04 | Temperature scaling | Pot at 0 %, 50 %, 100 % | Reads 0, 25±1, 50 °C |
| TC-05 | Soil scaling | Pot at 0 %, 50 %, 100 % | Reads 0, 50±1, 100 % |
| TC-06 | Light scaling | Pot at 0 %, 50 %, 100 % | Reads 0, 50±1, 100 % |
| TC-07 | ADC conversion time | Toggle pin around conversion | ≤ 120 µs |
| TC-08 | Fan ON edge | Sweep 30 → 40 °C | Fan turns on exactly at > 35 °C |
| TC-09 | Fan OFF edge | Sweep 40 → 30 °C | Fan turns off exactly at < 32 °C |
| TC-10 | Fan chatter | Hold 33 °C for 30 s | Zero transitions |
| TC-11 | Pump hysteresis | Sweep 70 → 30 → 70 % | One ON edge at < 40 %, one OFF at > 60 % |
| TC-12 | Pump dry-run timeout | Force soil to 10 %, wait 60 s | Pump forced off, `!EVT,PUMP,TIMEOUT` |
| TC-13 | Lamp hysteresis | Sweep 60 → 10 → 60 % | ON at < 25 %, OFF at > 40 % |
| TC-14 | Over-temp alarm | Raise to 47 °C | Alarm latches, buzzer beeps, fan forced on |
| TC-15 | Alarm latch | Lower to 30 °C | Alarm stays on until reset |
| TC-16 | Alarm reset blocked | Press reset at 47 °C | `ERR ACTIVE`, alarm still on |
| TC-17 | Alarm reset allowed | Reset at 30 °C | Alarm clears, returns to previous state |
| TC-18 | Dry alarm | Soil to 10 % | Alarm latches with cause `SOIL` |
| TC-19 | Mode toggle | Press Mode | `AUTO ↔ MANUAL` on LCD and in frame |
| TC-20 | Manual override | `MANUAL`, then `FAN ON` | Fan on regardless of temperature |
| TC-21 | Manual rejects in AUTO | `AUTO`, then `FAN ON` | `ERR MODE` |
| TC-22 | Alarm active in manual | `MANUAL`, raise to 47 °C | Alarm still fires |
| TC-23 | EEPROM save & verify | `SET TEMPON 38`, `SAVE`, power cycle | Value persists |
| TC-24 | Console unknown verb | Send `FOO` | `ERR CMD` |
| TC-25 | Console long line | Send 40 chars | `ERR LONG`, no reset |
| TC-26 | Console bad range | `SET TEMPON 99` | `ERR RANGE`, value unchanged |
| TC-27 | Hysteresis guard | `SET TEMPOFF 36` while `TEMPON`=35 | `ERR RANGE` |
| TC-28 | Telemetry cadence | Capture 60 s | 12 frames ±1, checksum valid on all |
| TC-29 | Telemetry integrity | Verify `*XX` on 100 frames | 100 % correct |
| TC-30 | Button debounce | Rapid presses on Mode | One toggle per physical press |
| TC-31 | Factory reset | Hold Save + Reset 3 s | Defaults restored, countdown shown |
| TC-32 | Factory reset abort | Release at 2 s | No change |
| TC-33 | Sensor fault | Short ADC0 to GND for 6 s | `SENSOR FAULT 0`, thermal loop safe |
| TC-34 | Sensor recovery | Return pot to mid | Fault clears within 1 s |
| TC-35 | Tick jitter | Toggle pin in tick ISR, scope it | 10 ms ±1 ms |
| TC-36 | CPU load | Measure busy pin duty | ≤ 60 % |
| TC-37 | RAM budget | `avr-size -C --mcu=atmega32` | `.data + .bss` ≤ 1024 B |
| TC-38 | LCD flicker | Watch 60 s | No visible clear-and-redraw |
| TC-39 | Pump run-time counter | Run pump 30 s, `STATUS` | `PUMPSEC` ≈ 30 |
| TC-40 | Long soak | Run 10 min with random pot moves | No hang, no missed frames |

---

## 22. Bonus Features

Maximum **+20** total; final score capped at 100.

| # | Feature | Marks | Requirement |
|---|---------|:-----:|-------------|
| B1 | Interrupt-driven buttons | +5 | INT0/INT1 with software debounce; ISR only sets flags |
| B2 | Full command parser | +10 | Complete §18.2 set, robust to junk, `HELP` implemented |
| B3 | LCD menu system | +10 | Navigate and edit every threshold with the three buttons — no PC needed |
| B4 | True cooperative scheduler | +15 | Task table with period/offset/handler, overrun counter reported over UART |
| B5 | Watchdog recovery | +10 | WDT enabled at 250 ms, kicked from the dispatcher; a deliberate hang recovers and logs `!EVT,WDT,RESET` (read `MCUCSR`) |
| B6 | Data logging | +10 | Circular log of 32 samples in EEPROM `0x0100+`, dumped by `LOG?` |
| B7 | Buzzer PWM tones | +5 | Timer2 fast PWM; distinct tone per alarm cause |
| B8 | Day/night photoperiod | +10 | Lamp obeys a 16 h-on / 8 h-off schedule (accelerated 100× for the demo) on top of the light loop |

---

## 23. Deliverables

| # | Item | Detail |
|---|------|--------|
| 1 | Source code | Layered per §9.1, builds clean with the provided `Makefile` |
| 2 | `Simulation/greenhouse.sim1` | Opens and runs in SimulIDE without edits |
| 3 | `Docs/flowchart.png` | Matches §16 |
| 4 | `Docs/state_machine.png` | Matches §17, includes the transition table |
| 5 | `Docs/test_report.md` | All 40 `TC` rows with results and evidence |
| 6 | Final report | 15 – 20 pages: analysis, design decisions, hysteresis maths, timing budget, problems and fixes |
| 7 | Demo video | 5 – 10 min narrated: boot, each loop, alarm, save/restore, console |
| 8 | Live defence | Every member answers questions on any file |

---

## 24. Evaluation Rubric

| Item | Marks | Full-mark criteria |
|------|:-----:|--------------------|
| GPIO | 5 | Own DIO driver; no register access outside MCAL |
| ADC | 10 | Correct prescaler, 3-channel scan, median filter, integer scaling |
| Timer | 10 | 10 ms tick within ±1 ms; used for scheduler, debounce and timeouts |
| Interrupts | 5 | Short ISRs, `volatile` discipline, debounce outside the ISR |
| USART | 10 | Exact frame with valid checksum; parser survives every TC-24…27 |
| SPI | 10 | EEPROM save/verify/restore proven across a power cycle |
| I2C | 10 | LCD via PCF8574, no flicker, correct 4-bit nibble sequencing |
| Application logic | 20 | All three hysteresis loops correct; FSM matches §17 exactly |
| Architecture | 10 | Layer rule respected, `config.h` holds every constant, no magic numbers |
| Testing | 10 | 40 test cases executed with evidence |
| Documentation & demo | 10 | Diagrams match code; every member can defend any module |
| **Total** | **100** | Bonus up to +20, capped at 100 |

---

*Prepared by Ahmed Ellamie | ahmed.ellamiee@gmail.com*
??? ??????? ?? ????? ??? ????? ??? ?????? ?? ???? ??????.
