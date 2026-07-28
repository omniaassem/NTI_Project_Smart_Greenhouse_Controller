#include "../../Service/STD_Types.h"
#include "../../Service/Bit_Math.h"
#include "../../MCL/GPIO/gpio_interface.h"
#include "lcd_hd44780.h"

/*
 * util/delay.h needs F_CPU at compile time. The Makefile passes -DF_CPU, this
 * fallback only keeps the file compilable on its own.
 */
#ifndef F_CPU
#define F_CPU 8000000UL
#endif
#include <util/delay.h>

/* ================================================================================
 *  HD44780 CHARACTER LCD DRIVER - IMPLEMENTATION (HAL, parallel / GPIO)
 *  ------------------------------------------------------------------------------
 *  Each body keeps the ordered steps it implements as comments, followed by the
 *  actual GPIO calls. All state lives in the caller's handle, so the driver is
 *  re-entrant across displays: two handles never interfere.
 * ============================================================================== */

/* Instruction execution time of the controller for a normal instruction. */
#define LCD_HD44780_EXEC_DELAY_US      50U
/* Clear and Home are the two slow instructions (1.52 ms on the datasheet). */
#define LCD_HD44780_LONG_DELAY_MS      2U


/* --------------------------------------------------------------------------
 *  INTERNAL HELPERS (static - not part of the public interface)
 * ------------------------------------------------------------------------ */

/*
 * Generates the latch strobe: EN high -> hold -> EN low. The controller samples
 * the data bus on the falling edge, which is why the data must already be valid.
 */
static void LCD_Hd44780_EnablePulse(const LCD_Hd44780_HandleType *handle)
{
    (void)GPIO_SetPinValue(handle->controlPort, handle->enPin, PIN_HIGH);
    _delay_us(2);
    (void)GPIO_SetPinValue(handle->controlPort, handle->enPin, PIN_LOW);
    _delay_us(2);
}

/*
 * Places the low 4 bits of 'nibble' on D4..D7 and latches them. Used both for
 * the reset sequence and for every byte in 4-bit mode.
 */
static void LCD_Hd44780_WriteNibble(const LCD_Hd44780_HandleType *handle, uint8_h nibble)
{
    uint8_h local_Bit = 0U;

    for (local_Bit = 0U; local_Bit < 4U; local_Bit++)
    {
        (void)GPIO_SetPinValue(handle->dataPort,
                               (uint8_h)(handle->dataStartPin + local_Bit),
                               (uint8_h)GET_BIT(nibble, local_Bit));
    }

    LCD_Hd44780_EnablePulse(handle);
}

/*
 * Puts one full byte on the bus. In 8-bit mode that is a single write of D0..D7;
 * in 4-bit mode it is the high nibble followed by the low nibble. RS must already
 * carry the command/data selection chosen by the caller.
 */
static void LCD_Hd44780_WriteBus(const LCD_Hd44780_HandleType *handle, uint8_h value)
{
    uint8_h local_Bit = 0U;

    if (handle->bus == LCD_HD44780_BUS_8BIT)
    {
        for (local_Bit = 0U; local_Bit < 8U; local_Bit++)
        {
            (void)GPIO_SetPinValue(handle->dataPort,
                                   (uint8_h)(handle->dataStartPin + local_Bit),
                                   (uint8_h)GET_BIT(value, local_Bit));
        }

        LCD_Hd44780_EnablePulse(handle);
    }
    else
    {
        LCD_Hd44780_WriteNibble(handle, (uint8_h)(value >> 4));   /* high nibble first */
        LCD_Hd44780_WriteNibble(handle, (uint8_h)(value & 0x0FU));
    }

    _delay_us(LCD_HD44780_EXEC_DELAY_US);
}

/*
 * DDRAM address of the first character of a row. Rows 2 and 3 of a 4-line module
 * are the continuation of rows 0 and 1, offset by the line length - which is why
 * the geometry has to come from the handle.
 */
