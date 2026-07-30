#include "../../Service/STD_Types.h"
#include "../../Service/Bit_Math.h"
#include "spi_registers.h"
#include "spi_interface.h"

/* ================================================================================
 *  SPI DRIVER - IMPLEMENTATION
 * ============================================================================== */

#define DDRB_REG  (*(volatile u8 *)0x37)

static SPI_CallBackType SPI_CallBack = NULL;

void __vector_12(void) __attribute__((signal));

STD_ReturnType SPI_Init(const SPI_ConfigType *addConfig)
{
    STD_ReturnType local_Status = E_OK;

    if (addConfig == NULL)
    {
        local_Status = E_NOK;
    }
    else
    {
        if (addConfig->role == SPI_MASTER)
        {
            SET_BIT(DDRB_REG, SPI_MOSI_PIN);
            SET_BIT(DDRB_REG, SPI_SCK_PIN);
            SET_BIT(DDRB_REG, SPI_SS_PIN);
            CLR_BIT(DDRB_REG, SPI_MISO_PIN);

            SET_BIT(SPI_SPCR_REG, SPI_MSTR_BIT);

            if (GET_BIT(addConfig->clockRate, 0) == 1U)
            {
                SET_BIT(SPI_SPCR_REG, SPI_SPR0_BIT);
            }
            else
            {
                CLR_BIT(SPI_SPCR_REG, SPI_SPR0_BIT);
            }

            if (GET_BIT(addConfig->clockRate, 1) == 1U)
            {
                SET_BIT(SPI_SPCR_REG, SPI_SPR1_BIT);
            }
            else
            {
                CLR_BIT(SPI_SPCR_REG, SPI_SPR1_BIT);
            }

            if (GET_BIT(addConfig->clockRate, 2) == 1U)
            {
                SET_BIT(SPI_SPSR_REG, SPI_SPI2X_BIT);
            }
            else
            {
                CLR_BIT(SPI_SPSR_REG, SPI_SPI2X_BIT);
            }
        }
        else if (addConfig->role == SPI_SLAVE)
        {
            SET_BIT(DDRB_REG, SPI_MISO_PIN);
            CLR_BIT(DDRB_REG, SPI_MOSI_PIN);
            CLR_BIT(DDRB_REG, SPI_SCK_PIN);
            CLR_BIT(DDRB_REG, SPI_SS_PIN);

            CLR_BIT(SPI_SPCR_REG, SPI_MSTR_BIT);
        }
        else
        {
            local_Status = E_NOK;
        }

        if (local_Status == E_OK)
        {
            if (addConfig->polarity == SPI_CPOL_IDLE_HIGH)
            {
                SET_BIT(SPI_SPCR_REG, SPI_CPOL_BIT);
            }
            else
            {
                CLR_BIT(SPI_SPCR_REG, SPI_CPOL_BIT);
            }

            if (addConfig->phase == SPI_CPHA_SAMPLE_TRAILING)
            {
                SET_BIT(SPI_SPCR_REG, SPI_CPHA_BIT);
            }
            else
            {
                CLR_BIT(SPI_SPCR_REG, SPI_CPHA_BIT);
            }

            if (addConfig->dataOrder == SPI_LSB_FIRST)
            {
                SET_BIT(SPI_SPCR_REG, SPI_DORD_BIT);
            }
            else
            {
                CLR_BIT(SPI_SPCR_REG, SPI_DORD_BIT);
            }

            SET_BIT(SPI_SPCR_REG, SPI_SPE_BIT);
        }
    }

    return local_Status;
}

STD_ReturnType SPI_DeInit(void)
{
    CLR_BIT(SPI_SPCR_REG, SPI_SPE_BIT);
    CLR_BIT(SPI_SPCR_REG, SPI_SPIE_BIT);

    return E_OK;
}

STD_ReturnType SPI_Transceive(uint8_h txByte, uint8_h *puint8Rx)
{
    SPI_SPDR_REG = txByte;

    while (GET_BIT(SPI_SPSR_REG, SPI_SPIF_BIT) == 0U)
    {
        /* Busy wait */
    }

    if (puint8Rx != NULL)
    {
        *puint8Rx = SPI_SPDR_REG;
    }
    else
    {
        (void)SPI_SPDR_REG;
    }

    return E_OK;
}

STD_ReturnType SPI_SendByte(uint8_h txByte)
{
    return SPI_Transceive(txByte, NULL);
}

STD_ReturnType SPI_SendString(const uint8_h *pString)
{
    STD_ReturnType local_Status = E_OK;

    if (pString == NULL)
    {
        local_Status = E_NOK;
    }
    else
    {
        uint16_h i = 0U;
        while (pString[i] != '\0')
        {
            (void)SPI_SendByte(pString[i]);
            i++;
        }
    }

    return local_Status;
}

STD_ReturnType SPI_SetCallBack(SPI_CallBackType callBack)
{
    STD_ReturnType local_Status = E_OK;

    if (callBack == NULL)
    {
        local_Status = E_NOK;
    }
    else
    {
        SPI_CallBack = callBack;
        SET_BIT(SPI_SPCR_REG, SPI_SPIE_BIT);
    }

    return local_Status;
}

/* ================================================================================
 *  INTERRUPT SERVICE ROUTINE (ISR)
 * ============================================================================== */

void __vector_12(void)
{
    uint8_h receivedByte = SPI_SPDR_REG;

    if (SPI_CallBack != NULL)
    {
        SPI_CallBack(receivedByte);
    }
}