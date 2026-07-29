#ifndef LCD_HD44780_H
#define LCD_HD44780_H

#include "../../Service/STD_Types.h"
#include "../../MCL/GPIO/gpio_interface.h"

/* ================================================================================
 *  HD44780 CHARACTER LCD DRIVER - PUBLIC INTERFACE (HAL, parallel / GPIO)
 *  ------------------------------------------------------------------------------
 *  Drives an alphanumeric LCD (8x1, 16x2, 20x4, ...) built around the HD44780
 *  controller (or a clone: KS0066, SPLC780, ST7066) over plain GPIO pins.
 *
 *  Bus widths:
 *    - 4-bit : only D4..D7 are wired; every byte is sent as two nibbles.
 *              Costs 6 MCU pins (4 data + RS + EN). Recommended.
 *    - 8-bit : D0..D7 are wired. Costs 10 MCU pins. Slightly faster, rarely worth it.
 *
 *  Control lines:
 *    - RS : Register Select   0 = instruction, 1 = character data.
 *    - RW : Read/Write        This driver only writes. Tie RW to GND and leave
 *                             'useRwPin' = 0, or wire it and let the driver hold
 *                             it low by setting 'useRwPin' = 1.
 *    - EN : Enable strobe     A high->low pulse latches whatever is on the bus.
 *
 *  --------------------------------------------------------------------------
 *  MULTIPLE DISPLAYS
 *  --------------------------------------------------------------------------
 *  Every function takes a pointer to YOUR handle, and the driver keeps no global
 *  state, so you may declare as many displays as you have pins for:
 *
 *      LCD_Hd44780_HandleType g_lcdMain;
 *      LCD_Hd44780_HandleType g_lcdDebug;
 *
 *  A cheaper trick for two displays: share the data bus and RS, and give each
 *  display its OWN EN pin. Only the display whose EN is pulsed latches the byte.
 *  Declare two handles with identical data/RS pins and different 'enPin'.
 *
 *  --------------------------------------------------------------------------
 *  HOW TO USE
 *  --------------------------------------------------------------------------
 *      LCD_Hd44780_HandleType lcd;
 *
 *      lcd.bus          = LCD_HD44780_BUS_4BIT;
 *      lcd.dataPort     = GPIO_PORTC;   // D4..D7 -> PC4..PC7
 *      lcd.dataStartPin = GPIO_PIN4;
 *      lcd.controlPort  = GPIO_PORTD;
 *      lcd.rsPin        = GPIO_PIN0;
 *      lcd.enPin        = GPIO_PIN1;
 *      lcd.useRwPin     = 0;            // RW wired to GND
 *      lcd.rwPin        = 0;            // ignored while useRwPin == 0
 *      lcd.rows         = 2;
 *      lcd.cols         = 16;
 *
 *      LCD_Hd44780_Init(&lcd);
 *      LCD_Hd44780_WriteStringAt(&lcd, 0, 0, (const uint8_h *)"ALARM PANEL");
 *      LCD_Hd44780_SetCursor(&lcd, 1, 0);
 *      LCD_Hd44780_WriteNumber(&lcd, 1234);
 * ============================================================================== */

/* ---------------- Bus Width ---------------- */
/**
 * @brief How many data lines are physically wired between the MCU and the LCD.
 */
typedef enum
{
    LCD_HD44780_BUS_4BIT = 0,   /* D4..D7 only, each byte sent as two nibbles */
    LCD_HD44780_BUS_8BIT = 1    /* D0..D7 wired, each byte sent in one write  */
} LCD_Hd44780_BusType;

/* ---------------- HD44780 Instruction Codes ---------------- */
#define LCD_HD44780_CMD_CLEAR              0x01   /* Clear display, cursor home      */
#define LCD_HD44780_CMD_HOME               0x02   /* Cursor home, DDRAM untouched    */
#define LCD_HD44780_CMD_ENTRY_MODE         0x04   /* Base of the entry-mode group    */
#define LCD_HD44780_CMD_DISPLAY_CTRL       0x08   /* Base of the display-on/off group*/
#define LCD_HD44780_CMD_SHIFT              0x10   /* Base of the cursor/display shift*/
#define LCD_HD44780_CMD_FUNCTION_SET       0x20   /* Base of the function-set group  */
#define LCD_HD44780_CMD_SET_CGRAM_ADDR     0x40   /* Custom-character RAM base       */
#define LCD_HD44780_CMD_SET_DDRAM_ADDR     0x80   /* Display RAM base (cursor move)  */

/* Entry-mode flags (OR into LCD_HD44780_CMD_ENTRY_MODE) */
#define LCD_HD44780_ENTRY_INCREMENT        0x02   /* Cursor moves right after a write */
#define LCD_HD44780_ENTRY_SHIFT_DISPLAY    0x01   /* Shift the whole display instead  */