static uint8_h LCD_Hd44780_RowBase(const LCD_Hd44780_HandleType *handle, uint8_h row)
{
    uint8_h local_Base = 0x00U;

    switch (row)
    {
        case 0U:  local_Base = (uint8_h)0x00U;                        break;
        case 1U:  local_Base = (uint8_h)0x40U;                        break;
        case 2U:  local_Base = (uint8_h)(0x00U + handle->cols);       break;
        case 3U:  local_Base = (uint8_h)(0x40U + handle->cols);       break;
        default:  local_Base = (uint8_h)0x00U;                        break;
    }

    return local_Base;
}


/* --------------------------------------------------------------------------
 *  PUBLIC FUNCTIONS
 * ------------------------------------------------------------------------ */

STD_ReturnType LCD_Hd44780_Init(LCD_Hd44780_HandleType *handle)
{
    uint8_h local_Bit         = 0U;
    uint8_h local_DataPins    = 0U;
    uint8_h local_FunctionSet = 0U;

    /*
     * STEP 1: Validate the handle: non-NULL, ports in range, sane geometry, and
     *         in 8-bit mode the eight data lines must start at pin 0.
     */
    if (handle == NULL)
    {
        return E_NOK;
    }

    if ((handle->dataPort >= GPIO_NUMBER_OF_PORTS) ||
        (handle->controlPort >= GPIO_NUMBER_OF_PORTS))
    {
        return E_NOK;
    }

    if ((handle->rows == 0U) || (handle->cols == 0U) || (handle->rows > 4U))
    {
        return E_NOK;
    }

    local_DataPins = (handle->bus == LCD_HD44780_BUS_8BIT) ? 8U : 4U;

    if (((uint16_h)handle->dataStartPin + local_DataPins) > GPIO_NUMBER_OF_PINS)
    {
        return E_NOK;
    }

    /* STEP 2: Drive every wired line as an output and park it low. */
    for (local_Bit = 0U; local_Bit < local_DataPins; local_Bit++)
    {
        (void)GPIO_SetPinDirection(handle->dataPort,
                                   (uint8_h)(handle->dataStartPin + local_Bit),
                                   GPIO_OUTPUT);
        (void)GPIO_SetPinValue(handle->dataPort,
                               (uint8_h)(handle->dataStartPin + local_Bit), PIN_LOW);
    }

    (void)GPIO_SetPinDirection(handle->controlPort, handle->rsPin, GPIO_OUTPUT);
    (void)GPIO_SetPinDirection(handle->controlPort, handle->enPin, GPIO_OUTPUT);
    (void)GPIO_SetPinValue(handle->controlPort, handle->rsPin, PIN_LOW);
    (void)GPIO_SetPinValue(handle->controlPort, handle->enPin, PIN_LOW);

    if (handle->useRwPin != 0U)
    {
        /* The driver never reads, so RW is held low for the whole session. */
        (void)GPIO_SetPinDirection(handle->controlPort, handle->rwPin, GPIO_OUTPUT);
        (void)GPIO_SetPinValue(handle->controlPort, handle->rwPin, PIN_LOW);
    }

    /* STEP 3: Wait out the controller's power-on time (datasheet: > 40 ms). */
    _delay_ms(50);

    /*
     * STEP 4: Datasheet reset sequence. Three "function set 8-bit" nibbles put
     *         the controller into a known state no matter how it powered up;
     *         only then may 4-bit mode be selected.
     */
    if (handle->bus == LCD_HD44780_BUS_4BIT)
    {
        LCD_Hd44780_WriteNibble(handle, 0x03U);
        _delay_ms(5);
        LCD_Hd44780_WriteNibble(handle, 0x03U);
        _delay_us(150);
        LCD_Hd44780_WriteNibble(handle, 0x03U);
        _delay_us(150);
        LCD_Hd44780_WriteNibble(handle, 0x02U);   /* now switch to 4-bit */
        _delay_us(150);
    }
    else
    {
        LCD_Hd44780_WriteBus(handle, 0x30U);
        _delay_ms(5);
        LCD_Hd44780_WriteBus(handle, 0x30U);
        _delay_us(150);
        LCD_Hd44780_WriteBus(handle, 0x30U);
        _delay_us(150);
    }

    /*
     * STEP 5: Function set - bus width, line count, 5x8 font. From here on the
     *         bus width is fixed and full bytes may be sent normally.
     */
    local_FunctionSet = LCD_HD44780_CMD_FUNCTION_SET;

    if (handle->bus == LCD_HD44780_BUS_8BIT)
    {
        local_FunctionSet |= 0x10U;    /* DL = 1 : 8-bit interface */
    }

    if (handle->rows > 1U)
    {
        local_FunctionSet |= 0x08U;    /* N = 1 : two-line mode    */
    }

    (void)LCD_Hd44780_SendCommand(handle, local_FunctionSet);

    /* STEP 6: Display off, clear, entry mode = increment, then display on. */
    handle->displayControl = LCD_HD44780_CMD_DISPLAY_CTRL;                    /* all off */
    (void)LCD_Hd44780_SendCommand(handle, handle->displayControl);

    (void)LCD_Hd44780_SendCommand(handle, LCD_HD44780_CMD_CLEAR);
    _delay_ms(LCD_HD44780_LONG_DELAY_MS);

    handle->entryMode = (uint8_h)(LCD_HD44780_CMD_ENTRY_MODE | LCD_HD44780_ENTRY_INCREMENT);
    (void)LCD_Hd44780_SendCommand(handle, handle->entryMode);

    handle->displayControl = (uint8_h)(LCD_HD44780_CMD_DISPLAY_CTRL | LCD_HD44780_DISPLAY_ON);
    (void)LCD_Hd44780_SendCommand(handle, handle->displayControl);

    /* STEP 7: Mark the handle usable and return. */
    handle->cursorRow   = 0U;
    handle->cursorCol   = 0U;
    handle->initialized = 1U;

    return E_OK;
}


