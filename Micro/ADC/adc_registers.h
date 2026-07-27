#ifndef ADC_REGISTER_H_
#define ADC_REGISTER_H_

/*                             ADC Registers Addresses                           */
/* ADC Multiplexer Selection Register */
#define ADMUX       (*((volatile u8*)0x27))

/* ADC Control and Status Register A */
#define ADCSRA      (*((volatile u8*)0x26))

/* ADC Data Register High Byte */
#define ADCH        (*((volatile u8*)0x25))

/* ADC Data Register Low Byte */
#define ADCL        (*((volatile u8*)0x24))

/* Combined 16-bit ADC Data Register for reading both ADCL & ADCH at once */
#define ADC_DATA    (*((volatile u16*)0x24))

/* Special Function IO Register (Contains Auto Trigger Source Bits) */
#define SFIOR       (*((volatile u8*)0x50))

/*                             Registers Bits Definition                         */

/* ADMUX Bits Definition */
#define ADMUX_MUX0      0   /* Analog Channel and Gain Selection Bit 0 */
#define ADMUX_MUX1      1   /* Analog Channel and Gain Selection Bit 1 */
#define ADMUX_MUX2      2   /* Analog Channel and Gain Selection Bit 2 */
#define ADMUX_MUX3      3   /* Analog Channel and Gain Selection Bit 3 */
#define ADMUX_MUX4      4   /* Analog Channel and Gain Selection Bit 4 */
#define ADMUX_ADLAR     5   /* ADC Left Adjust Result */
#define ADMUX_REFS0     6   /* Reference Selection Bit 0 */
#define ADMUX_REFS1     7   /* Reference Selection Bit 1 */

/* ADCSRA Bits Definition */
#define ADCSRA_ADPS0    0   /* ADC Prescaler Select Bit 0 */
#define ADCSRA_ADPS1    1   /* ADC Prescaler Select Bit 1 */
#define ADCSRA_ADPS2    2   /* ADC Prescaler Select Bit 2 */
#define ADCSRA_ADIE     3   /* ADC Interrupt Enable */
#define ADCSRA_ADIF     4   /* ADC Interrupt Flag */
#define ADCSRA_ATE      5   /* ADC Auto Trigger Enable */
#define ADCSRA_ADSC     6   /* ADC Start Conversion */
#define ADCSRA_ADEN     7   /* ADC Enable */

/* SFIOR Bits Definition (Auto Trigger Source Bits) */
#define SFIOR_ADTS0     5   /* ADC Auto Trigger Source Bit 0 */
#define SFIOR_ADTS1     6   /* ADC Auto Trigger Source Bit 1 */
#define SFIOR_ADTS2     7   /* ADC Auto Trigger Source Bit 2 */

#endif /* ADC_REGISTER_H_ */