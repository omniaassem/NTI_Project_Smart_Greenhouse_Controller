#include "../../Service/STD_Types.h"
#include "../../Service/Bit_Math.h"
#include "../../MCL/I2C/i2c_interface.h"
#include "lcd_aip31068_i2c.h"

/*
 * util/delay.h needs F_CPU at compile time. The Makefile passes -DF_CPU, this
 * fallback only keeps the file compilable on its own.
 */
#ifndef F_CPU
#define F_CPU 8000000UL
#endif
#include <util/delay.h>

/* ================================================================================
 *  AiP31068 CHARACTER LCD DRIVER - IMPLEMENTATION (HAL, native I2C)
 *  ------------------------------------------------------------------------------
 *  Each body keeps the ordered steps it implements as comments, followed by the
 *  actual bus calls. All state lives in the caller's handle, so two displays on
 *  two addresses never interfere.
 *
 *  DEPENDENCY: the I2C (TWI) master driver in MCL/I2C. The application must call
 *  I2C_InitMaster() once before any display is initialized - the bus is shared,
 *  so it is not this driver's to configure.
 * ============================================================================== */

/* Controller execution times (datasheet: ~27 us normal, ~1.08 ms clear/home). */
#define LCD_AIP31068_EXEC_DELAY_US     50U
#define LCD_AIP31068_LONG_DELAY_MS     2U


/* --------------------------------------------------------------------------
 *  INTERNAL HELPERS (static - not part of the public interface)
 * ------------------------------------------------------------------------ */

/*
 * One complete I2C transaction:
 *
 *      START | SLA+W | control | payload[0..len-1] | STOP
 *
 * 'control' is 0x00 for instructions or 0x40 for character data. Sending the
 * control byte once and then every payload byte is what lets a whole string go
 * out in a single transfer instead of one transfer per character.
 */
static STD_ReturnType LCD_Aip31068_Transfer(const LCD_Aip31068_HandleType *handle,
                                            uint8_h control,
                                            const uint8_h *pPayload,
                                            uint16_h length)
{
    uint16_h local_Index = 0U;

    /* STEP 1: Claim the bus. */
    if (I2C_Start() != E_OK)
    {
        return E_NOK;
    }

    /* STEP 2: Address the display for writing (7-bit address + R/W = 0). */
    if (I2C_WriteByte((uint8_h)((handle->i2cAddress << 1) | 0x00U)) != E_OK)
    {
        (void)I2C_Stop();
        return E_NOK;
    }

    /* STEP 3: Announce what the payload is: instructions or characters. */
    if (I2C_WriteByte(control) != E_OK)
    {
        (void)I2C_Stop();
        return E_NOK;
    }

    /* STEP 4: Push the payload. */
    for (local_Index = 0U; local_Index < length; local_Index++)
    {
        if (I2C_WriteByte(pPayload[local_Index]) != E_OK)
        {
            (void)I2C_Stop();
            return E_NOK;
        }
    }

    /* STEP 5: Release the bus. */
    (void)I2C_Stop();

    return E_OK;
}

/*
 * DDRAM address of the first character of a row. Rows 2 and 3 of a 4-line module
 * continue rows 0 and 1, offset by the line length - hence the geometry lookup.
 */
static uint8_h LCD_Aip31068_RowBase(const LCD_Aip31068_HandleType *handle, uint8_h row)
{
    uint8_h local_Base = 0x00U;

    switch (row)
    {
        case 0U:  local_Base = (uint8_h)0x00U;                   break;
        case 1U:  local_Base = (uint8_h)0x40U;                   break;
        case 2U:  local_Base = (uint8_h)(0x00U + handle->cols);  break;
        case 3U:  local_Base = (uint8_h)(0x40U + handle->cols);  break;
        default:  local_Base = (uint8_h)0x00U;                   break;
    }

    return local_Base;
}


/* --------------------------------------------------------------------------
 *  PUBLIC FUNCTIONS
 * ------------------------------------------------------------------------ */