STD_ReturnType LCD_Hd44780_SendCommand(LCD_Hd44780_HandleType *handle, uint8_h command)
{
    /* STEP 1: Validate the handle. */
    if (handle == NULL)
    {
        return E_NOK;
    }

    /* STEP 2: RS = 0 selects the instruction register. */
    (void)GPIO_SetPinValue(handle->controlPort, handle->rsPin, PIN_LOW);

    /* STEP 3: Put the byte on the bus (one write, or two nibbles in 4-bit mode). */
    LCD_Hd44780_WriteBus(handle, command);

    return E_OK;
}


STD_ReturnType LCD_Hd44780_WriteChar(LCD_Hd44780_HandleType *handle, uint8_h character)
{
    /* STEP 1: Validate the handle and that Init() has run. */
    if ((handle == NULL) || (handle->initialized == 0U))
    {
        return E_NOK;
    }

    /* STEP 2: RS = 1 selects the data register. */
    (void)GPIO_SetPinValue(handle->controlPort, handle->rsPin, PIN_HIGH);

    /* STEP 3: Send the character; the controller advances its own cursor. */
    LCD_Hd44780_WriteBus(handle, character);

    return E_OK;
}


STD_ReturnType LCD_Hd44780_WriteString(LCD_Hd44780_HandleType *handle, const uint8_h *pString)
{
    uint16_h local_Index = 0U;

    /* STEP 1: Validate the handle and the pointer. */
    if ((handle == NULL) || (handle->initialized == 0U) || (pString == NULL))
    {
        return E_NOK;
    }

    /* STEP 2: Walk the string to its '\0' terminator, one character at a time. */
    for (local_Index = 0U; pString[local_Index] != '\0'; local_Index++)
    {
        (void)LCD_Hd44780_WriteChar(handle, pString[local_Index]);
    }

    return E_OK;
}


