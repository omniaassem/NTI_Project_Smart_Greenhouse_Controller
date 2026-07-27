#include "../../Service/STD_Types.h"
#include "../../Service/Bit_Math.h"
#include "i2c_registers.h"
#include "i2c_interface.h"

/* ================================================================================
 *  I2C (TWI) DRIVER - IMPLEMENTATION SKELETON
 *  ------------------------------------------------------------------------------
 *  Each body lists the ordered steps to implement the function. Replace the
 *  numbered comments with the actual register manipulation code.
 *
 *  Common TWI idiom used throughout:
 *    - Start an operation by writing TWCR with TWINT=1 (clears the flag/kicks it
 *      off) plus whatever control bits are needed (TWEN, TWSTA, TWSTO, TWEA...).
 *    - Wait for completion by polling until TWINT becomes 1 again:
 *          while ( GET_BIT(I2C_TWCR_REG, I2C_TWINT_BIT) == 0 ) { }
 *    - Then inspect the status with (I2C_TWSR_REG & I2C_TWSR_STATUS_MASK).
 * ============================================================================== */


STD_ReturnType I2C_InitMaster(const I2C_MasterConfigType *addConfig)
{
    /*
     * STEP 1: Validate addConfig != NULL (else E_NOK).
     *
     * STEP 2: Set the prescaler to 1 by clearing TWPS1:TWPS0 in TWSR.
     *
     * STEP 3: Compute and load the bit-rate register (prescaler = 1):
     *         TWBR = (F_CPU / SCL_freq - 16) / 2
     *         I2C_TWBR_REG = (u8)TWBR;
     *
     * STEP 4: Enable the peripheral: SET_BIT(I2C_TWCR_REG, I2C_TWEN_BIT).
     *         (A pure master needs no own address; TWAR may be left at reset.)
     *
     * STEP 5: Return E_OK.
     */
    return E_NOK;
}


STD_ReturnType I2C_InitSlave(const I2C_SlaveConfigType *addConfig)
{
    /*
     * STEP 1: Validate addConfig != NULL (else E_NOK).
     *
     * STEP 2: Load the own address into the high 7 bits of TWAR:
     *         I2C_TWAR_REG = (u8)(addConfig->ownAddress << 1);
     *         If addConfig->enableGeneralCall is non-zero, also SET_BIT(TWAR, TWGCE).
     *
     * STEP 3: Enable the peripheral with acknowledge on:
     *         set TWEA and TWEN in TWCR (write TWINT=1 as well to clear the flag).
     *
     * STEP 4: Return E_OK.
     */
    return E_NOK;
}


STD_ReturnType I2C_DeInit(void)
{
    /*
     * STEP 1: Disable the peripheral: CLR_BIT(I2C_TWCR_REG, I2C_TWEN_BIT).
     * STEP 2: Return E_OK.
     */
    return E_NOK;
}


STD_ReturnType I2C_Start(void)
{
    /*
     * STEP 1: Trigger a START condition by writing TWCR with TWINT + TWSTA + TWEN set:
     *         I2C_TWCR_REG = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
     * STEP 2: Wait until TWINT is set again (operation complete).
     * STEP 3: Read the status; return E_OK if it is I2C_STATUS_START or
     *         I2C_STATUS_REP_START, otherwise E_NOK.
     */
    return E_NOK;
}


STD_ReturnType I2C_Stop(void)
{
    /*
     * STEP 1: Trigger a STOP condition by writing TWCR with TWINT + TWSTO + TWEN set:
     *         I2C_TWCR_REG = (1<<TWINT)|(1<<TWSTO)|(1<<TWEN);
     * STEP 2: (STOP has no TWINT completion flag; optionally spin until TWSTO clears.)
     * STEP 3: Return E_OK.
     */
    return E_NOK;
}


STD_ReturnType I2C_WriteByte(uint8_h uint8Data)
{
    /*
     * STEP 1: Load the byte into the data register: I2C_TWDR_REG = uint8Data;
     * STEP 2: Start transmission: write TWCR with TWINT + TWEN set (no TWSTA/TWSTO):
     *         I2C_TWCR_REG = (1<<TWINT)|(1<<TWEN);
     * STEP 3: Wait until TWINT is set again.
     * STEP 4: Return E_OK. (The caller checks I2C_GetStatus() for ACK/NACK when needed.)
     */
    return E_NOK;
}


STD_ReturnType I2C_ReadByteWithAck(uint8_h *puint8Data)
{
    /*
     * STEP 1: Validate puint8Data != NULL (else E_NOK).
     * STEP 2: Start a read that ACKs afterward: write TWCR with TWINT + TWEN + TWEA:
     *         I2C_TWCR_REG = (1<<TWINT)|(1<<TWEN)|(1<<TWEA);
     * STEP 3: Wait until TWINT is set again.
     * STEP 4: Read the received byte: *puint8Data = I2C_TWDR_REG;
     * STEP 5: Return E_OK.
     */
    return E_NOK;
}


STD_ReturnType I2C_ReadByteWithNack(uint8_h *puint8Data)
{
    /*
     * STEP 1: Validate puint8Data != NULL (else E_NOK).
     * STEP 2: Start a read that NACKs afterward: write TWCR with TWINT + TWEN
     *         (TWEA cleared): I2C_TWCR_REG = (1<<TWINT)|(1<<TWEN);
     * STEP 3: Wait until TWINT is set again.
     * STEP 4: Read the received byte: *puint8Data = I2C_TWDR_REG;
     * STEP 5: Return E_OK.
     */
    return E_NOK;
}


uint8_h I2C_GetStatus(void)
{
    /*
     * STEP 1: Return the masked status bits:
     *         return (uint8_h)(I2C_TWSR_REG & I2C_TWSR_STATUS_MASK);
     */
    return 0;
}


STD_ReturnType I2C_MasterWrite(uint8_h slaveAddress, const uint8_h *pData, uint16_h length)
{
    /*
     * STEP 1: Validate pData != NULL (else E_NOK).
     * STEP 2: I2C_Start(); if it fails, return E_NOK.
     * STEP 3: Send SLA+W: I2C_WriteByte( (slaveAddress << 1) | 0 );
     *         Then check I2C_GetStatus() == I2C_STATUS_MT_SLA_ACK, else STOP + E_NOK.
     * STEP 4: Loop over the buffer: for each byte call I2C_WriteByte(pData[i]) and
     *         confirm I2C_GetStatus() == I2C_STATUS_MT_DATA_ACK; on NACK -> STOP + E_NOK.
     * STEP 5: I2C_Stop();
     * STEP 6: Return E_OK.
     */
    return E_NOK;
}


STD_ReturnType I2C_MasterRead(uint8_h slaveAddress, uint8_h *pBuffer, uint16_h length)
{
    /*
     * STEP 1: Validate pBuffer != NULL and length > 0 (else E_NOK).
     * STEP 2: I2C_Start(); if it fails, return E_NOK.
     * STEP 3: Send SLA+R: I2C_WriteByte( (slaveAddress << 1) | 1 );
     *         Confirm I2C_GetStatus() == I2C_STATUS_MR_SLA_ACK, else STOP + E_NOK.
     * STEP 4: Read 'length' bytes:
     *         - For all but the LAST byte, use I2C_ReadByteWithAck(&pBuffer[i]).
     *         - For the LAST byte, use I2C_ReadByteWithNack(&pBuffer[length-1]).
     * STEP 5: I2C_Stop();
     * STEP 6: Return E_OK.
     */
    return E_NOK;
}
