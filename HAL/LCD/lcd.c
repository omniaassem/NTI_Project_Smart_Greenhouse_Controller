#include "../../Service/STD_Types.h"
#include "../../Service/Bit_Math.h"
#include "../../MCL/GPIO/gpio_interface.h"
#include "lcd_interface.h"

/* ================================================================================
 *  CHARACTER LCD DRIVER - IMPLEMENTATION SKELETON
 *  ------------------------------------------------------------------------------
 *  Each body lists the ordered steps to implement the function. Replace the
 *  numbered comments with the actual GPIO calls.
 *
 *  You will need a short blocking delay helper (a busy loop or a Timer driver
 *  call) for the power-up sequence and the EN pulse. Referenced below as
 *  LCD_DelayMs(ms) / LCD_DelayUs(us) - implement these as static helpers.
 * ============================================================================== */

/*
 * INTERNAL HELPERS TO IMPLEMENT (static, not exposed in the header):
 *
 *  static void LCD_DelayMs(uint16_h ms);
 *      Blocking millisecond delay.
 *
 *  static void LCD_EnablePulse(const LCD_ConfigType *cfg);
 *      Generate the latch strobe: set EN high, wait ~1us, set EN low, wait ~100us.
 *
 *  static void LCD_WriteBus(const LCD_ConfigType *cfg, uint8_h value);
 *      8-bit mode: write all 8 bits to dataStartPin..+7, then pulse EN once.
 *      4-bit mode: write the HIGH nibble to dataStartPin..+3, pulse EN, then the
 *                  LOW nibble, pulse EN again.
 *      RS must already be set by the caller (command vs data).
 */


STD_ReturnType LCD_Init(const LCD_ConfigType *addConfig)
{
    /*
     * STEP 1: Validate addConfig != NULL and the ports are in range (else E_NOK).
     *
     * STEP 2: Configure pin directions as OUTPUT via GPIO_SetPinDirection:
     *   - Control pins: rsPin, rwPin, enPin on controlPort.
     *   - Data pins:    8 pins (8-bit) or 4 pins (4-bit) on dataPort.
     *   Drive RW low (this driver only writes).
     *
     * STEP 3: Wait >15 ms for the LCD power to stabilise: LCD_DelayMs(20).
     *
     * STEP 4: Select the bus width via the function-set command:
     *   - 8-bit: LCD_SendCommand(cfg, LCD_CMD_FUNCTION_SET_8BIT).
     *   - 4-bit: perform the special 4-bit init handshake (send 0x33, 0x32) then
     *            LCD_SendCommand(cfg, LCD_CMD_FUNCTION_SET_4BIT).
     *
     * STEP 5: Display ON, cursor off:  LCD_SendCommand(cfg, LCD_CMD_DISPLAY_ON_NO_CURSOR).
     * STEP 6: Entry mode (increment):  LCD_SendCommand(cfg, LCD_CMD_ENTRY_MODE_INC).
     * STEP 7: Clear display:           LCD_Clear(cfg).
     * STEP 8: Return E_OK.
     */
    return E_NOK;
}


STD_ReturnType LCD_SendCommand(const LCD_ConfigType *addConfig, uint8_h command)
{
    /*
     * STEP 1: Validate addConfig != NULL (else E_NOK).
     * STEP 2: Select the instruction register: GPIO_SetPinValue(controlPort, rsPin, 0).
     * STEP 3: Keep RW low (write): GPIO_SetPinValue(controlPort, rwPin, 0).
     * STEP 4: Put 'command' on the bus and latch it: LCD_WriteBus(addConfig, command).
     * STEP 5: Some commands (clear/home) need ~2 ms; add LCD_DelayMs(2) for those.
     * STEP 6: Return E_OK.
     */
    return E_NOK;
}


STD_ReturnType LCD_WriteChar(const LCD_ConfigType *addConfig, uint8_h character)
{
    /*
     * STEP 1: Validate addConfig != NULL (else E_NOK).
     * STEP 2: Select the data register: GPIO_SetPinValue(controlPort, rsPin, 1).
     * STEP 3: Keep RW low (write).
     * STEP 4: Put 'character' on the bus and latch it: LCD_WriteBus(addConfig, character).
     * STEP 5: Return E_OK.
     */
    return E_NOK;
}


STD_ReturnType LCD_WriteString(const LCD_ConfigType *addConfig, const uint8_h *pString)
{
    /*
     * STEP 1: Validate addConfig != NULL and pString != NULL (else E_NOK).
     * STEP 2: Loop until '\0', calling LCD_WriteChar(addConfig, pString[i]) each time.
     * STEP 3: Return E_OK.
     */
    return E_NOK;
}


STD_ReturnType LCD_WriteNumber(const LCD_ConfigType *addConfig, sint32 number)
{
    /*
     * STEP 1: Validate addConfig != NULL (else E_NOK).
     * STEP 2: If number == 0, write '0' and return.
     * STEP 3: If number < 0, write '-' and negate it.
     * STEP 4: Extract decimal digits (number % 10) into a temporary buffer - they
     *         come out least-significant first, so reverse them.
     * STEP 5: Write the digits in order via LCD_WriteChar (digit + '0').
     * STEP 6: Return E_OK.
     */
    return E_NOK;
}


STD_ReturnType LCD_SetCursor(const LCD_ConfigType *addConfig, uint8_h row, uint8_h column)
{
    /*
     * STEP 1: Validate addConfig != NULL (else E_NOK).
     * STEP 2: Map 'row' to its DDRAM base address:
     *         row 0 -> LCD_ROW0_BASE, 1 -> LCD_ROW1_BASE, 2 -> LCD_ROW2_BASE, 3 -> LCD_ROW3_BASE.
     * STEP 3: Add the column: address = base + column.
     * STEP 4: Send the set-DDRAM-address command:
     *         LCD_SendCommand(addConfig, LCD_CMD_SET_DDRAM_ADDR | address).
     * STEP 5: Return E_OK.
     */
    return E_NOK;
}


STD_ReturnType LCD_Clear(const LCD_ConfigType *addConfig)
{
    /*
     * STEP 1: Validate addConfig != NULL (else E_NOK).
     * STEP 2: Send LCD_CMD_CLEAR_DISPLAY.
     * STEP 3: Wait ~2 ms (clear is slow): LCD_DelayMs(2).
     * STEP 4: Return E_OK.
     */
    return E_NOK;
}


STD_ReturnType LCD_CreateCustomChar(const LCD_ConfigType *addConfig,
                                    uint8_h location, const uint8_h *pPattern)
{
    /*
     * STEP 1: Validate addConfig != NULL, pPattern != NULL, location <= 7 (else E_NOK).
     * STEP 2: Point to the CGRAM slot:
     *         LCD_SendCommand(addConfig, LCD_CMD_SET_CGRAM_ADDR | (location * 8)).
     * STEP 3: Write the 8 pattern bytes with LCD_WriteChar(addConfig, pPattern[i]).
     * STEP 4: Return the cursor to DDRAM (e.g. LCD_SetCursor(addConfig, 0, 0)) so
     *         subsequent writes go to the screen, not CGRAM.
     * STEP 5: Return E_OK.
     */
    return E_NOK;
}