STD_ReturnType LCD_Aip31068_Init(LCD_Aip31068_HandleType *handle)
{
    uint8_h local_FunctionSet = 0U;

    /* STEP 1: Validate the handle, the 7-bit address and the geometry. */
    if (handle == NULL)
    {
        return E_NOK;
    }

    if (handle->i2cAddress > 0x7FU)
    {
        return E_NOK;
    }

    if ((handle->rows == 0U) || (handle->cols == 0U) || (handle->rows > 4U))
    {
        return E_NOK;
    }

    handle->initialized = 0U;

    /* STEP 2: Let the controller finish its own power-on reset (> 40 ms). */
    _delay_ms(50);

    /*
     * STEP 3: Function set - 8-bit interface bit is fixed high on this part,
     *         N selects one or two display lines, 5x8 font.
     *         The datasheet asks for it three times, spaced out, so a controller
     *         that missed the first one still lands in a known state.
     */
    local_FunctionSet = LCD_AIP31068_CMD_FUNCTION_SET | 0x10U;   /* DL = 1 */

    if (handle->rows > 1U)
    {
        local_FunctionSet |= 0x08U;                              /* N = 1  */
    }

    if (LCD_Aip31068_SendCommand(handle, local_FunctionSet) != E_OK)
    {
        return E_NOK;      /* nobody acknowledged: wrong address or no pull-ups */
    }

    _delay_ms(5);
    (void)LCD_Aip31068_SendCommand(handle, local_FunctionSet);
    _delay_us(200);
    (void)LCD_Aip31068_SendCommand(handle, local_FunctionSet);
    _delay_us(200);

    /* STEP 4: Display off while the rest is configured. */
    handle->displayControl = LCD_AIP31068_CMD_DISPLAY_CTRL;
    (void)LCD_Aip31068_SendCommand(handle, handle->displayControl);

    /* STEP 5: Clear the screen (slow instruction - wait it out). */
    (void)LCD_Aip31068_SendCommand(handle, LCD_AIP31068_CMD_CLEAR);
    _delay_ms(LCD_AIP31068_LONG_DELAY_MS);

    /* STEP 6: Entry mode = cursor moves right, display does not shift. */
    handle->entryMode = (uint8_h)(LCD_AIP31068_CMD_ENTRY_MODE | LCD_AIP31068_ENTRY_INCREMENT);
    (void)LCD_Aip31068_SendCommand(handle, handle->entryMode);

    /* STEP 7: Display on, cursor and blink off. */
    handle->displayControl = (uint8_h)(LCD_AIP31068_CMD_DISPLAY_CTRL | LCD_AIP31068_DISPLAY_ON);
    (void)LCD_Aip31068_SendCommand(handle, handle->displayControl);

    /* STEP 8: Mark the handle usable. */
    handle->cursorRow   = 0U;
    handle->cursorCol   = 0U;
    handle->initialized = 1U;

    return E_OK;
}


STD_ReturnType LCD_Aip31068_SendCommand(LCD_Aip31068_HandleType *handle, uint8_h command)
{
    STD_ReturnType local_Status = E_NOK;

    /* STEP 1: Validate the handle. */
    if (handle == NULL)
    {
        return E_NOK;
    }

    /* STEP 2: One transfer: control byte 0x00 then the instruction. */
    local_Status = LCD_Aip31068_Transfer(handle, LCD_AIP31068_CTRL_COMMAND, &command, 1U);

    /* STEP 3: Give the controller time to execute before the next transfer. */
    _delay_us(LCD_AIP31068_EXEC_DELAY_US);

    return local_Status;
}


STD_ReturnType LCD_Aip31068_WriteChar(LCD_Aip31068_HandleType *handle, uint8_h character)
{
    STD_ReturnType local_Status = E_NOK;

    /* STEP 1: Validate the handle and that Init() has run. */
    if ((handle == NULL) || (handle->initialized == 0U))
    {
        return E_NOK;
    }

    /* STEP 2: One transfer: control byte 0x40 then the character. */
    local_Status = LCD_Aip31068_Transfer(handle, LCD_AIP31068_CTRL_DATA, &character, 1U);

    /* STEP 3: Let the write settle. */
    _delay_us(LCD_AIP31068_EXEC_DELAY_US);

    return local_Status;
}


STD_ReturnType LCD_Aip31068_WriteString(LCD_Aip31068_HandleType *handle, const uint8_h *pString)
{
    uint16_h local_Length = 0U;

    /* STEP 1: Validate the handle and the pointer. */
    if ((handle == NULL) || (handle->initialized == 0U) || (pString == NULL))
    {
        return E_NOK;
    }

    /* STEP 2: Measure the string - the whole thing rides in one transaction. */
    while (pString[local_Length] != '\0')
    {
        local_Length++;
    }

    if (local_Length == 0U)
    {
        return E_OK;      /* an empty string is a valid no-op */
    }

    /* STEP 3: Control byte 0x40 once, then every character back to back. */
    return LCD_Aip31068_Transfer(handle, LCD_AIP31068_CTRL_DATA, pString, local_Length);
}


