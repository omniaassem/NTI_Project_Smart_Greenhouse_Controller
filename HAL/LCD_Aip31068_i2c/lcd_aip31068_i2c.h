#ifndef LCD_AIP31068_I2C_H
#define LCD_AIP31068_I2C_H

#include "../../Service/STD_Types.h"
#include "../../MCL/I2C/i2c_interface.h"

/* ================================================================================
 *  AiP31068 CHARACTER LCD DRIVER - PUBLIC INTERFACE (HAL, native I2C)
 *  ------------------------------------------------------------------------------
 *  The AiP31068 (AiP31068L, and the compatible ST7032 / Grove-LCD controller) is
 *  an HD44780-style character controller with an I2C interface built in. Unlike a
 *  PCF8574 "I2C backpack", there is no port expander and no nibble shuffling: the
 *  MCU sends a control byte followed by the instruction or character byte.
 *
 *  Wire format on the bus:
 *
 *      START | SLA+W | control | payload | ... | STOP
 *
 *      control = 0x00  ->  every following byte is an INSTRUCTION
 *      control = 0x40  ->  every following byte is DISPLAY DATA
 *
 *  (Bit 7 of the control byte is Co "continue"; leaving it 0 means "the rest of
 *  this transfer is all the same kind", which lets a whole string go out in one
 *  I2C transaction.)
 *
 *  Only 2 MCU pins are needed - SDA and SCL - no matter how many displays.
 *
 *  --------------------------------------------------------------------------
 *  MULTIPLE DISPLAYS
 *  --------------------------------------------------------------------------
 *  All state lives in your handle, so several displays are simply several
 *  handles that differ in 'i2cAddress':
 *
 *      LCD_Aip31068_HandleType g_lcdA;   // g_lcdA.i2cAddress = 0x3E
 *      LCD_Aip31068_HandleType g_lcdB;   // g_lcdB.i2cAddress = 0x3F
 *
 *  IMPORTANT, and worth saying in your report: most AiP31068 modules have the
 *  address strapped to 0x3E in hardware with no address-select pads. Two such
 *  modules on one bus will both answer and corrupt each other. To run two you
 *  need either a module with a selectable address, or a TCA9548A-style I2C
 *  multiplexer, or a second bus. The driver supports it - your hardware may not.
 *
 *  --------------------------------------------------------------------------
 *  HOW TO USE
 *  --------------------------------------------------------------------------
 *      I2C_MasterConfigType i2cCfg = { I2C_SCL_100KHZ };
 *      I2C_InitMaster(&i2cCfg);          // ONCE for the whole bus, not per LCD
 *
 *      LCD_Aip31068_HandleType lcd;
 *      lcd.i2cAddress = LCD_AIP31068_DEFAULT_ADDRESS;   // 0x3E
 *      lcd.rows       = 2;
 *      lcd.cols       = 16;
 *
 *      LCD_Aip31068_Init(&lcd);
 *      LCD_Aip31068_WriteStringAt(&lcd, 0, 0, (const uint8_h *)"ALARM PANEL");
 *
 *  Note that the I2C bus itself is a shared resource this driver does NOT own:
 *  call I2C_InitMaster() yourself, and do not call these functions from an ISR
 *  while the main loop is mid-transfer.
 * ============================================================================== */

/* ---------------- Bus Address ---------------- */
/** @brief Factory 7-bit address of nearly every AiP31068 module. */
#define LCD_AIP31068_DEFAULT_ADDRESS       0x3E

/* ---------------- Control Bytes ---------------- */
#define LCD_AIP31068_CTRL_COMMAND          0x00   /* payload bytes are instructions */
#define LCD_AIP31068_CTRL_DATA             0x40   /* payload bytes are characters   */

/* ---------------- Instruction Codes (HD44780-compatible) ---------------- */
#define LCD_AIP31068_CMD_CLEAR             0x01
#define LCD_AIP31068_CMD_HOME              0x02
#define LCD_AIP31068_CMD_ENTRY_MODE        0x04
#define LCD_AIP31068_CMD_DISPLAY_CTRL      0x08
#define LCD_AIP31068_CMD_SHIFT             0x10
#define LCD_AIP31068_CMD_FUNCTION_SET      0x20
#define LCD_AIP31068_CMD_SET_CGRAM_ADDR    0x40
#define LCD_AIP31068_CMD_SET_DDRAM_ADDR    0x80

