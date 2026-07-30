#include "../../Service/STD_Types.h"
#include "../../Service/Bit_Math.h"
#include "i2c_registers.h"
#include "i2c_interface.h"

/* ================================================================================
 *  I2C (TWI) DRIVER - IMPLEMENTATION
 * ============================================================================== */

STD_ReturnType I2C_InitMaster(const I2C_MasterConfigType *addConfig)
{
    STD_ReturnType local_Status = E_OK;

    if (addConfig == NULL)
    {
        local_Status = E_NOK;
    }
    else
    {
        /* Set prescaler to 1 (TWPS1:TWPS0 = 00) */
        CLR_BIT(I2C_TWSR_REG, I2C_TWPS0_BIT);
        CLR_BIT(I2C_TWSR_REG, I2C_TWPS1_BIT);

        /* Compute and load TWBR: TWBR = ((F_CPU / SCL_freq) - 16) / 2 */
        uint32_h twbrValue = ((I2C_F_CPU / addConfig->sclFrequency) - 16U) / 2U;
        I2C_TWBR_REG = (uint8_h)twbrValue;

        /* Enable TWI peripheral */
        SET_BIT(I2C_TWCR_REG, I2C_TWEN_BIT);
    }

    return local_Status;
}

STD_ReturnType I2C_InitSlave(const I2C_SlaveConfigType *addConfig)
{
    STD_ReturnType local_Status = E_OK;

    if (addConfig == NULL)
    {
        local_Status = E_NOK;
    }
    else
    {
        /* Set own slave address in upper 7 bits of TWAR */
        I2C_TWAR_REG = (uint8_h)(addConfig->ownAddress << 1U);

        /* Enable general call if configured */
        if (addConfig->enableGeneralCall != 0U)
        {
            SET_BIT(I2C_TWAR_REG, I2C_TWGCE_BIT);
        }

        /* Enable TWI and Enable ACK (and clear TWINT) */
        I2C_TWCR_REG = (1U << I2C_TWINT_BIT) | (1U << I2C_TWEN_BIT) | (1U << I2C_TWEA_BIT);
    }

    return local_Status;
}

STD_ReturnType I2C_DeInit(void)
{
    CLR_BIT(I2C_TWCR_REG, I2C_TWEN_BIT);
    return E_OK;
}

STD_ReturnType I2C_Start(void)
{
    STD_ReturnType local_Status = E_OK;

    /* Trigger START condition */
    I2C_TWCR_REG = (1U << I2C_TWINT_BIT) | (1U << I2C_TWSTA_BIT) | (1U << I2C_TWEN_BIT);

    /* Wait for TWINT flag set */
    while (GET_BIT(I2C_TWCR_REG, I2C_TWINT_BIT) == 0U)
    {
        /* Busy wait */
    }

    /* Check status */
    uint8_h status = I2C_GetStatus();
    if ((status != I2C_STATUS_START) && (status != I2C_STATUS_REP_START))
    {
        local_Status = E_NOK;
    }

    return local_Status;
}

STD_ReturnType I2C_Stop(void)
{
    /* Trigger STOP condition */
    I2C_TWCR_REG = (1U << I2C_TWINT_BIT) | (1U << I2C_TWSTO_BIT) | (1U << I2C_TWEN_BIT);

    return E_OK;
}

STD_ReturnType I2C_WriteByte(uint8_h uint8Data)
{
    I2C_TWDR_REG = uint8Data;

    /* Start transmission */
    I2C_TWCR_REG = (1U << I2C_TWINT_BIT) | (1U << I2C_TWEN_BIT);

    /* Wait for completion */
    while (GET_BIT(I2C_TWCR_REG, I2C_TWINT_BIT) == 0U)
    {
        /* Busy wait */
    }

    return E_OK;
}

STD_ReturnType I2C_ReadByteWithAck(uint8_h *puint8Data)
{
    STD_ReturnType local_Status = E_OK;

    if (puint8Data == NULL)
    {
        local_Status = E_NOK;
    }
    else
    {
        /* Enable TWI, clear TWINT, and send ACK */
        I2C_TWCR_REG = (1U << I2C_TWINT_BIT) | (1U << I2C_TWEN_BIT) | (1U << I2C_TWEA_BIT);

        /* Wait for completion */
        while (GET_BIT(I2C_TWCR_REG, I2C_TWINT_BIT) == 0U)
        {
            /* Busy wait */
        }

        *puint8Data = I2C_TWDR_REG;
    }

    return local_Status;
}

STD_ReturnType I2C_ReadByteWithNack(uint8_h *puint8Data)
{
    STD_ReturnType local_Status = E_OK;

    if (puint8Data == NULL)
    {
        local_Status = E_NOK;
    }
    else
    {
        /* Enable TWI, clear TWINT (without TWEA -> NACK) */
        I2C_TWCR_REG = (1U << I2C_TWINT_BIT) | (1U << I2C_TWEN_BIT);

        /* Wait for completion */
        while (GET_BIT(I2C_TWCR_REG, I2C_TWINT_BIT) == 0U)
        {
            /* Busy wait */
        }

        *puint8Data = I2C_TWDR_REG;
    }

    return local_Status;
}

uint8_h I2C_GetStatus(void)
{
    return (uint8_h)(I2C_TWSR_REG & I2C_TWSR_STATUS_MASK);
}

STD_ReturnType I2C_MasterWrite(uint8_h slaveAddress, const uint8_h *pData, uint16_h length)
{
    STD_ReturnType local_Status = E_OK;

    if (pData == NULL)
    {
        local_Status = E_NOK;
    }
    else
    {
        /* Send START */
        if (I2C_Start() != E_OK)
        {
            local_Status = E_NOK;
        }
        else
        {
            /* Send SLA + W (R/W bit = 0) */
            (void)I2C_WriteByte((uint8_h)(slaveAddress << 1U));
            if (I2C_GetStatus() != I2C_STATUS_MT_SLA_ACK)
            {
                (void)I2C_Stop();
                local_Status = E_NOK;
            }
            else
            {
                /* Send data bytes */
                for (uint16_h i = 0U; i < length; ++i)
                {
                    (void)I2C_WriteByte(pData[i]);
                    if (I2C_GetStatus() != I2C_STATUS_MT_DATA_ACK)
                    {
                        local_Status = E_NOK;
                        break;
                    }
                }
                (void)I2C_Stop();
            }
        }
    }

    return local_Status;
}

STD_ReturnType I2C_MasterRead(uint8_h slaveAddress, uint8_h *pBuffer, uint16_h length)
{
    STD_ReturnType local_Status = E_OK;

    if ((pBuffer == NULL) || (length == 0U))
    {
        local_Status = E_NOK;
    }
    else
    {
        /* Send START */
        if (I2C_Start() != E_OK)
        {
            local_Status = E_NOK;
        }
        else
        {
            /* Send SLA + R (R/W bit = 1) */
            (void)I2C_WriteByte((uint8_h)((slaveAddress << 1U) | 1U));
            if (I2C_GetStatus() != I2C_STATUS_MR_SLA_ACK)
            {
                (void)I2C_Stop();
                local_Status = E_NOK;
            }
            else
            {
                /* Read data bytes */
                for (uint16_h i = 0U; i < length; ++i)
                {
                    if (i == (length - 1U))
                    {
                        /* Last byte -> send NACK */
                        (void)I2C_ReadByteWithNack(&pBuffer[i]);
                    }
                    else
                    {
                        /* Intermediate bytes -> send ACK */
                        (void)I2C_ReadByteWithAck(&pBuffer[i]);
                    }
                }
                (void)I2C_Stop();
            }
        }
    }

    return local_Status;
}