STD_ReturnType LCD_Aip31068_WriteStringAt(LCD_Aip31068_HandleType *handle,
                                          uint8_h row, uint8_h column,
                                          const uint8_h *pString)
{
    /* STEP 1: Move the cursor first; an impossible position must fail early. */
    if (LCD_Aip31068_SetCursor(handle, row, column) != E_OK)
    {
        return E_NOK;
    }

    /* STEP 2: Print from there. */
    return LCD_Aip31068_WriteString(handle, pString);
}


STD_ReturnType LCD_Aip31068_WriteNumber(LCD_Aip31068_HandleType *handle, sint32 number)
{
    uint8_h  local_Text[12];
    uint8_h  local_Digits[10];
    uint8_h  local_Count  = 0U;
    uint8_h  local_Length = 0U;
    uint32_h local_Value  = 0UL;

    /* STEP 1: Validate the handle. */
    if ((handle == NULL) || (handle->initialized == 0U))
    {
        return E_NOK;
    }

    /* STEP 2: Zero is the one value the digit loop below never produces. */
    if (number == 0)
    {
        return LCD_Aip31068_WriteChar(handle, (uint8_h)'0');
    }

    /*
     * STEP 3: Emit the sign and take the magnitude. Casting through uint32 is
     *         what keeps the most negative value from overflowing.
     */
    if (number < 0)
    {
        local_Text[local_Length] = (uint8_h)'-';
        local_Length++;
        local_Value = (uint32_h)(-(sint32)number);
    }
    else
    {
        local_Value = (uint32_h)number;
    }

    /* STEP 4: Extract digits least-significant first (so, reversed). */
    while ((local_Value > 0UL) && (local_Count < 10U))
    {
        local_Digits[local_Count] = (uint8_h)('0' + (uint8_h)(local_Value % 10UL));
        local_Value /= 10UL;
        local_Count++;
    }

    /* STEP 5: Reverse them into the output buffer and terminate it. */
    while (local_Count > 0U)
    {
        local_Count--;
        local_Text[local_Length] = local_Digits[local_Count];
        local_Length++;
    }

    local_Text[local_Length] = '\0';

    /* STEP 6: One transfer for the whole number. */
    return LCD_Aip31068_WriteString(handle, local_Text);
}


STD_ReturnType LCD_Aip31068_SetCursor(LCD_Aip31068_HandleType *handle,
                                      uint8_h row, uint8_h column)
{
    uint8_h local_Address = 0U;

    /* STEP 1: Validate the handle and that the position exists on this module. */
    if ((handle == NULL) || (handle->initialized == 0U))
    {
        return E_NOK;
    }

    if ((row >= handle->rows) || (column >= handle->cols))
    {
        return E_NOK;
    }

    /* STEP 2: DDRAM address = row base + column. */
    local_Address = (uint8_h)(LCD_Aip31068_RowBase(handle, row) + column);

    /* STEP 3: Bit 7 marks the instruction as "set DDRAM address". */
    if (LCD_Aip31068_SendCommand(handle,
            (uint8_h)(LCD_AIP31068_CMD_SET_DDRAM_ADDR | local_Address)) != E_OK)
    {
        return E_NOK;
    }

    handle->cursorRow = row;
    handle->cursorCol = column;

    return E_OK;
}


STD_ReturnType LCD_Aip31068_Clear(LCD_Aip31068_HandleType *handle)
{
    /* STEP 1: Validate the handle. */
    if ((handle == NULL) || (handle->initialized == 0U))
    {
        return E_NOK;
    }

    /* STEP 2: Send Clear and wait - it is ~40x slower than an ordinary write. */
    if (LCD_Aip31068_SendCommand(handle, LCD_AIP31068_CMD_CLEAR) != E_OK)
    {
        return E_NOK;
    }

    _delay_ms(LCD_AIP31068_LONG_DELAY_MS);

    handle->cursorRow = 0U;
    handle->cursorCol = 0U;

    return E_OK;
}


STD_ReturnType LCD_Aip31068_Home(LCD_Aip31068_HandleType *handle)
{
    /* STEP 1: Validate the handle. */
    if ((handle == NULL) || (handle->initialized == 0U))
    {
        return E_NOK;
    }

    /* STEP 2: Home is the other slow instruction - same wait as Clear. */
    if (LCD_Aip31068_SendCommand(handle, LCD_AIP31068_CMD_HOME) != E_OK)
    {
        return E_NOK;
    }

    _delay_ms(LCD_AIP31068_LONG_DELAY_MS);

    handle->cursorRow = 0U;
    handle->cursorCol = 0U;

    return E_OK;
}