/* Entry-mode flags */
#define LCD_AIP31068_ENTRY_INCREMENT       0x02
#define LCD_AIP31068_ENTRY_SHIFT_DISPLAY   0x01

/* Display-control flags */
#define LCD_AIP31068_DISPLAY_ON            0x04
#define LCD_AIP31068_CURSOR_ON             0x02
#define LCD_AIP31068_BLINK_ON              0x01

/* ---------------- Handle ---------------- */
/**
 * @brief One I2C display instance: its bus address, its geometry and the
 *        driver's private shadow of the controller state.
 *
 * Fill the CONFIGURATION fields before calling LCD_Aip31068_Init().
 * The RUNTIME fields belong to the driver.
 *
 * @var LCD_Aip31068_HandleType::i2cAddress  7-bit slave address (usually 0x3E).
 * @var LCD_Aip31068_HandleType::rows        Character lines (1, 2 or 4).
 * @var LCD_Aip31068_HandleType::cols        Characters per line (8, 16, 20, ...).
 */
typedef struct
{
    /* ---- configuration: fill these before Init ---- */
    uint8_h i2cAddress;
    uint8_h rows;
    uint8_h cols;

    /* ---- runtime: owned by the driver, do not modify ---- */
    uint8_h initialized;
    uint8_h displayControl;
    uint8_h entryMode;
    uint8_h cursorRow;
    uint8_h cursorCol;
} LCD_Aip31068_HandleType;

/* ================================================================================
 *  FUNCTION PROTOTYPES
 * ============================================================================== */

/**
 * @brief  Brings one display up over I2C: waits the power-on time, sends the
 *         function set three times as the datasheet requires, turns the display
 *         on with the cursor off, clears it and selects increment entry mode.
 * @param  handle  Pointer to YOUR handle with i2cAddress/rows/cols filled in.
 * @return E_OK on success; E_NOK on a NULL handle, an address above 0x7F, an
 *         impossible geometry, or if the module does not acknowledge on the bus.
 * @pre    I2C_InitMaster() must already have been called for the shared bus.
 * @note   Blocks for roughly 60 ms.
 * @note   A returned E_NOK on a correctly wired board almost always means the
 *         address is wrong or the pull-ups are missing.
 */
STD_ReturnType LCD_Aip31068_Init(LCD_Aip31068_HandleType *handle);

/**
 * @brief  Sends one raw instruction byte (control byte 0x00). Use it for
 *         controller features this API does not wrap.
 * @param  handle   Initialized display.
 * @param  command  Instruction byte, e.g. LCD_AIP31068_CMD_HOME.
 * @return E_OK/E_NOK (E_NOK if the slave did not acknowledge).
 */
STD_ReturnType LCD_Aip31068_SendCommand(LCD_Aip31068_HandleType *handle, uint8_h command);

/**
 * @brief  Writes one character at the cursor position (control byte 0x40).
 * @param  handle     Initialized display.
 * @param  character  ASCII code, or 0..7 for a custom character.
 * @return E_OK/E_NOK.
 */
STD_ReturnType LCD_Aip31068_WriteChar(LCD_Aip31068_HandleType *handle, uint8_h character);

/**
 * @brief  Writes a NUL-terminated string from the cursor position. The whole
 *         string goes out in ONE I2C transaction (single control byte followed
 *         by every character), which is what makes this driver fast enough to
 *         repaint a screen without flicker.
 * @param  handle   Initialized display.
 * @param  pString  '\0'-terminated text; must not be NULL.
 * @return E_OK/E_NOK.
 * @note   Text longer than the line does not wrap - position with SetCursor.
 */
STD_ReturnType LCD_Aip31068_WriteString(LCD_Aip31068_HandleType *handle, const uint8_h *pString);

