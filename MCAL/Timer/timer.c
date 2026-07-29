#include "../../Service/STD_Types.h"
#include "../../Service/Bit_Math.h"
#include <avr/interrupt.h>
#include "timer_registers.h"
#include "timer_interface.h"

/* ================================================================================
 *  TIMER DRIVER - IMPLEMENTATION (ATmega32)
 * ============================================================================== */

/*
 * Callback storage: one slot per channel per interrupt source.
 * [channel][TIMER_INT_OVERFLOW] = overflow callback,
 * [channel][TIMER_INT_COMPARE_MATCH] = compare-match callback.
 */
static Timer_CallBackType Timer_CallBacks[TIMER_CHANNEL_MAX][2] = { { NULL, NULL },
                                                                    { NULL, NULL },
                                                                    { NULL, NULL } };


/* Set the waveform-generation-mode bits for the selected channel. */
static void Timer_SetMode(Timer_ChannelType channel, Timer_ModeType mode)
{
    switch (channel)
    {
        case TIMER_CHANNEL_0:
            /* WGM00 = bit6, WGM01 = bit3. */
            switch (mode)
            {
                case TIMER_MODE_NORMAL:
                    CLR_BIT(TIMER_TCCR0_REG, TIMER_WGM00_BIT);
                    CLR_BIT(TIMER_TCCR0_REG, TIMER_WGM01_BIT);
                    break;
                case TIMER_MODE_CTC:
                    CLR_BIT(TIMER_TCCR0_REG, TIMER_WGM00_BIT);
                    SET_BIT(TIMER_TCCR0_REG, TIMER_WGM01_BIT);
                    break;
                case TIMER_MODE_FAST_PWM:
                    SET_BIT(TIMER_TCCR0_REG, TIMER_WGM00_BIT);
                    SET_BIT(TIMER_TCCR0_REG, TIMER_WGM01_BIT);
                    break;
                case TIMER_MODE_PHASE_PWM:
                default:
                    SET_BIT(TIMER_TCCR0_REG, TIMER_WGM00_BIT);
                    CLR_BIT(TIMER_TCCR0_REG, TIMER_WGM01_BIT);
                    break;
            }
            break;

        case TIMER_CHANNEL_2:
            /* WGM20 = bit6, WGM21 = bit3. */
            switch (mode)
            {
                case TIMER_MODE_NORMAL:
                    CLR_BIT(TIMER_TCCR2_REG, TIMER_WGM20_BIT);
                    CLR_BIT(TIMER_TCCR2_REG, TIMER_WGM21_BIT);
                    break;
                case TIMER_MODE_CTC:
                    CLR_BIT(TIMER_TCCR2_REG, TIMER_WGM20_BIT);
                    SET_BIT(TIMER_TCCR2_REG, TIMER_WGM21_BIT);
                    break;
                case TIMER_MODE_FAST_PWM:
                    SET_BIT(TIMER_TCCR2_REG, TIMER_WGM20_BIT);
                    SET_BIT(TIMER_TCCR2_REG, TIMER_WGM21_BIT);
                    break;
                case TIMER_MODE_PHASE_PWM:
                default:
                    SET_BIT(TIMER_TCCR2_REG, TIMER_WGM20_BIT);
                    CLR_BIT(TIMER_TCCR2_REG, TIMER_WGM21_BIT);
                    break;
            }
            break;

        case TIMER_CHANNEL_1:
        default:
            /* Timer1 16-bit: WGM11:10 in TCCR1A, WGM13:12 in TCCR1B. */
            CLR_BIT(TIMER_TCCR1A_REG, TIMER_WGM10_BIT);
            CLR_BIT(TIMER_TCCR1A_REG, TIMER_WGM11_BIT);
            CLR_BIT(TIMER_TCCR1B_REG, TIMER_WGM12_BIT);
            CLR_BIT(TIMER_TCCR1B_REG, TIMER_WGM13_BIT);
            switch (mode)
            {
                case TIMER_MODE_NORMAL:
                    /* Mode 0: 0000 */
                    break;
                case TIMER_MODE_CTC:
                    /* Mode 4: CTC, TOP = OCR1A -> WGM12 = 1 */
                    SET_BIT(TIMER_TCCR1B_REG, TIMER_WGM12_BIT);
                    break;
                case TIMER_MODE_FAST_PWM:
                    /* Mode 5: Fast PWM 8-bit -> WGM12 = 1, WGM10 = 1 */
                    SET_BIT(TIMER_TCCR1A_REG, TIMER_WGM10_BIT);
                    SET_BIT(TIMER_TCCR1B_REG, TIMER_WGM12_BIT);
                    break;
                case TIMER_MODE_PHASE_PWM:
                default:
                    /* Mode 1: Phase-correct PWM 8-bit -> WGM10 = 1 */
                    SET_BIT(TIMER_TCCR1A_REG, TIMER_WGM10_BIT);
                    break;
            }
            break;
    }
}


