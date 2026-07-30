#include "../../Service/STD_Types.h"
#include "../../Service/Bit_Math.h"
#include "spi_registers.h"
#include "spi_interface.h"

/* ================================================================================
 *  SPI DRIVER - IMPLEMENTATION
 * ============================================================================== */

/* Register DDRB to set pin directions as mentioned in description */
#define DDRB_REG  (*(volatile u8 *)0x37)

/* Storage for the transfer-complete callback */
static SPI_CallBackType SPI_CallBack = NULL;

/* ISR Declaration for SPI Serial Transfer Complete */
void __vector_12(void) __attribute__((signal));

STD_ReturnType SPI_Init(const SPI_ConfigType *addConfig)
{
    STD_ReturnType local_Status = E_OK;

    /* STEP 1: Validate input configuration pointer */
    if (addConfig == NULL)
    {
        local_Status = E_NOK;
    }
    else
    {
        /* STEP 2: Configure SPI pin directions based on Role */
        if (addConfig->role == SPI_MASTER)
        {
            /* Master: MOSI (PB5), SCK (PB7), SS (PB4) as Output; MISO (PB6) as Input */
            SET_BIT(DDRB_REG, SPI_MOSI_PIN);
            SET_BIT(DDRB_REG, SPI_SCK_PIN);
            SET_BIT(DDRB_REG, SPI_SS_PIN);
            CLR_BIT(DDRB_REG, SPI_MISO_PIN);

            /* STEP 3: Select Master mode */
            SET_BIT(SPI_SPCR_REG, SPI_MSTR_BIT);

            /* STEP 7: Clock rate configuration (Master mode only) */
            /* SPR1:SPR0 bits (Low 2 bits of clockRate) */
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

            /* SPI2X bit (bit 2 of clockRate) */
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
            /* Slave: MISO (PB6) as Output; MOSI, SCK, SS as Input */
            SET_BIT(DDRB_REG, SPI_MISO_PIN);
            CLR_BIT(DDRB_REG, SPI_MOSI_PIN);
            CLR_BIT(DDRB_REG, SPI_SCK_PIN);
            CLR_BIT(DDRB_REG, SPI_SS_PIN);

            /* STEP 3: Select Slave mode */
            CLR_BIT(SPI_SPCR_REG, SPI_MSTR_BIT);
        }
        else
        {
            local_Status = E_NOK;
        }

        if (local_Status == E_OK)
        {
            /* STEP 4: Clock polarity (CPOL) */
            if (addConfig->polarity == SPI_CPOL_IDLE_HIGH)
            {
                SET_BIT(SPI_SPCR_REG, SPI_CPOL_BIT);
            }
            else
            {
                CLR_BIT(SPI_SPCR_REG, SPI_CPOL_BIT);
            }

            /* STEP 5: Clock phase (CPHA) */
            if (addConfig->phase == SPI_CPHA_SAMPLE_TRAILING)
            {
                SET_BIT(SPI_SPCR_REG, SPI_CPHA_BIT);
            }
            else
            {
                CLR_BIT(SPI_SPCR_REG, SPI_CPHA_BIT);
            }

            /* STEP 6: Data order (DORD) */
            if (addConfig->dataOrder == SPI_LSB_FIRST)
            {
                SET_BIT(SPI_SPCR_REG, SPI_DORD_BIT);
            }
            else
            {
                CLR_BIT(SPI_SPCR_REG, SPI_DORD_BIT);
            }

            /* STEP 8: Enable SPI Peripheral */
            SET_BIT(SPI_SPCR_REG, SPI_SPE_BIT);
        }
    }

    return local_Status;
}

STD_ReturnType SPI_DeInit(void)
{
    /* STEP 1: Disable SPI Module */
    CLR_BIT(SPI_SPCR_REG, SPI_SPE_BIT);

    /* STEP 2: Disable SPI Interrupt */
    CLR_BIT(SPI_SPCR_REG, SPI_SPIE_BIT);

    return E_OK;
}

STD_ReturnType SPI_Transceive(uint8_h txByte, uint8_h *puint8Rx)
{
    /* STEP 1: Write data to register to start transmission */
    SPI_SPDR_REG = txByte;

    /* STEP 2: Busy-wait until transfer complete flag (SPIF) is set */
    while (GET_BIT(SPI_SPSR_REG, SPI_SPIF_BIT) == 0U)
    {
        /* Busy wait */
    }

    /* STEP 3: Read received byte */
    if (puint8Rx != NULL)
    {
        *puint8Rx = SPI_SPDR_REG;
    }
    else
    {
        /* Dummy read to clear flag if pointer is NULL */
        (void)SPI_SPDR_REG;
    }

    return E_OK;
}

STD_ReturnType SPI_SendByte(uint8_h txByte)
{
    /* STEP 1: Call transceive and discard received byte */
    return SPI_Transceive(txByte, NULL);
}

STD_ReturnType SPI_SendString(const uint8_h *pString)
{
    STD_ReturnType local_Status = E_OK;

    /* STEP 1: Validate string pointer */
    if (pString == NULL)
    {
        local_Status = E_NOK;
    }
    else
    {
        /* STEP 2: Send string characters until null-terminator */
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

    /* STEP 1: Validate callback pointer */
    if (callBack == NULL)
    {
        local_Status = E_NOK;
    }
    else
    {
        /* STEP 2: Store callback function */
        SPI_CallBack = callBack;

        /* STEP 3: Enable SPI Interrupt */
        SET_BIT(SPI_SPCR_REG, SPI_SPIE_BIT);
    }

    return local_Status;
}

/* ================================================================================
 *  INTERRUPT SERVICE ROUTINE (ISR)
 * ============================================================================== */

/* ISR for SPI Serial Transfer Complete */
void __vector_12(void)
{
    uint8_h receivedByte = SPI_SPDR_REG;

    if (SPI_CallBack != NULL)
    {
        SPI_CallBack(receivedByte);
    }
}