/**
 * @brief  Convenience: move to (row, column) and write a string there.
 * @param  handle   Initialized display.
 * @param  row      Zero-based line index.
 * @param  column   Zero-based character index.
 * @param  pString  '\0'-terminated text; must not be NULL.
 * @return E_OK/E_NOK.
 */
STD_ReturnType LCD_Aip31068_WriteStringAt(LCD_Aip31068_HandleType *handle,
                                          uint8_h row, uint8_h column,
                                          const uint8_h *pString);

/**
 * @brief  Writes a signed integer in decimal at the cursor position.
 * @param  handle  Initialized display.
 * @param  number  Value to print; negatives get a leading '-'.
 * @return E_OK/E_NOK.
 * @note   Shrinking values leave a stale digit behind - pad the field or
 *         repaint the line.
 */
STD_ReturnType LCD_Aip31068_WriteNumber(LCD_Aip31068_HandleType *handle, sint32 number);

/**
 * @brief  Moves the cursor to a zero-based (row, column) using the geometry
 *         stored in the handle.
 * @param  handle  Initialized display.
 * @param  row     0 .. rows-1.
 * @param  column  0 .. cols-1.
 * @return E_OK/E_NOK (E_NOK if the position is off the display).
 */
STD_ReturnType LCD_Aip31068_SetCursor(LCD_Aip31068_HandleType *handle,
                                      uint8_h row, uint8_h column);

/**
 * @brief  Clears the screen and homes the cursor.
 * @param  handle  Initialized display.
 * @return E_OK/E_NOK.
 * @note   Blocks ~2 ms; do not call it on every repaint or the screen flickers.
 */
STD_ReturnType LCD_Aip31068_Clear(LCD_Aip31068_HandleType *handle);

/**
 * @brief  Homes the cursor without erasing the text.
 * @param  handle  Initialized display.
 * @return E_OK/E_NOK.
 */
STD_ReturnType LCD_Aip31068_Home(LCD_Aip31068_HandleType *handle);

/**
 * @brief  Turns the visible display on or off (the text in DDRAM survives).
 * @param  handle  Initialized display.
 * @param  on      1 = visible, 0 = blank.
 * @return E_OK/E_NOK.
 */
STD_ReturnType LCD_Aip31068_DisplayOnOff(LCD_Aip31068_HandleType *handle, uint8_h on);

/**
 * @brief  Shows or hides the underline cursor.
 * @param  handle  Initialized display.
 * @param  on      1 = show, 0 = hide.
 * @return E_OK/E_NOK.
 */
STD_ReturnType LCD_Aip31068_CursorOnOff(LCD_Aip31068_HandleType *handle, uint8_h on);

/**
 * @brief  Enables or disables the blinking block cursor.
 * @param  handle  Initialized display.
 * @param  on      1 = blink, 0 = steady.
 * @return E_OK/E_NOK.
 */
STD_ReturnType LCD_Aip31068_BlinkOnOff(LCD_Aip31068_HandleType *handle, uint8_h on);

/**
 * @brief  Shifts the displayed window one character left or right without
 *         changing DDRAM. Call repeatedly on a timer to scroll.
 * @param  handle   Initialized display.
 * @param  toRight  1 = right, 0 = left.
 * @return E_OK/E_NOK.
 */
STD_ReturnType LCD_Aip31068_ShiftDisplay(LCD_Aip31068_HandleType *handle, uint8_h toRight);

/**
 * @brief  Stores a 5x8 glyph in CGRAM slot 0..7; print it with WriteChar(slot).
 * @param  handle    Initialized display.
 * @param  location  CGRAM slot 0..7.
 * @param  pPattern  Eight bytes, top row first, bits 4..0 used. Must not be NULL.
 * @return E_OK/E_NOK.
 * @note   Returns the cursor to (0,0) so the next write lands in DDRAM again.
 */
STD_ReturnType LCD_Aip31068_CreateCustomChar(LCD_Aip31068_HandleType *handle,
                                             uint8_h location, const uint8_h *pPattern);

#endif /* LCD_AIP31068_I2C_H */
