#ifndef I2C_REGISTERS_H
#define I2C_REGISTERS_H

#include "../../Service/STD_Types.h"

/* ================================================================================
 *  ATmega32 TWI (I2C) REGISTER MAP
 *  ------------------------------------------------------------------------------
 *  The Two-Wire Interface (TWI) is Atmel's name for I2C. Bus pins are fixed:
 *      - SCL = PC0   - SDA = PC1
 *  Both lines require external pull-up resistors. After every bus operation the
 *  hardware writes a status code into the top 5 bits of TWSR (mask with 0xF8).
 * ============================================================================== */

/* ---------------- Control / Status / Data / Address Registers ---------------- */
#define I2C_TWBR_REG       (*(volatile u8 *)0x20)   /* TWI Bit Rate Register        */
#define I2C_TWSR_REG       (*(volatile u8 *)0x21)   /* TWI Status Register          */
#define I2C_TWAR_REG       (*(volatile u8 *)0x22)   /* TWI (own) Slave Address Reg  */
#define I2C_TWDR_REG       (*(volatile u8 *)0x23)   /* TWI Data Register            */
#define I2C_TWCR_REG       (*(volatile u8 *)0x56)   /* TWI Control Register         */

/* ---------------- TWCR bit positions ---------------- */
#define I2C_TWIE_BIT       0    /* TWI Interrupt Enable         */
#define I2C_TWEN_BIT       2    /* TWI Enable                   */
#define I2C_TWWC_BIT       3    /* TWI Write Collision Flag     */
#define I2C_TWSTO_BIT      4    /* TWI STOP Condition           */
#define I2C_TWSTA_BIT      5    /* TWI START Condition          */
#define I2C_TWEA_BIT       6    /* TWI Enable Acknowledge       */
#define I2C_TWINT_BIT      7    /* TWI Interrupt Flag           */

/* ---------------- TWSR bit positions (prescaler) ---------------- */
#define I2C_TWPS0_BIT      0    /* TWI Prescaler bit 0 */
#define I2C_TWPS1_BIT      1    /* TWI Prescaler bit 1 */
#define I2C_TWSR_STATUS_MASK   0xF8   /* Mask keeping only the 5 status bits */

/* ---------------- TWAR bit position ---------------- */
#define I2C_TWGCE_BIT      0    /* TWI General Call Recognition Enable */

/* ---------------- Standard TWI Status Codes (TWSR & 0xF8) ---------------- */
#define I2C_STATUS_START            0x08   /* START transmitted                          */
#define I2C_STATUS_REP_START        0x10   /* Repeated START transmitted                 */
#define I2C_STATUS_MT_SLA_ACK       0x18   /* SLA+W transmitted, ACK received            */
#define I2C_STATUS_MT_SLA_NACK      0x20   /* SLA+W transmitted, NACK received           */
#define I2C_STATUS_MT_DATA_ACK      0x28   /* Data byte transmitted, ACK received        */
#define I2C_STATUS_MT_DATA_NACK     0x30   /* Data byte transmitted, NACK received       */
#define I2C_STATUS_MR_SLA_ACK       0x40   /* SLA+R transmitted, ACK received            */
#define I2C_STATUS_MR_SLA_NACK      0x48   /* SLA+R transmitted, NACK received           */
#define I2C_STATUS_MR_DATA_ACK      0x50   /* Data byte received, ACK returned           */
#define I2C_STATUS_MR_DATA_NACK     0x58   /* Data byte received, NACK returned          */

#define I2C_GLOBAL_INT_BIT    7    /* I-bit inside SREG */
#define I2C_SREG_REG       (*(volatile u8 *)0x5F)   /* Status Register */

#endif /* I2C_REGISTERS_H */