STD_ReturnType Timer_Init(const Timer_ConfigType *addConfig)
{
    if ((addConfig == NULL) || (addConfig->channel >= TIMER_CHANNEL_MAX))
    {
        return E_NOK;
    }

    Timer_SetMode(addConfig->channel, addConfig->mode);

    switch (addConfig->channel)
    {
        case TIMER_CHANNEL_0:
            TIMER_TCNT0_REG = (u8)addConfig->initialValue;
            TIMER_OCR0_REG  = (u8)addConfig->compareValue;
            break;
        case TIMER_CHANNEL_1:
            TIMER_TCNT1_REG = addConfig->initialValue;
            TIMER_OCR1A_REG = addConfig->compareValue;
            break;
        case TIMER_CHANNEL_2:
            TIMER_TCNT2_REG = (u8)addConfig->initialValue;
            TIMER_OCR2_REG  = (u8)addConfig->compareValue;
            break;
        default:
            return E_NOK;
    }

    /* Connect the clock (or stay paused if TIMER_CLOCK_STOPPED). */
    return Timer_Start(addConfig->channel, addConfig->prescaler);
}


STD_ReturnType Timer_DeInit(Timer_ChannelType channel)
{
    if (channel >= TIMER_CHANNEL_MAX)
    {
        return E_NOK;
    }

    (void)Timer_DisableInterrupt(channel, TIMER_INT_OVERFLOW);
    (void)Timer_DisableInterrupt(channel, TIMER_INT_COMPARE_MATCH);

    switch (channel)
    {
        case TIMER_CHANNEL_0:
            TIMER_TCCR0_REG = 0U;
            TIMER_TCNT0_REG = 0U;
            break;
        case TIMER_CHANNEL_1:
            TIMER_TCCR1A_REG = 0U;
            TIMER_TCCR1B_REG = 0U;
            TIMER_TCNT1_REG  = 0U;
            break;
        case TIMER_CHANNEL_2:
            TIMER_TCCR2_REG = 0U;
            TIMER_TCNT2_REG = 0U;
            break;
        default:
            return E_NOK;
    }

    Timer_CallBacks[channel][TIMER_INT_OVERFLOW]      = NULL;
    Timer_CallBacks[channel][TIMER_INT_COMPARE_MATCH] = NULL;

    return E_OK;
}


STD_ReturnType Timer_Start(Timer_ChannelType channel, Timer_PrescalerType prescaler)
{
    if (channel >= TIMER_CHANNEL_MAX)
    {
        return E_NOK;
    }

    switch (channel)
    {
        case TIMER_CHANNEL_0:
            TIMER_TCCR0_REG &= ~((1U << TIMER_CS02_BIT) | (1U << TIMER_CS01_BIT) |
                                 (1U << TIMER_CS00_BIT));
            TIMER_TCCR0_REG |= ((uint8_h)prescaler & 0x07U);
            break;
        case TIMER_CHANNEL_1:
            TIMER_TCCR1B_REG &= ~((1U << TIMER_CS12_BIT) | (1U << TIMER_CS11_BIT) |
                                  (1U << TIMER_CS10_BIT));
            TIMER_TCCR1B_REG |= ((uint8_h)prescaler & 0x07U);
            break;
        case TIMER_CHANNEL_2:
            TIMER_TCCR2_REG &= ~((1U << TIMER_CS22_BIT) | (1U << TIMER_CS21_BIT) |
                                 (1U << TIMER_CS20_BIT));
            TIMER_TCCR2_REG |= ((uint8_h)prescaler & 0x07U);
            break;
        default:
            return E_NOK;
    }

    return E_OK;
}