/* Display-control flags (OR into LCD_HD44780_CMD_DISPLAY_CTRL) */
#define LCD_HD44780_DISPLAY_ON             0x04
#define LCD_HD44780_CURSOR_ON              0x02
#define LCD_HD44780_BLINK_ON               0x01

/* ---------------- Handle ---------------- */
/**
 * @brief One LCD instance: its wiring, its geometry and the driver's private
 *        shadow of the controller state.
 *
 * Fill the CONFIGURATION fields yourself before calling LCD_Hd44780_Init().
 * The RUNTIME fields are written by the driver - do not touch them.
 *
 * @var LCD_Hd44780_HandleType::bus           4-bit or 8-bit data bus.
 * @var LCD_Hd44780_HandleType::dataPort      GPIO port carrying the data lines.
 * @var LCD_Hd44780_HandleType::dataStartPin  First data pin on dataPort:
 *                                             - 8-bit: D0..D7 = dataStartPin..+7 (must be 0)
 *                                             - 4-bit: D4..D7 = dataStartPin..+3 (0 or 4)
 * @var LCD_Hd44780_HandleType::controlPort   GPIO port carrying RS / RW / EN.
 * @var LCD_Hd44780_HandleType::rsPin         Pin used for RS.
 * @var LCD_Hd44780_HandleType::rwPin         Pin used for RW (only if useRwPin != 0).
 * @var LCD_Hd44780_HandleType::enPin         Pin used for the EN strobe.
 * @var LCD_Hd44780_HandleType::useRwPin      0 = RW hard-wired to GND (normal),
 *                                            1 = RW driven low by the driver.
 * @var LCD_Hd44780_HandleType::rows          Number of character lines (1, 2 or 4).
 * @var LCD_Hd44780_HandleType::cols          Characters per line (8, 16, 20, ...).
 */
typedef struct
{
    /* ---- configuration: fill these before Init ---- */
    LCD_Hd44780_BusType bus;
    uint8_h dataPort;
    uint8_h dataStartPin;
    uint8_h controlPort;
    uint8_h rsPin;
    uint8_h rwPin;
    uint8_h enPin;
    uint8_h useRwPin;
    uint8_h rows;
    uint8_h cols;

    /* ---- runtime: owned by the driver, do not modify ---- */
    uint8_h initialized;      /* 1 once Init() succeeded                        */
    uint8_h displayControl;   /* shadow of the display/cursor/blink instruction */
    uint8_h entryMode;        /* shadow of the entry-mode instruction           */
    uint8_h cursorRow;        /* last row given to SetCursor                    */
    uint8_h cursorCol;        /* last column given to SetCursor                 */
} LCD_Hd44780_HandleType;

/* ================================================================================
 *  FUNCTION PROTOTYPES
 * ============================================================================== */

/**
 * @brief  Brings one display up: sets every pin to output, waits the >40 ms
 *         power-on time, runs the datasheet reset sequence, selects the bus
 *         width, turns the display on with the cursor off and clears the screen.
 * @param  handle  Pointer to YOUR handle with the configuration fields filled in.
 * @return E_OK on success; E_NOK on a NULL handle, a port/pin out of range, or
 *         an impossible geometry (rows == 0 or cols == 0).
 * @note   Blocks for roughly 60 ms. Call it once per display, at start-up.
 * @note   In 8-bit mode 'dataStartPin' must be 0 - eight consecutive lines do
 *         not fit on a port otherwise.
 */
STD_ReturnType LCD_Hd44780_Init(LCD_Hd44780_HandleType *handle);

/**
 * @brief  Sends a raw instruction byte (RS = 0). Use it for controller features
 *         this API does not wrap.
 * @param  handle   Initialized display.
 * @param  command  Instruction byte, e.g. LCD_HD44780_CMD_HOME.
 * @return E_OK/E_NOK.
 */
STD_ReturnType LCD_Hd44780_SendCommand(LCD_Hd44780_HandleType *handle, uint8_h command);

/**
 * @brief  Writes one character at the current cursor position (RS = 1). The
 *         controller advances the cursor by itself.
 * @param  handle     Initialized display.
 * @param  character  ASCII code, or 0..7 for a custom character.
 * @return E_OK/E_NOK.
 */
STD_ReturnType LCD_Hd44780_WriteChar(LCD_Hd44780_HandleType *handle, uint8_h character);

/**
 * @brief  Writes a NUL-terminated string from the current cursor position.
 * @param  handle   Initialized display.
 * @param  pString  '\0'-terminated text; must not be NULL.
 * @return E_OK/E_NOK.
 * @note   Text longer than the line does NOT wrap onto the next line - the
 *         HD44780 keeps writing into off-screen DDRAM. Position yourself with
 *         LCD_Hd44780_SetCursor() instead of relying on wrapping.
 */
