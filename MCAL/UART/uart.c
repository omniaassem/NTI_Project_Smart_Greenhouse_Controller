#include "../../Service/STD_Types.h"
#include "../../Service/Bit_Math.h"
#include <avr/interrupt.h>
#include "uart_registers.h"
#include "uart_interface.h"

/* ================================================================================
 *  UART (USART) DRIVER - IMPLEMENTATION (ATmega32)
 *  ------------------------------------------------------------------------------
 *  Each body keeps the ordered steps it implements as comments, followed by the
 *  actual register manipulation code.
 * ============================================================================== */

/*
 * Storage for the RX-complete callback.
 */
static UART_RxCallBackType UART_RxCallBack = NULL;


STD_ReturnType UART_Init(const UART_ConfigType *addConfig)
{
    uint16_h local_UBRR  = 0U;
    uint8_h  local_UCSRC = 0U;

    /*
     * STEP 1: Validate the input.
     *   - If addConfig == NULL, return E_NOK.
     */
    if (addConfig == NULL)
    {
        return E_NOK;
    }

    /*
     * STEP 2: Compute the baud rate register value (normal speed, U2X = 0):
     *         UBRR = (UART_F_CPU / (16 * baudRate)) - 1
     *         Use a local uint16_h to hold the result.
     */
    if (addConfig->baudRate == 0UL)
    {
        return E_NOK;
    }
    local_UBRR = (uint16_h)((UART_F_CPU / (16UL * addConfig->baudRate)) - 1UL);

    /*
     * STEP 3: Load the baud value into the baud registers:
     *   - High byte: write UBRRH with URSEL = 0 (so the write targets UBRRH, not UCSRC),
     *                i.e. UART_UBRRH_REG = (u8)(UBRR >> 8) & 0x0F;   (URSEL bit stays 0)
     *   - Low  byte: UART_UBRRL_REG = (u8)UBRR;
     */
    UART_UBRRH_REG = (u8)((local_UBRR >> 8) & 0x0FU);
    UART_UBRRL_REG = (u8)local_UBRR;

    /*
     * STEP 4: Configure the frame format in UCSRC. Because UCSRC shares the address
     *         with UBRRH, every write MUST set URSEL (bit 7) = 1. Build the value:
     *   - Set URSEL.
     *   - Async mode: clear UMSEL.
     *   - Parity  (UPM1:UPM0) from addConfig->parity.
     *   - Stop    (USBS)      from addConfig->stopBits.
     *   - Data    (UCSZ1:UCSZ0) from the low 2 bits of addConfig->dataSize; if
     *             9-bit is requested also set UCSZ2 in UCSRB (STEP 5).
     *   Then write the assembled byte to UART_UCSRC_REG in ONE store.
     */
    SET_BIT(local_UCSRC, UART_URSEL_BIT);                                     /* target UCSRC      */
    CLR_BIT(local_UCSRC, UART_UMSEL_BIT);                                     /* asynchronous mode */
    local_UCSRC |= (u8)(((u8)addConfig->parity   & 0x03U) << UART_UPM0_BIT);  /* UPM1:UPM0         */
    local_UCSRC |= (u8)(((u8)addConfig->stopBits & 0x01U) << UART_USBS_BIT);  /* USBS              */
    local_UCSRC |= (u8)(((u8)addConfig->dataSize & 0x03U) << UART_UCSZ0_BIT); /* UCSZ1:UCSZ0       */

    UART_UCSRC_REG = local_UCSRC;

    /*
     * STEP 5: Configure UCSRB:
     *   - Set UCSZ2 only for 9-bit data, otherwise clear it.
     *   - Enable transmitter (TXEN) and receiver (RXEN).
     */
    if (addConfig->dataSize == UART_DATA_9BITS)
    {
        SET_BIT(UART_UCSRB_REG, UART_UCSZ2_BIT);
    }
    else
    {
        CLR_BIT(UART_UCSRB_REG, UART_UCSZ2_BIT);
    }

    SET_BIT(UART_UCSRB_REG, UART_TXEN_BIT);
    SET_BIT(UART_UCSRB_REG, UART_RXEN_BIT);

    /* STEP 6: Return E_OK. */
    return E_OK;
}


STD_ReturnType UART_DeInit(void)
{
    /* STEP 1: Clear TXEN and RXEN in UCSRB to disable the transmitter and receiver. */
    CLR_BIT(UART_UCSRB_REG, UART_TXEN_BIT);
    CLR_BIT(UART_UCSRB_REG, UART_RXEN_BIT);

    /* STEP 2: Clear RXCIE/TXCIE/UDRIE in UCSRB to disable USART interrupts. */
    CLR_BIT(UART_UCSRB_REG, UART_RXCIE_BIT);
    CLR_BIT(UART_UCSRB_REG, UART_TXCIE_BIT);
    CLR_BIT(UART_UCSRB_REG, UART_UDRIE_BIT);

    UART_RxCallBack = NULL;

    /* STEP 3: Return E_OK. */
    return E_OK;
}