STD_ReturnType Timer_Stop(Timer_ChannelType channel)
{
    if (channel >= TIMER_CHANNEL_MAX)
    {
        return E_NOK;
    }

    switch (channel)
    {
        case TIMER_CHANNEL_0:
            TIMER_TCCR0_REG &= ~((1U << TIMER_CS02_BIT) | (1U << TIMER_CS01_BIT) |
                                 (1U << TIMER_CS00_BIT));
            break;
        case TIMER_CHANNEL_1:
            TIMER_TCCR1B_REG &= ~((1U << TIMER_CS12_BIT) | (1U << TIMER_CS11_BIT) |
                                  (1U << TIMER_CS10_BIT));
            break;
        case TIMER_CHANNEL_2:
            TIMER_TCCR2_REG &= ~((1U << TIMER_CS22_BIT) | (1U << TIMER_CS21_BIT) |
                                 (1U << TIMER_CS20_BIT));
            break;
        default:
            return E_NOK;
    }

    return E_OK;
}


STD_ReturnType Timer_SetCounterValue(Timer_ChannelType channel, uint16_h value)
{
    if (channel >= TIMER_CHANNEL_MAX)
    {
        return E_NOK;
    }

    switch (channel)
    {
        case TIMER_CHANNEL_0: TIMER_TCNT0_REG = (u8)value; break;
        case TIMER_CHANNEL_1: TIMER_TCNT1_REG = value;     break;
        case TIMER_CHANNEL_2: TIMER_TCNT2_REG = (u8)value; break;
        default: return E_NOK;
    }

    return E_OK;
}


STD_ReturnType Timer_GetCounterValue(Timer_ChannelType channel, uint16_h *puint16Val)
{
    if ((channel >= TIMER_CHANNEL_MAX) || (puint16Val == NULL))
    {
        return E_NOK;
    }

    switch (channel)
    {
        case TIMER_CHANNEL_0: *puint16Val = TIMER_TCNT0_REG; break;
        case TIMER_CHANNEL_1: *puint16Val = TIMER_TCNT1_REG; break;
        case TIMER_CHANNEL_2: *puint16Val = TIMER_TCNT2_REG; break;
        default: return E_NOK;
    }

    return E_OK;
}


STD_ReturnType Timer_SetCompareValue(Timer_ChannelType channel, uint16_h value)
{
    if (channel >= TIMER_CHANNEL_MAX)
    {
        return E_NOK;
    }

    switch (channel)
    {
        case TIMER_CHANNEL_0: TIMER_OCR0_REG  = (u8)value; break;
        case TIMER_CHANNEL_1: TIMER_OCR1A_REG = value;     break;
        case TIMER_CHANNEL_2: TIMER_OCR2_REG  = (u8)value; break;
        default: return E_NOK;
    }

    return E_OK;
}


STD_ReturnType Timer_EnableInterrupt(Timer_ChannelType channel, Timer_InterruptType intType)
{
    if (channel >= TIMER_CHANNEL_MAX)
    {
        return E_NOK;
    }

    switch (channel)
    {
        case TIMER_CHANNEL_0:
            SET_BIT(TIMER_TIMSK_REG,
                    (intType == TIMER_INT_OVERFLOW) ? TIMER_TOIE0_BIT : TIMER_OCIE0_BIT);
            break;
        case TIMER_CHANNEL_1:
            SET_BIT(TIMER_TIMSK_REG,
                    (intType == TIMER_INT_OVERFLOW) ? TIMER_TOIE1_BIT : TIMER_OCIE1A_BIT);
            break;
        case TIMER_CHANNEL_2:
            SET_BIT(TIMER_TIMSK_REG,
                    (intType == TIMER_INT_OVERFLOW) ? TIMER_TOIE2_BIT : TIMER_OCIE2_BIT);
            break;
        default:
            return E_NOK;
    }

    return E_OK;
}


