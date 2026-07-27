#ifndef TIMER_REGISTERS_H
#define TIMER_REGISTERS_H

#include "../../Service/STD_Types.h"

/* ================================================================================
 *  ATmega32 TIMER / COUNTER REGISTER MAP
 *  ------------------------------------------------------------------------------
 *  All addresses below are the data-memory addresses (I/O address + 0x20).
 *  The device has three timer/counter units:
 *      - Timer0 : 8-bit
 *      - Timer1 : 16-bit (two compare units A/B + input capture)
 *      - Timer2 : 8-bit (can also run asynchronously from a 32.768kHz crystal)
 * ============================================================================== */

/* ---------------- Timer0 (8-bit) ---------------- */
#define TIMER_TCCR0_REG    (*(volatile u8 *)0x53)   /* Timer0 Control Register            */
#define TIMER_TCNT0_REG    (*(volatile u8 *)0x52)   /* Timer0 Counter Value Register      */
#define TIMER_OCR0_REG     (*(volatile u8 *)0x5C)   /* Timer0 Output Compare Register     */

/* TCCR0 bit positions */
#define TIMER_CS00_BIT     0    /* Clock Select bit 0 (CS02:CS00 = prescaler) */
#define TIMER_CS01_BIT     1    /* Clock Select bit 1                         */
#define TIMER_CS02_BIT     2    /* Clock Select bit 2                         */
#define TIMER_WGM01_BIT    3    /* Waveform Generation Mode bit 1             */
#define TIMER_COM00_BIT    4    /* Compare Output Mode bit 0                  */
#define TIMER_COM01_BIT    5    /* Compare Output Mode bit 1                  */
#define TIMER_WGM00_BIT    6    /* Waveform Generation Mode bit 0             */
#define TIMER_FOC0_BIT     7    /* Force Output Compare                       */

/* ---------------- Timer1 (16-bit) ---------------- */
#define TIMER_TCCR1A_REG   (*(volatile u8  *)0x4F)  /* Timer1 Control Register A          */
#define TIMER_TCCR1B_REG   (*(volatile u8  *)0x4E)  /* Timer1 Control Register B          */
#define TIMER_TCNT1_REG    (*(volatile u16 *)0x4C)  /* Timer1 Counter (16-bit combined)   */
#define TIMER_TCNT1L_REG   (*(volatile u8  *)0x4C)  /* Timer1 Counter low byte            */
#define TIMER_TCNT1H_REG   (*(volatile u8  *)0x4D)  /* Timer1 Counter high byte           */
#define TIMER_OCR1A_REG    (*(volatile u16 *)0x4A)  /* Timer1 Output Compare A (16-bit)   */
#define TIMER_OCR1B_REG    (*(volatile u16 *)0x48)  /* Timer1 Output Compare B (16-bit)   */
#define TIMER_ICR1_REG     (*(volatile u16 *)0x46)  /* Timer1 Input Capture (16-bit)      */

/* TCCR1A bit positions */
#define TIMER_WGM10_BIT    0
#define TIMER_WGM11_BIT    1
#define TIMER_FOC1B_BIT    2
#define TIMER_FOC1A_BIT    3
#define TIMER_COM1B0_BIT   4
#define TIMER_COM1B1_BIT   5
#define TIMER_COM1A0_BIT   6
#define TIMER_COM1A1_BIT   7

/* TCCR1B bit positions */
#define TIMER_CS10_BIT     0    /* Clock Select bit 0 (CS12:CS10 = prescaler) */
#define TIMER_CS11_BIT     1
#define TIMER_CS12_BIT     2
#define TIMER_WGM12_BIT    3
#define TIMER_WGM13_BIT    4
#define TIMER_ICES1_BIT    6    /* Input Capture Edge Select                  */
#define TIMER_ICNC1_BIT    7    /* Input Capture Noise Canceler               */

/* ---------------- Timer2 (8-bit) ---------------- */
#define TIMER_TCCR2_REG    (*(volatile u8 *)0x45)   /* Timer2 Control Register            */
#define TIMER_TCNT2_REG    (*(volatile u8 *)0x44)   /* Timer2 Counter Value Register      */
#define TIMER_OCR2_REG     (*(volatile u8 *)0x43)   /* Timer2 Output Compare Register     */
#define TIMER_ASSR_REG     (*(volatile u8 *)0x42)   /* Timer2 Asynchronous Status Register*/

/* TCCR2 bit positions */
#define TIMER_CS20_BIT     0    /* Clock Select bit 0 (CS22:CS20 = prescaler) */
#define TIMER_CS21_BIT     1
#define TIMER_CS22_BIT     2
#define TIMER_WGM21_BIT    3
#define TIMER_COM20_BIT    4
#define TIMER_COM21_BIT    5
#define TIMER_WGM20_BIT    6
#define TIMER_FOC2_BIT     7

/* ---------------- Shared Interrupt Registers ---------------- */
#define TIMER_TIMSK_REG    (*(volatile u8 *)0x59)   /* Timer/Counter Interrupt Mask Register  */
#define TIMER_TIFR_REG     (*(volatile u8 *)0x58)   /* Timer/Counter Interrupt Flag Register  */
#define TIMER_SREG_REG     (*(volatile u8 *)0x5F)   /* Status Register (global interrupt bit) */

/* TIMSK bit positions */
#define TIMER_TOIE0_BIT    0    /* Timer0 Overflow Interrupt Enable        */
#define TIMER_OCIE0_BIT    1    /* Timer0 Output Compare Match Int Enable   */
#define TIMER_TOIE1_BIT    2    /* Timer1 Overflow Interrupt Enable        */
#define TIMER_OCIE1B_BIT   3    /* Timer1 Compare B Match Interrupt Enable  */
#define TIMER_OCIE1A_BIT   4    /* Timer1 Compare A Match Interrupt Enable  */
#define TIMER_TICIE1_BIT   5    /* Timer1 Input Capture Interrupt Enable    */
#define TIMER_TOIE2_BIT    6    /* Timer2 Overflow Interrupt Enable        */
#define TIMER_OCIE2_BIT    7    /* Timer2 Output Compare Match Int Enable   */

/* TIFR bit positions (write 1 to clear the flag) */
#define TIMER_TOV0_BIT     0    /* Timer0 Overflow Flag        */
#define TIMER_OCF0_BIT     1    /* Timer0 Output Compare Flag   */
#define TIMER_TOV1_BIT     2    /* Timer1 Overflow Flag        */
#define TIMER_OCF1B_BIT    3    /* Timer1 Compare B Flag        */
#define TIMER_OCF1A_BIT    4    /* Timer1 Compare A Flag        */
#define TIMER_ICF1_BIT     5    /* Timer1 Input Capture Flag    */
#define TIMER_TOV2_BIT     6    /* Timer2 Overflow Flag        */
#define TIMER_OCF2_BIT     7    /* Timer2 Output Compare Flag   */

#define TIMER_GLOBAL_INT_BIT   7    /* I-bit inside SREG (global interrupt enable) */

/* Resolution helpers */
#define TIMER0_MAX_COUNT   255U      /* 8-bit timer top value  */
#define TIMER1_MAX_COUNT   65535U    /* 16-bit timer top value */
#define TIMER2_MAX_COUNT   255U      /* 8-bit timer top value  */

#endif /* TIMER_REGISTERS_H */