STD_ReturnType LCD_Hd44780_WriteString(LCD_Hd44780_HandleType *handle, const uint8_h *pString);

/**
 * @brief  Convenience: move to (row, column) and write a string there. This is
 *         the call you want for repainting a fixed screen layout.
 * @param  handle   Initialized display.
 * @param  row      Zero-based line index.
 * @param  column   Zero-based character index.
 * @param  pString  '\0'-terminated text; must not be NULL.
 * @return E_OK/E_NOK.
 */
STD_ReturnType LCD_Hd44780_WriteStringAt(LCD_Hd44780_HandleType *handle,
                                         uint8_h row, uint8_h column,
                                         const uint8_h *pString);

/**
 * @brief  Writes a signed integer in decimal at the cursor position.
 * @param  handle  Initialized display.
 * @param  number  Value to print; negatives get a leading '-'.
 * @return E_OK/E_NOK.
 * @note   Prints only as many digits as the number needs. When a value shrinks
 *         (100 -> 99) the stale digit stays on screen, so either pad the field
 *         yourself or repaint the line.
 */
STD_ReturnType LCD_Hd44780_WriteNumber(LCD_Hd44780_HandleType *handle, sint32 number);

/**
 * @brief  Moves the cursor. Row/column are zero-based; the DDRAM address is
 *         computed from the handle's 'cols', so 16x2 and 20x4 both work.
 * @param  handle  Initialized display.
 * @param  row     0 .. rows-1.
 * @param  column  0 .. cols-1.
 * @return E_OK/E_NOK (E_NOK if row or column is outside the geometry).
 */
STD_ReturnType LCD_Hd44780_SetCursor(LCD_Hd44780_HandleType *handle,
                                     uint8_h row, uint8_h column);

/**
 * @brief  Clears the whole screen and returns the cursor to (0,0).
 * @param  handle  Initialized display.
 * @return E_OK/E_NOK.
 * @note   Blocks for ~2 ms - this instruction is slow on the controller. Do not
 *         call it every repaint or the display will visibly flicker; overwrite
 *         the changed characters instead.
 */
STD_ReturnType LCD_Hd44780_Clear(LCD_Hd44780_HandleType *handle);

/**
 * @brief  Returns the cursor to (0,0) without erasing the text.
 * @param  handle  Initialized display.
 * @return E_OK/E_NOK.
 */
STD_ReturnType LCD_Hd44780_Home(LCD_Hd44780_HandleType *handle);

/**
 * @brief  Turns the visible display on or off. The text in DDRAM survives, so
 *         this is a fast way to blank a screen (or flash it as an alert).
 * @param  handle  Initialized display.
 * @param  on      1 = visible, 0 = blank.
 * @return E_OK/E_NOK.
 */
STD_ReturnType LCD_Hd44780_DisplayOnOff(LCD_Hd44780_HandleType *handle, uint8_h on);

/**
 * @brief  Shows or hides the underline cursor.
 * @param  handle  Initialized display.
 * @param  on      1 = show, 0 = hide.
 * @return E_OK/E_NOK.
 */
STD_ReturnType LCD_Hd44780_CursorOnOff(LCD_Hd44780_HandleType *handle, uint8_h on);

/**
 * @brief  Enables or disables the blinking block cursor. Useful to show which
 *         digit a keypad entry will land on.
 * @param  handle  Initialized display.
 * @param  on      1 = blink, 0 = steady.
 * @return E_OK/E_NOK.
 */
STD_ReturnType LCD_Hd44780_BlinkOnOff(LCD_Hd44780_HandleType *handle, uint8_h on);

/**
 * @brief  Shifts the whole displayed window one character left or right without
 *         changing DDRAM. Call it repeatedly on a timer to scroll a long line.
 * @param  handle   Initialized display.
 * @param  toRight  1 = shift right, 0 = shift left.
 * @return E_OK/E_NOK.
 */
STD_ReturnType LCD_Hd44780_ShiftDisplay(LCD_Hd44780_HandleType *handle, uint8_h toRight);

/**
 * @brief  Stores a 5x8 glyph into one of the eight CGRAM slots. Print it later
 *         with LCD_Hd44780_WriteChar(handle, location).
 * @param  handle    Initialized display.
 * @param  location  CGRAM slot 0..7.
 * @param  pPattern  Eight bytes, top row first; only bits 4..0 of each byte are
 *                   used (bit 4 is the leftmost pixel). Must not be NULL.
 * @return E_OK/E_NOK.
 * @note   Writing CGRAM leaves the address counter pointing into CGRAM, so this
 *         function moves the cursor back to (0,0) before returning.
 */
STD_ReturnType LCD_Hd44780_CreateCustomChar(LCD_Hd44780_HandleType *handle,
                                            uint8_h location, const uint8_h *pPattern);

#endif /* LCD_HD44780_H */