STD_ReturnType Timer_DisableInterrupt(Timer_ChannelType channel, Timer_InterruptType intType)
{
    if (channel >= TIMER_CHANNEL_MAX)
    {
        return E_NOK;
    }

    switch (channel)
    {
        case TIMER_CHANNEL_0:
            CLR_BIT(TIMER_TIMSK_REG,
                    (intType == TIMER_INT_OVERFLOW) ? TIMER_TOIE0_BIT : TIMER_OCIE0_BIT);
            break;
        case TIMER_CHANNEL_1:
            CLR_BIT(TIMER_TIMSK_REG,
                    (intType == TIMER_INT_OVERFLOW) ? TIMER_TOIE1_BIT : TIMER_OCIE1A_BIT);
            break;
        case TIMER_CHANNEL_2:
            CLR_BIT(TIMER_TIMSK_REG,
                    (intType == TIMER_INT_OVERFLOW) ? TIMER_TOIE2_BIT : TIMER_OCIE2_BIT);
            break;
        default:
            return E_NOK;
    }

    return E_OK;
}


STD_ReturnType Timer_SetCallBack(Timer_ChannelType channel,
                                 Timer_InterruptType intType,
                                 Timer_CallBackType callBack)
{
    if ((channel >= TIMER_CHANNEL_MAX) || (callBack == NULL))
    {
        return E_NOK;
    }

    Timer_CallBacks[channel][intType] = callBack;
    return E_OK;
}


void Timer_EnableGlobalInterrupt(void)
{
    SET_BIT(TIMER_SREG_REG, TIMER_GLOBAL_INT_BIT);   /* sei() */
}


void Timer_DisableGlobalInterrupt(void)
{
    CLR_BIT(TIMER_SREG_REG, TIMER_GLOBAL_INT_BIT);   /* cli() */
}


/* ================================================================================
 *  INTERRUPT SERVICE ROUTINES
 *  Each vector dispatches to the registered callback if one is set.
 * ============================================================================== */
ISR(TIMER0_OVF_vect)
{
    if (Timer_CallBacks[TIMER_CHANNEL_0][TIMER_INT_OVERFLOW] != NULL)
    {
        Timer_CallBacks[TIMER_CHANNEL_0][TIMER_INT_OVERFLOW]();
    }
}

ISR(TIMER0_COMP_vect)
{
    if (Timer_CallBacks[TIMER_CHANNEL_0][TIMER_INT_COMPARE_MATCH] != NULL)
    {
        Timer_CallBacks[TIMER_CHANNEL_0][TIMER_INT_COMPARE_MATCH]();
    }
}

ISR(TIMER1_OVF_vect)
{
    if (Timer_CallBacks[TIMER_CHANNEL_1][TIMER_INT_OVERFLOW] != NULL)
    {
        Timer_CallBacks[TIMER_CHANNEL_1][TIMER_INT_OVERFLOW]();
    }
}

ISR(TIMER1_COMPA_vect)
{
    if (Timer_CallBacks[TIMER_CHANNEL_1][TIMER_INT_COMPARE_MATCH] != NULL)
    {
        Timer_CallBacks[TIMER_CHANNEL_1][TIMER_INT_COMPARE_MATCH]();
    }
}

ISR(TIMER2_OVF_vect)
{
    if (Timer_CallBacks[TIMER_CHANNEL_2][TIMER_INT_OVERFLOW] != NULL)
    {
        Timer_CallBacks[TIMER_CHANNEL_2][TIMER_INT_OVERFLOW]();
    }
}

ISR(TIMER2_COMP_vect)
{
    if (Timer_CallBacks[TIMER_CHANNEL_2][TIMER_INT_COMPARE_MATCH] != NULL)
    {
        Timer_CallBacks[TIMER_CHANNEL_2][TIMER_INT_COMPARE_MATCH]();
    }
}