STD_ReturnType LCD_Hd44780_WriteStringAt(LCD_Hd44780_HandleType *handle,
                                         uint8_h row, uint8_h column,
                                         const uint8_h *pString)
{
    /* STEP 1: Move the cursor first; a bad position must fail before printing. */
    if (LCD_Hd44780_SetCursor(handle, row, column) != E_OK)
    {
        return E_NOK;
    }

    /* STEP 2: Print from there. */
    return LCD_Hd44780_WriteString(handle, pString);
}


STD_ReturnType LCD_Hd44780_WriteNumber(LCD_Hd44780_HandleType *handle, sint32 number)
{
    uint8_h  local_Digits[10];
    uint8_h  local_Count = 0U;
    uint32_h local_Value = 0UL;

    /* STEP 1: Validate the handle. */
    if ((handle == NULL) || (handle->initialized == 0U))
    {
        return E_NOK;
    }

    /* STEP 2: Zero is the one value the digit loop below never produces. */
    if (number == 0)
    {
        return LCD_Hd44780_WriteChar(handle, (uint8_h)'0');
    }

    /*
     * STEP 3: Print the sign and take the magnitude. The cast to uint32 is what
     *         makes the most negative value work instead of overflowing.
     */
    if (number < 0)
    {
        (void)LCD_Hd44780_WriteChar(handle, (uint8_h)'-');
        local_Value = (uint32_h)(-(sint32)number);
    }
    else
    {
        local_Value = (uint32_h)number;
    }

    /* STEP 4: Extract digits least-significant first, so they come out reversed. */
    while ((local_Value > 0UL) && (local_Count < 10U))
    {
        local_Digits[local_Count] = (uint8_h)('0' + (uint8_h)(local_Value % 10UL));
        local_Value /= 10UL;
        local_Count++;
    }

    /* STEP 5: Print them back out in the right order. */
    while (local_Count > 0U)
    {
        local_Count--;
        (void)LCD_Hd44780_WriteChar(handle, local_Digits[local_Count]);
    }

    return E_OK;
}


STD_ReturnType LCD_Hd44780_SetCursor(LCD_Hd44780_HandleType *handle,
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
    local_Address = (uint8_h)(LCD_Hd44780_RowBase(handle, row) + column);

    /* STEP 3: Bit 7 of the instruction marks it as "set DDRAM address". */
    (void)LCD_Hd44780_SendCommand(handle,
                                  (uint8_h)(LCD_HD44780_CMD_SET_DDRAM_ADDR | local_Address));

    handle->cursorRow = row;
    handle->cursorCol = column;

    return E_OK;
}


STD_ReturnType LCD_Hd44780_Clear(LCD_Hd44780_HandleType *handle)
{
    /* STEP 1: Validate the handle. */
    if ((handle == NULL) || (handle->initialized == 0U))
    {
        return E_NOK;
    }

    /* STEP 2: Send Clear, then wait - it takes ~1.52 ms, far longer than a write. */
    (void)LCD_Hd44780_SendCommand(handle, LCD_HD44780_CMD_CLEAR);
    _delay_ms(LCD_HD44780_LONG_DELAY_MS);

    handle->cursorRow = 0U;
    handle->cursorCol = 0U;

    return E_OK;
}


STD_ReturnType LCD_Hd44780_Home(LCD_Hd44780_HandleType *handle)
{
    /* STEP 1: Validate the handle. */
    if ((handle == NULL) || (handle->initialized == 0U))
    {
        return E_NOK;
    }

    /* STEP 2: Home is the second slow instruction - same wait as Clear. */
    (void)LCD_Hd44780_SendCommand(handle, LCD_HD44780_CMD_HOME);
    _delay_ms(LCD_HD44780_LONG_DELAY_MS);

    handle->cursorRow = 0U;
    handle->cursorCol = 0U;

    return E_OK;
}


