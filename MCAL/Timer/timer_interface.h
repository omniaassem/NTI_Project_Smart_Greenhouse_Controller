#ifndef TIMER_INTERFACE_H
#define TIMER_INTERFACE_H

#include "../../Service/STD_Types.h"
#include "timer_registers.h"

/* ================================================================================
 *  TIMER DRIVER - PUBLIC INTERFACE (ATmega32)
 *  ------------------------------------------------------------------------------
 *  This driver abstracts the three timer/counter units of the ATmega32 behind a
 *  single, channel-based API. The caller selects a channel (Timer0/1/2), an
 *  operating mode, and a prescaler through a configuration structure, then uses
 *  the runtime functions to start/stop the timer, read/write its counter, set a
 *  compare value, or register an interrupt callback.
 * ============================================================================== */

/* ---------------- Timer Channels ---------------- */
/**
 * @brief Identifies which physical timer/counter unit an API call targets.
 *        TIMER_CHANNEL_0 and TIMER_CHANNEL_2 are 8-bit; TIMER_CHANNEL_1 is 16-bit.
 */
typedef enum
{
    TIMER_CHANNEL_0 = 0,   /* 8-bit  Timer0 */
    TIMER_CHANNEL_1 = 1,   /* 16-bit Timer1 */
    TIMER_CHANNEL_2 = 2,   /* 8-bit  Timer2 */
    TIMER_CHANNEL_MAX      /* Sentinel used for range checking - not a real channel */
} Timer_ChannelType;

/* ---------------- Operating Modes ---------------- */
/**
 * @brief Waveform generation mode of the selected timer.
 *  - TIMER_MODE_NORMAL       : Counter runs 0 -> TOP and overflows (TOV interrupt).
 *  - TIMER_MODE_CTC          : Counter runs 0 -> OCR and resets (compare-match interrupt).
 *  - TIMER_MODE_FAST_PWM     : Single-slope PWM on the OCx pin.
 *  - TIMER_MODE_PHASE_PWM    : Dual-slope (phase-correct) PWM on the OCx pin.
 */
typedef enum
{
    TIMER_MODE_NORMAL     = 0,
    TIMER_MODE_CTC        = 1,
    TIMER_MODE_FAST_PWM   = 2,
    TIMER_MODE_PHASE_PWM  = 3
} Timer_ModeType;

/* ---------------- Clock Prescaler ---------------- */
/**
 * @brief Clock source / prescaler for the timer. The numeric value matches the
 *        CSx2:CSx0 bit pattern that is written into the control register.
 *        TIMER_CLOCK_STOPPED disconnects the clock and effectively pauses the timer.
 */
typedef enum
{
    TIMER_CLOCK_STOPPED   = 0,   /* No clock source - timer stopped */
    TIMER_CLOCK_DIV_1     = 1,   /* clk/1    (no prescaling)        */
    TIMER_CLOCK_DIV_8     = 2,   /* clk/8                           */
    TIMER_CLOCK_DIV_64    = 3,   /* clk/64                          */
    TIMER_CLOCK_DIV_256   = 4,   /* clk/256                         */
    TIMER_CLOCK_DIV_1024  = 5    /* clk/1024                        */
} Timer_PrescalerType;

/* ---------------- Interrupt Sources ---------------- */
/**
 * @brief Selects which timer event fires an interrupt / callback.
 *  - TIMER_INT_OVERFLOW      : Fires when the counter overflows past TOP.
 *  - TIMER_INT_COMPARE_MATCH : Fires when the counter equals the compare value.
 */
typedef enum
{
    TIMER_INT_OVERFLOW       = 0,
    TIMER_INT_COMPARE_MATCH  = 1
} Timer_InterruptType;

/* ---------------- Configuration Structure ---------------- */
/**
 * @brief Aggregates everything Timer_Init() needs to configure one channel.
 * @var Timer_ConfigType::channel        Which timer unit to configure (Timer_ChannelType).
 * @var Timer_ConfigType::mode           Waveform generation mode (Timer_ModeType).
 * @var Timer_ConfigType::prescaler      Clock source / prescaler (Timer_PrescalerType).
 * @var Timer_ConfigType::initialValue   Value preloaded into the counter register (TCNTx).
 * @var Timer_ConfigType::compareValue   Value written to the compare register (OCRx),
 *                                        used in CTC / PWM modes and for compare interrupts.
 */
typedef struct
{
    Timer_ChannelType   channel;
    Timer_ModeType      mode;
    Timer_PrescalerType prescaler;
    uint16_h            initialValue;
    uint16_h            compareValue;
} Timer_ConfigType;

