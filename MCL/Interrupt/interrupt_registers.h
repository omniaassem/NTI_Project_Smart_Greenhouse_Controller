#ifndef INTERRUPT_REGISTERS_H
#define INTERRUPT_REGISTERS_H

#include "../../Service/STD_Types.h"

/* ================================================================================
 *  ATmega32 EXTERNAL INTERRUPT REGISTER MAP
 *  ------------------------------------------------------------------------------
 *  The ATmega32 exposes three external interrupt lines:
 *      - INT0 : pin PD2  (edge/level configurable via ISC01:ISC00 in MCUCR)
 *      - INT1 : pin PD3  (edge/level configurable via ISC11:ISC10 in MCUCR)
 *      - INT2 : pin PB2  (edge-only, configurable via ISC2 in MCUCSR)
 * ============================================================================== */

/* ---------------- Control / Status Registers ---------------- */
#define EXTI_MCUCR_REG     (*(volatile u8 *)0x55)   /* MCU Control Register  - ISC bits for INT0/INT1 */
#define EXTI_MCUCSR_REG    (*(volatile u8 *)0x54)   /* MCU Ctrl & Status Reg - ISC2 for INT2          */
#define EXTI_GICR_REG      (*(volatile u8 *)0x5B)   /* General Interrupt Control Reg - enable bits     */
#define EXTI_GIFR_REG      (*(volatile u8 *)0x5A)   /* General Interrupt Flag Reg - pending flags      */
#define EXTI_SREG_REG      (*(volatile u8 *)0x5F)   /* Status Register - global interrupt (I) bit      */

/* ---------------- MCUCR bit positions (INT0 / INT1 sense control) ---------------- */
#define EXTI_ISC00_BIT     0    /* Interrupt Sense Control INT0 bit 0 */
#define EXTI_ISC01_BIT     1    /* Interrupt Sense Control INT0 bit 1 */
#define EXTI_ISC10_BIT     2    /* Interrupt Sense Control INT1 bit 0 */
#define EXTI_ISC11_BIT     3    /* Interrupt Sense Control INT1 bit 1 */

/* ---------------- MCUCSR bit position (INT2 sense control) ---------------- */
#define EXTI_ISC2_BIT      6    /* Interrupt Sense Control INT2 (0=falling, 1=rising) */

/* ---------------- GICR bit positions (interrupt enables) ---------------- */
#define EXTI_INT2_BIT      5    /* External Interrupt Request 2 Enable */
#define EXTI_INT0_BIT      6    /* External Interrupt Request 0 Enable */
#define EXTI_INT1_BIT      7    /* External Interrupt Request 1 Enable */

/* ---------------- GIFR bit positions (pending flags, write 1 to clear) ---------------- */
#define EXTI_INTF2_BIT     5    /* External Interrupt Flag 2 */
#define EXTI_INTF0_BIT     6    /* External Interrupt Flag 0 */
#define EXTI_INTF1_BIT     7    /* External Interrupt Flag 1 */

#define EXTI_GLOBAL_INT_BIT   7   /* I-bit inside SREG (global interrupt enable) */

#endif /* INTERRUPT_REGISTERS_H */