STD_ReturnType LCD_Hd44780_DisplayOnOff(LCD_Hd44780_HandleType *handle, uint8_h on)
{
    /* STEP 1: Validate the handle. */
    if ((handle == NULL) || (handle->initialized == 0U))
    {
        return E_NOK;
    }

    /*
     * STEP 2: Display, cursor and blink share ONE instruction byte, so the driver
     *         keeps a shadow copy and edits only the bit that changed.
     */
    if (on != 0U)
    {
        handle->displayControl |= LCD_HD44780_DISPLAY_ON;
    }
    else
    {
        handle->displayControl &= (uint8_h)(~LCD_HD44780_DISPLAY_ON);
    }

    /* STEP 3: Re-send the whole group. */
    return LCD_Hd44780_SendCommand(handle, handle->displayControl);
}


STD_ReturnType LCD_Hd44780_CursorOnOff(LCD_Hd44780_HandleType *handle, uint8_h on)
{
    /* STEP 1: Validate the handle. */
    if ((handle == NULL) || (handle->initialized == 0U))
    {
        return E_NOK;
    }

    /* STEP 2: Edit the cursor bit of the shadow byte. */
    if (on != 0U)
    {
        handle->displayControl |= LCD_HD44780_CURSOR_ON;
    }
    else
    {
        handle->displayControl &= (uint8_h)(~LCD_HD44780_CURSOR_ON);
    }

    /* STEP 3: Re-send the whole group. */
    return LCD_Hd44780_SendCommand(handle, handle->displayControl);
}


STD_ReturnType LCD_Hd44780_BlinkOnOff(LCD_Hd44780_HandleType *handle, uint8_h on)
{
    /* STEP 1: Validate the handle. */
    if ((handle == NULL) || (handle->initialized == 0U))
    {
        return E_NOK;
    }

    /* STEP 2: Edit the blink bit of the shadow byte. */
    if (on != 0U)
    {
        handle->displayControl |= LCD_HD44780_BLINK_ON;
    }
    else
    {
        handle->displayControl &= (uint8_h)(~LCD_HD44780_BLINK_ON);
    }

    /* STEP 3: Re-send the whole group. */
    return LCD_Hd44780_SendCommand(handle, handle->displayControl);
}


STD_ReturnType LCD_Hd44780_ShiftDisplay(LCD_Hd44780_HandleType *handle, uint8_h toRight)
{
    uint8_h local_Command = 0U;

    /* STEP 1: Validate the handle. */
    if ((handle == NULL) || (handle->initialized == 0U))
    {
        return E_NOK;
    }

    /*
     * STEP 2: Bit 3 (S/C) selects "shift the display" over "move the cursor";
     *         bit 2 (R/L) picks the direction.
     */
    local_Command = (uint8_h)(LCD_HD44780_CMD_SHIFT | 0x08U);

    if (toRight != 0U)
    {
        local_Command |= 0x04U;
    }

    /* STEP 3: Send it. */
    return LCD_Hd44780_SendCommand(handle, local_Command);
}


STD_ReturnType LCD_Hd44780_CreateCustomChar(LCD_Hd44780_HandleType *handle,
                                            uint8_h location, const uint8_h *pPattern)
{
    uint8_h local_Row = 0U;

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
    (void)LCD_Hd44780_SendCommand(handle,
                                  (uint8_h)(LCD_HD44780_CMD_SET_CGRAM_ADDR | (uint8_h)(location << 3)));

    /* STEP 3: Write the eight pixel rows as ordinary data bytes. */
    for (local_Row = 0U; local_Row < 8U; local_Row++)
    {
        (void)LCD_Hd44780_WriteChar(handle, (uint8_h)(pPattern[local_Row] & 0x1FU));
    }

    /*
     * STEP 4: The address counter is still inside CGRAM. Move it back to DDRAM,
     *         otherwise the next WriteChar would overwrite the glyph just stored.
     */
    return LCD_Hd44780_SetCursor(handle, 0U, 0U);
}