/* ---------------- Callback Pointer Type ---------------- */
/**
 * @brief Type of the user function invoked from the timer ISR.
 *        Keep the callback short and non-blocking - it runs in interrupt context.
 */
typedef void (*Timer_CallBackType)(void);

/* ================================================================================
 *  FUNCTION PROTOTYPES
 * ============================================================================== */

/**
 * @brief  Configures a timer channel (mode, prescaler, initial + compare values)
 *         without starting it. Loads TCNTx/OCRx and selects the waveform mode.
 * @param  addConfig  Pointer to a fully populated configuration structure.
 * @return STD_ReturnType  E_OK if configured, E_NOK on NULL pointer or bad channel.
 * @note   The clock is applied here; pass TIMER_CLOCK_STOPPED and call Timer_Start()
 *         later if you want to defer the actual counting.
 */
STD_ReturnType Timer_Init(const Timer_ConfigType *addConfig);

/**
 * @brief  Stops the channel and restores its control registers to reset values.
 * @param  channel  Timer channel to de-initialize.
 * @return STD_ReturnType  E_OK/E_NOK (E_NOK on invalid channel).
 */
STD_ReturnType Timer_DeInit(Timer_ChannelType channel);

/**
 * @brief  (Re)connects the clock source so the selected channel starts counting.
 * @param  channel    Timer channel to start.
 * @param  prescaler  Clock source / prescaler to apply.
 * @return STD_ReturnType  E_OK/E_NOK.
 */
STD_ReturnType Timer_Start(Timer_ChannelType channel, Timer_PrescalerType prescaler);

/**
 * @brief  Disconnects the clock source so the channel stops counting (value kept).
 * @param  channel  Timer channel to stop.
 * @return STD_ReturnType  E_OK/E_NOK.
 */
STD_ReturnType Timer_Stop(Timer_ChannelType channel);

/**
 * @brief  Writes the counter register (TCNTx) of the selected channel.
 * @param  channel  Timer channel.
 * @param  value    New counter value (8-bit for Timer0/2, 16-bit for Timer1).
 * @return STD_ReturnType  E_OK/E_NOK.
 */
STD_ReturnType Timer_SetCounterValue(Timer_ChannelType channel, uint16_h value);

/**
 * @brief  Reads the current counter register (TCNTx) of the selected channel.
 * @param  channel     Timer channel.
 * @param  puint16Val  Pointer that receives the current counter value.
 * @return STD_ReturnType  E_OK/E_NOK (E_NOK on NULL pointer or bad channel).
 */
STD_ReturnType Timer_GetCounterValue(Timer_ChannelType channel, uint16_h *puint16Val);

/**
 * @brief  Writes the output-compare register (OCRx) used in CTC / PWM modes.
 * @param  channel  Timer channel.
 * @param  value    Compare value (8-bit for Timer0/2, 16-bit for Timer1 channel A).
 * @return STD_ReturnType  E_OK/E_NOK.
 */
STD_ReturnType Timer_SetCompareValue(Timer_ChannelType channel, uint16_h value);

/**
 * @brief  Enables the given interrupt source for the channel (updates TIMSK).
 *         The user must also enable global interrupts (see Timer_EnableGlobalInterrupt).
 * @param  channel   Timer channel.
 * @param  intType   Overflow or compare-match interrupt.
 * @return STD_ReturnType  E_OK/E_NOK.
 */
STD_ReturnType Timer_EnableInterrupt(Timer_ChannelType channel, Timer_InterruptType intType);

/**
 * @brief  Disables the given interrupt source for the channel (updates TIMSK).
 * @param  channel   Timer channel.
 * @param  intType   Overflow or compare-match interrupt.
 * @return STD_ReturnType  E_OK/E_NOK.
 */
STD_ReturnType Timer_DisableInterrupt(Timer_ChannelType channel, Timer_InterruptType intType);

/**
 * @brief  Registers the callback invoked from the channel's ISR for a given source.
 * @param  channel   Timer channel.
 * @param  intType   Which event the callback belongs to (overflow / compare).
 * @param  callBack  Pointer to a void(void) function; must not be NULL.
 * @return STD_ReturnType  E_OK/E_NOK.
 */
STD_ReturnType Timer_SetCallBack(Timer_ChannelType channel,
                                 Timer_InterruptType intType,
                                 Timer_CallBackType callBack);

/**
 * @brief  Sets the global interrupt enable bit (I-bit of SREG). Equivalent to sei().
 */
void Timer_EnableGlobalInterrupt(void);

/**
 * @brief  Clears the global interrupt enable bit (I-bit of SREG). Equivalent to cli().
 */
void Timer_DisableGlobalInterrupt(void);

#endif /* TIMER_INTERFACE_H */