STD_ReturnType UART_SendByte(uint8_h uint8Data)
{
    /*
     * STEP 1: Busy-wait until the data register is empty:
     *         while ( GET_BIT(UART_UCSRA_REG, UART_UDRE_BIT) == 0 ) { }
     */
    while (GET_BIT(UART_UCSRA_REG, UART_UDRE_BIT) == 0U)
    {
        /* wait for UDR to be free */
    }

    /* STEP 2: Write the byte: UART_UDR_REG = uint8Data; */
    UART_UDR_REG = uint8Data;

    /* STEP 3: Return E_OK. */
    return E_OK;
}


STD_ReturnType UART_ReceiveByte(uint8_h *puint8Data)
{
    /* STEP 1: Validate puint8Data != NULL (else E_NOK). */
    if (puint8Data == NULL)
    {
        return E_NOK;
    }

    /*
     * STEP 2: Busy-wait until a full frame has been received:
     *         while ( GET_BIT(UART_UCSRA_REG, UART_RXC_BIT) == 0 ) { }
     */
    while (GET_BIT(UART_UCSRA_REG, UART_RXC_BIT) == 0U)
    {
        /* wait for a complete frame */
    }

    /* STEP 3: Read the byte: *puint8Data = UART_UDR_REG; */
    *puint8Data = UART_UDR_REG;

    /* STEP 4: Return E_OK. */
    return E_OK;
}


STD_ReturnType UART_ReceiveByteNonBlocking(uint8_h *puint8Data)
{
    /* STEP 1: Validate puint8Data != NULL (else E_NOK). */
    if (puint8Data == NULL)
    {
        return E_NOK;
    }

    /*
     * STEP 2: Check the RXC flag ONCE (no waiting):
     *         if RXC == 0, return E_NOK (nothing available).
     */
    if (GET_BIT(UART_UCSRA_REG, UART_RXC_BIT) == 0U)
    {
        return E_NOK;
    }

    /* STEP 3: Otherwise read *puint8Data = UART_UDR_REG and return E_OK. */
    *puint8Data = UART_UDR_REG;
    return E_OK;
}


STD_ReturnType UART_SendString(const uint8_h *pString)
{
    uint16_h local_Index = 0U;

    /* STEP 1: Validate pString != NULL (else E_NOK). */
    if (pString == NULL)
    {
        return E_NOK;
    }

    /*
     * STEP 2: Loop over the string until the '\0' terminator:
     *         for (i = 0; pString[i] != '\0'; i++) UART_SendByte(pString[i]);
     */
    for (local_Index = 0U; pString[local_Index] != '\0'; local_Index++)
    {
        (void)UART_SendByte(pString[local_Index]);
    }

    /* STEP 3: Return E_OK. */
    return E_OK;
}


STD_ReturnType UART_ReceiveString(uint8_h *buffer, uint16_h maxLength, uint8_h terminator)
{
    uint16_h local_Index    = 0U;
    uint8_h  local_Received = 0U;

    /* STEP 1: Validate buffer != NULL and maxLength > 0 (else E_NOK). */
    if ((buffer == NULL) || (maxLength == 0U))
    {
        return E_NOK;
    }

    /*
     * STEP 2: Loop:
     *   - Read one byte with UART_ReceiveByte().
     *   - If it equals 'terminator', break.
     *   - Store it in buffer[index] and increment index.
     *   - Stop when index reaches (maxLength - 1) to leave room for '\0'.
     */
    while (local_Index < (uint16_h)(maxLength - 1U))
    {
        if (UART_ReceiveByte(&local_Received) != E_OK)
        {
            return E_NOK;
        }

        if (local_Received == terminator)
        {
            break;
        }

        buffer[local_Index] = local_Received;
        local_Index++;
    }

    /* STEP 3: Write buffer[index] = '\0' to NUL-terminate. */
    buffer[local_Index] = '\0';

    /* STEP 4: Return E_OK. */
    return E_OK;
}


STD_ReturnType UART_SetRxCallBack(UART_RxCallBackType callBack)
{
    /* STEP 1: Validate callBack != NULL (else E_NOK). */
    if (callBack == NULL)
    {
        return E_NOK;
    }

    /* STEP 2: Store it: UART_RxCallBack = callBack; */
    UART_RxCallBack = callBack;

    /* STEP 3: Enable the RX-complete interrupt: SET_BIT(UART_UCSRB_REG, UART_RXCIE_BIT); */
    SET_BIT(UART_UCSRB_REG, UART_RXCIE_BIT);

    /* STEP 4: Return E_OK. */
    return E_OK;
}


/* ================================================================================
 *  INTERRUPT SERVICE ROUTINE
 *  Reading UDR clears the RXC flag, then the registered callback is dispatched.
 * ============================================================================== */
ISR(USART_RXC_vect)
{
    uint8_h local_Data = UART_UDR_REG;

    if (UART_RxCallBack != NULL)
    {
        UART_RxCallBack(local_Data);
    }
}