STD_ReturnType LCD_Aip31068_DisplayOnOff(LCD_Aip31068_HandleType *handle, uint8_h on)
{
    /* STEP 1: Validate the handle. */
    if ((handle == NULL) || (handle->initialized == 0U))
    {
        return E_NOK;
    }

    /*
     * STEP 2: Display, cursor and blink share ONE instruction byte, so the
     *         driver edits its shadow copy and re-sends the whole group.
     */
    if (on != 0U)
    {
        handle->displayControl |= LCD_AIP31068_DISPLAY_ON;
    }
    else
    {
        handle->displayControl &= (uint8_h)(~LCD_AIP31068_DISPLAY_ON);
    }

    /* STEP 3: Re-send it. */
    return LCD_Aip31068_SendCommand(handle, handle->displayControl);
}


STD_ReturnType LCD_Aip31068_CursorOnOff(LCD_Aip31068_HandleType *handle, uint8_h on)
{
    /* STEP 1: Validate the handle. */
    if ((handle == NULL) || (handle->initialized == 0U))
    {
        return E_NOK;
    }

    /* STEP 2: Edit the cursor bit of the shadow byte. */
    if (on != 0U)
    {
        handle->displayControl |= LCD_AIP31068_CURSOR_ON;
    }
    else
    {
        handle->displayControl &= (uint8_h)(~LCD_AIP31068_CURSOR_ON);
    }

    /* STEP 3: Re-send it. */
    return LCD_Aip31068_SendCommand(handle, handle->displayControl);
}


STD_ReturnType LCD_Aip31068_BlinkOnOff(LCD_Aip31068_HandleType *handle, uint8_h on)
{
    /* STEP 1: Validate the handle. */
    if ((handle == NULL) || (handle->initialized == 0U))
    {
        return E_NOK;
    }

    /* STEP 2: Edit the blink bit of the shadow byte. */
    if (on != 0U)
    {
        handle->displayControl |= LCD_AIP31068_BLINK_ON;
    }
    else
    {
        handle->displayControl &= (uint8_h)(~LCD_AIP31068_BLINK_ON);
    }

    /* STEP 3: Re-send it. */
    return LCD_Aip31068_SendCommand(handle, handle->displayControl);
}


STD_ReturnType LCD_Aip31068_ShiftDisplay(LCD_Aip31068_HandleType *handle, uint8_h toRight)
{
    uint8_h local_Command = 0U;

    /* STEP 1: Validate the handle. */
    if ((handle == NULL) || (handle->initialized == 0U))
    {
        return E_NOK;
    }

    /*
     * STEP 2: Bit 3 (S/C) selects "shift display" over "move cursor";
     *         bit 2 (R/L) picks the direction.
     */
    local_Command = (uint8_h)(LCD_AIP31068_CMD_SHIFT | 0x08U);

    if (toRight != 0U)
    {
        local_Command |= 0x04U;
    }

    /* STEP 3: Send it. */
    return LCD_Aip31068_SendCommand(handle, local_Command);
}


STD_ReturnType LCD_Aip31068_CreateCustomChar(LCD_Aip31068_HandleType *handle,
                                             uint8_h location, const uint8_h *pPattern)
{
    uint8_h local_Rows[8];
    uint8_h local_Index = 0U;

    /* STEP 1: Validate the handle, the slot (0..7) and the pattern pointer. */
    if ((handle == NULL) || (handle->initialized == 0U) || (pPattern == NULL))
    {
        return E_NOK;
    }

    if (location > 7U)
    {
        return E_NOK;
    }

    /* STEP 2: Point the address counter at that slot - 8 bytes per glyph. */
    if (LCD_Aip31068_SendCommand(handle,
            (uint8_h)(LCD_AIP31068_CMD_SET_CGRAM_ADDR | (uint8_h)(location << 3))) != E_OK)
    {
        return E_NOK;
    }

    /* STEP 3: Mask each row to the five visible pixels. */
    for (local_Index = 0U; local_Index < 8U; local_Index++)
    {
        local_Rows[local_Index] = (uint8_h)(pPattern[local_Index] & 0x1FU);
    }

    /* STEP 4: All eight rows go out as one data transfer. */
    if (LCD_Aip31068_Transfer(handle, LCD_AIP31068_CTRL_DATA, local_Rows, 8U) != E_OK)
    {
        return E_NOK;
    }

    /*
     * STEP 5: The address counter still points into CGRAM. Move it back to DDRAM
     *         or the next character written would overwrite the glyph.
     */
    return LCD_Aip31068_SetCursor(handle, 0U, 0U);
}
