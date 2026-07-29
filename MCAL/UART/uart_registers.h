#ifndef UART_REGISTERS_H
#define UART_REGISTERS_H

#include "../../Service/STD_Types.h"

/* ================================================================================
 *  ATmega32 USART REGISTER MAP
 *  ------------------------------------------------------------------------------
 *  The ATmega32 has a single USART. Note the register-sharing quirk:
 *  UCSRC and UBRRH occupy the SAME I/O address (0x40). The URSEL bit (bit 7) of
 *  the written value selects which one is being addressed:
 *      - Write with URSEL = 1 -> the write targets UCSRC.
 *      - Write with URSEL = 0 -> the write targets UBRRH.
 * ============================================================================== */

/* ---------------- Data / Control / Status Registers ---------------- */
#define UART_UDR_REG       (*(volatile u8 *)0x2C)   /* USART I/O Data Register            */
#define UART_UCSRA_REG     (*(volatile u8 *)0x2B)   /* USART Control & Status Register A  */
#define UART_UCSRB_REG     (*(volatile u8 *)0x2A)   /* USART Control & Status Register B  */
#define UART_UCSRC_REG     (*(volatile u8 *)0x40)   /* USART Control & Status Register C  */
#define UART_UBRRL_REG     (*(volatile u8 *)0x29)   /* USART Baud Rate Register - Low     */
#define UART_UBRRH_REG     (*(volatile u8 *)0x40)   /* USART Baud Rate Register - High (shared w/ UCSRC) */

/* ---------------- UCSRA bit positions ---------------- */
#define UART_MPCM_BIT      0    /* Multi-processor Communication Mode */
#define UART_U2X_BIT       1    /* Double the USART Transmission Speed */
#define UART_PE_BIT        2    /* Parity Error                       */
#define UART_DOR_BIT       3    /* Data OverRun                       */
#define UART_FE_BIT        4    /* Frame Error                        */
#define UART_UDRE_BIT      5    /* USART Data Register Empty          */
#define UART_TXC_BIT       6    /* USART Transmit Complete            */
#define UART_RXC_BIT       7    /* USART Receive Complete             */

/* ---------------- UCSRB bit positions ---------------- */
#define UART_TXB8_BIT      0    /* Transmit Data Bit 8                */
#define UART_RXB8_BIT      1    /* Receive Data Bit 8                 */
#define UART_UCSZ2_BIT     2    /* Character Size bit 2               */
#define UART_TXEN_BIT      3    /* Transmitter Enable                 */
#define UART_RXEN_BIT      4    /* Receiver Enable                    */
#define UART_UDRIE_BIT     5    /* USART Data Register Empty Int En   */
#define UART_TXCIE_BIT     6    /* TX Complete Interrupt Enable       */
#define UART_RXCIE_BIT     7    /* RX Complete Interrupt Enable       */

/* ---------------- UCSRC bit positions ---------------- */
#define UART_UCPOL_BIT     0    /* Clock Polarity (synchronous mode)  */
#define UART_UCSZ0_BIT     1    /* Character Size bit 0               */
#define UART_UCSZ1_BIT     2    /* Character Size bit 1               */
#define UART_USBS_BIT      3    /* Stop Bit Select                    */
#define UART_UPM0_BIT      4    /* Parity Mode bit 0                  */
#define UART_UPM1_BIT      5    /* Parity Mode bit 1                  */
#define UART_UMSEL_BIT     6    /* USART Mode Select (0=async,1=sync) */
#define UART_URSEL_BIT     7    /* Register Select (UCSRC vs UBRRH)   */

#define UART_GLOBAL_INT_BIT   7    /* I-bit inside SREG */
#define UART_SREG_REG      (*(volatile u8 *)0x5F)   /* Status Register */

#endif /* UART_REGISTERS_H */
