#ifndef ADC_REGISTERS_H
#define ADC_REGISTERS_H

#include "../../Service/STD_Types.h"

/* ---------------- ADC Registers (ATmega32 memory-mapped addresses) ---------------- */
#define ADC_ADMUX_REG      (*(volatile u8  *)0x27)   /* ADC Multiplexer Selection Register       */
#define ADC_ADCSRA_REG     (*(volatile u8  *)0x26)   /* ADC Control and Status Register A        */
#define ADC_ADCH_REG       (*(volatile u8  *)0x25)   /* ADC Data Register - High byte             */
#define ADC_ADCL_REG       (*(volatile u8  *)0x24)   /* ADC Data Register - Low byte              */
#define ADC_ADCDATA_REG    (*(volatile u16 *)0x24)   /* ADC Data Register - combined 16-bit view  */
#define ADC_SFIOR_REG      (*(volatile u8  *)0x50)   /* Special Function IO Register (trigger src)*/

/* ---------------- ADMUX bit positions ---------------- */
#define ADC_MUX0_BIT       0     /* MUX0 - bit 0 of the 5-bit channel selector (MUX4:MUX0) */
#define ADC_MUX1_BIT       1
#define ADC_MUX2_BIT       2
#define ADC_MUX3_BIT       3
#define ADC_MUX4_BIT       4
#define ADC_ADLAR_BIT      5     /* ADC Left Adjust Result                                  */
#define ADC_REFS0_BIT      6     /* Reference Selection bit 0                               */
#define ADC_REFS1_BIT      7     /* Reference Selection bit 1                               */

/* ---------------- ADCSRA bit positions ---------------- */
#define ADC_ADPS0_BIT      0     /* ADC Prescaler Select bit 0 (ADPS2:ADPS0)  */
#define ADC_ADPS1_BIT      1
#define ADC_ADPS2_BIT      2
#define ADC_ADATE_BIT      5     /* ADC Auto Trigger Enable                    */
#define ADC_ADSC_BIT       6     /* ADC Start Conversion                       */
#define ADC_ADEN_BIT       7     /* ADC Enable                                 */

/* ---------------- SFIOR bit positions (Auto Trigger Source select) ---------------- */
#define ADC_ADTS0_BIT      5
#define ADC_ADTS1_BIT      6
#define ADC_ADTS2_BIT      7

#define ADC_NUMBER_OF_CHANNELS   8

#endif /* ADC_REGISTERS_H */
