#include "../../Service/STD_Types.h"
#include "../../Service/Bit_Math.h"
#include "spi_registers.h"
#include "spi_interface.h"

/* ================================================================================
 *  SPI DRIVER - IMPLEMENTATION SKELETON
 *  ------------------------------------------------------------------------------
 *  Each body lists the ordered steps to implement the function. Replace the
 *  numbered comments with the actual register manipulation code.
 * ============================================================================== */

/*
 * Storage for the transfer-complete callback.
 * TODO: static SPI_CallBackType SPI_CallBack = NULL;
 */


STD_ReturnType SPI_Init(const SPI_ConfigType *addConfig)
{
    /*
     * STEP 1: Validate addConfig != NULL (else E_NOK).
     *
     * STEP 2: Set the SPI pin directions (via the GPIO driver or DDRB directly):
     *   - Master role: MOSI (PB5), SCK (PB7), SS (PB4) as OUTPUT; MISO (PB6) as INPUT.
     *   - Slave  role: MISO (PB6) as OUTPUT; MOSI, SCK, SS as INPUT.
     *
     * STEP 3: Select master/slave in SPCR:
     *   - role == SPI_MASTER -> SET_BIT(SPCR, SPI_MSTR_BIT)
     *   - role == SPI_SLAVE  -> CLR_BIT(SPCR, SPI_MSTR_BIT)
     *
     * STEP 4: Clock polarity: copy addConfig->polarity into CPOL bit of SPCR.
     * STEP 5: Clock phase:    copy addConfig->phase    into CPHA bit of SPCR.
     * STEP 6: Data order:     copy addConfig->dataOrder into DORD bit of SPCR.
     *
     * STEP 7: Clock rate (master only). The clockRate value encodes SPI2X:SPR1:SPR0:
     *   - Write SPR1:SPR0 (low 2 bits) into SPCR.
     *   - Write SPI2X (bit 2 of clockRate) into SPSR.
     *
     * STEP 8: Enable the peripheral: SET_BIT(SPCR, SPI_SPE_BIT).
     * STEP 9: Return E_OK.
     */
    return E_NOK;
}


STD_ReturnType SPI_DeInit(void)
{
    /*
     * STEP 1: Disable the module: CLR_BIT(SPI_SPCR_REG, SPI_SPE_BIT).
     * STEP 2: Optionally clear SPIE to disable the SPI interrupt.
     * STEP 3: Return E_OK.
     */
    return E_NOK;
}


STD_ReturnType SPI_Transceive(uint8_h txByte, uint8_h *puint8Rx)
{
    /*
     * STEP 1: Start the transfer by writing the byte to the data register:
     *         SPI_SPDR_REG = txByte;
     * STEP 2: Busy-wait for the transfer-complete flag:
     *         while ( GET_BIT(SPI_SPSR_REG, SPI_SPIF_BIT) == 0 ) { }
     *         (Reading SPSR then SPDR clears SPIF.)
     * STEP 3: Read the received byte:
     *         if (puint8Rx != NULL) *puint8Rx = SPI_SPDR_REG;
     *         else                  (void)SPI_SPDR_REG;   // still read to clear flag
     * STEP 4: Return E_OK.
     */
    return E_NOK;
}


STD_ReturnType SPI_SendByte(uint8_h txByte)
{
    /*
     * STEP 1: Call SPI_Transceive(txByte, NULL) and return its status.
     */
    return E_NOK;
}


STD_ReturnType SPI_SendString(const uint8_h *pString)
{
    /*
     * STEP 1: Validate pString != NULL (else E_NOK).
     * STEP 2: Loop over the string until '\0', calling SPI_SendByte() for each char.
     * STEP 3: Return E_OK.
     */
    return E_NOK;
}


STD_ReturnType SPI_SetCallBack(SPI_CallBackType callBack)
{
    /*
     * STEP 1: Validate callBack != NULL (else E_NOK).
     * STEP 2: Store it: SPI_CallBack = callBack;
     * STEP 3: Enable the SPI interrupt: SET_BIT(SPI_SPCR_REG, SPI_SPIE_BIT).
     * STEP 4: Return E_OK.
     *
     * NOTE: Provide the ISR here:
     *       ISR(SPI_STC_vect){ uint8_h d = SPI_SPDR_REG;
     *                          if (SPI_CallBack) SPI_CallBack(d); }
     */
    return E_NOK;
}
