#ifndef SPI_REGISTERS_H
#define SPI_REGISTERS_H

#include "../../Service/STD_Types.h"

/* ================================================================================
 *  ATmega32 SPI REGISTER MAP
 *  ------------------------------------------------------------------------------
 *  SPI pins are fixed on PORTB:
 *      - SS   = PB4   - MOSI = PB5
 *      - MISO = PB6   - SCK  = PB7
 *  In master mode SS/MOSI/SCK are outputs and MISO is input; in slave mode the
 *  directions are reversed. The GPIO driver is responsible for the pin directions.
 * ============================================================================== */

/* ---------------- Control / Status / Data Registers ---------------- */
#define SPI_SPCR_REG       (*(volatile u8 *)0x2D)   /* SPI Control Register */
#define SPI_SPSR_REG       (*(volatile u8 *)0x2E)   /* SPI Status Register  */
#define SPI_SPDR_REG       (*(volatile u8 *)0x2F)   /* SPI Data Register    */

/* ---------------- SPCR bit positions ---------------- */
#define SPI_SPR0_BIT       0    /* SPI Clock Rate Select bit 0 */
#define SPI_SPR1_BIT       1    /* SPI Clock Rate Select bit 1 */
#define SPI_CPHA_BIT       2    /* Clock Phase                 */
#define SPI_CPOL_BIT       3    /* Clock Polarity              */
#define SPI_MSTR_BIT       4    /* Master/Slave Select         */
#define SPI_DORD_BIT       5    /* Data Order (0=MSB first)    */
#define SPI_SPE_BIT        6    /* SPI Enable                  */
#define SPI_SPIE_BIT       7    /* SPI Interrupt Enable        */

/* ---------------- SPSR bit positions ---------------- */
#define SPI_SPI2X_BIT      0    /* Double SPI Speed            */
#define SPI_WCOL_BIT       6    /* Write Collision Flag        */
#define SPI_SPIF_BIT       7    /* SPI Interrupt (transfer complete) Flag */

/* ---------------- SPI pin mapping (PORTB) ---------------- */
#define SPI_SS_PIN         4
#define SPI_MOSI_PIN       5
#define SPI_MISO_PIN       6
#define SPI_SCK_PIN        7

#define SPI_GLOBAL_INT_BIT    7    /* I-bit inside SREG */
#define SPI_SREG_REG       (*(volatile u8 *)0x5F)   /* Status Register */

#endif /* SPI_REGISTERS_H */
