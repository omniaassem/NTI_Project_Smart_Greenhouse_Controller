#ifndef LCD_INTERFACE_H
#define LCD_INTERFACE_H

#include "../../Service/STD_Types.h"
#include "../../MCL/GPIO/gpio_interface.h"

/* ================================================================================
 *  CHARACTER LCD DRIVER - PUBLIC INTERFACE (HAL, HD44780-compatible)
 *  ------------------------------------------------------------------------------
 *  Drives an alphanumeric LCD (16x2, 20x4, ...) built around the HD44780
 *  controller through GPIO. Two bus widths are supported:
 *    - 8-bit mode : data lines D0..D7 use eight MCU pins.
 *    - 4-bit mode : only D4..D7 are used; each byte is sent as two nibbles.
 *
 *  Control lines:
 *    - RS : Register Select (0 = command/instruction, 1 = data/character).
 *    - RW : Read/Write (this driver only writes, so RW is tied low or driven low).
 *    - EN : Enable strobe (a high->low pulse latches the data).
 * ============================================================================== */

/* ---------------- Bus Mode ---------------- */
/**
 * @brief Data-bus width used to talk to the controller.
 */
typedef enum
{
    LCD_MODE_4BIT = 0,
    LCD_MODE_8BIT = 1
} LCD_ModeType;

/* ---------------- Common HD44780 Instruction Codes ---------------- */
#define LCD_CMD_CLEAR_DISPLAY        0x01   /* Clear screen, cursor home            */
#define LCD_CMD_RETURN_HOME          0x02   /* Cursor to (0,0), DDRAM unchanged     */
#define LCD_CMD_ENTRY_MODE_INC       0x06   /* Increment cursor, no display shift   */
#define LCD_CMD_DISPLAY_OFF          0x08   /* Display off                          */
#define LCD_CMD_DISPLAY_ON_NO_CURSOR 0x0C   /* Display on, cursor off               */
#define LCD_CMD_DISPLAY_ON_CURSOR    0x0E   /* Display on, cursor on                */
#define LCD_CMD_DISPLAY_ON_BLINK     0x0F   /* Display on, cursor blinking          */
#define LCD_CMD_SHIFT_CURSOR_LEFT    0x10   /* Move cursor left                     */
#define LCD_CMD_SHIFT_CURSOR_RIGHT   0x14   /* Move cursor right                    */
#define LCD_CMD_FUNCTION_SET_8BIT    0x38   /* 8-bit, 2 lines, 5x8 font             */
#define LCD_CMD_FUNCTION_SET_4BIT    0x28   /* 4-bit, 2 lines, 5x8 font             */
#define LCD_CMD_SET_CGRAM_ADDR       0x40   /* Base address of custom-char RAM      */
#define LCD_CMD_SET_DDRAM_ADDR       0x80   /* Base address for cursor positioning  */

/* Row base addresses (DDRAM) for a typical 4-line display */
#define LCD_ROW0_BASE                0x00
#define LCD_ROW1_BASE                0x40
#define LCD_ROW2_BASE                0x14
#define LCD_ROW3_BASE                0x54

/* ---------------- Configuration Structure ---------------- */
/**
 * @brief Pin assignment and bus width for the LCD.
 * @var LCD_ConfigType::mode        4-bit or 8-bit bus (LCD_ModeType).
 * @var LCD_ConfigType::dataPort    GPIO port carrying the data lines.
 * @var LCD_ConfigType::dataStartPin First data pin on dataPort:
 *                                   - 8-bit mode: D0..D7 = dataStartPin .. +7.
 *                                   - 4-bit mode: D4..D7 = dataStartPin .. +3.
 * @var LCD_ConfigType::controlPort GPIO port carrying RS / RW / EN.
 * @var LCD_ConfigType::rsPin       Pin (on controlPort) for the RS line.
 * @var LCD_ConfigType::rwPin       Pin (on controlPort) for the RW line (drive low).
 * @var LCD_ConfigType::enPin       Pin (on controlPort) for the EN strobe.
 */
typedef struct
{
    LCD_ModeType mode;
    uint8_h      dataPort;
    uint8_h      dataStartPin;
    uint8_h      controlPort;
    uint8_h      rsPin;
    uint8_h      rwPin;
    uint8_h      enPin;
} LCD_ConfigType;

/* ================================================================================
 *  FUNCTION PROTOTYPES
 * ============================================================================== */

/**
 * @brief  Runs the HD44780 power-on initialization sequence: sets pin directions,
 *         waits the required power-up delay, selects 4-/8-bit function set, turns
 *         the display on, and clears it.
 * @param  addConfig  Pointer to a populated configuration structure.
 * @return STD_ReturnType  E_OK/E_NOK (E_NOK on NULL pointer or bad port).
 */
STD_ReturnType LCD_Init(const LCD_ConfigType *addConfig);

/**
 * @brief  Sends an instruction byte to the controller (RS = 0).
 * @param  addConfig  Same configuration used at init.
 * @param  command    One of the LCD_CMD_* codes (or a raw instruction).
 * @return STD_ReturnType  E_OK/E_NOK.
 */
STD_ReturnType LCD_SendCommand(const LCD_ConfigType *addConfig, uint8_h command);

/**
 * @brief  Writes a single character to the current cursor position (RS = 1).
 * @param  addConfig  Same configuration used at init.
 * @param  character  ASCII/CGRAM code to display.
 * @return STD_ReturnType  E_OK/E_NOK.
 */
STD_ReturnType LCD_WriteChar(const LCD_ConfigType *addConfig, uint8_h character);

/**
 * @brief  Writes a NUL-terminated string starting at the current cursor position.
 * @param  addConfig  Same configuration used at init.
 * @param  pString    Pointer to a '\0'-terminated string; must not be NULL.
 * @return STD_ReturnType  E_OK/E_NOK.
 */
STD_ReturnType LCD_WriteString(const LCD_ConfigType *addConfig, const uint8_h *pString);

/**
 * @brief  Writes a signed integer as decimal text at the cursor position.
 * @param  addConfig  Same configuration used at init.
 * @param  number     Value to render.
 * @return STD_ReturnType  E_OK/E_NOK.
 */
STD_ReturnType LCD_WriteNumber(const LCD_ConfigType *addConfig, sint32 number);

/**
 * @brief  Moves the cursor to (row, column). Row/column are zero-based.
 * @param  addConfig  Same configuration used at init.
 * @param  row        Line index (0-based).
 * @param  column     Character index within the line (0-based).
 * @return STD_ReturnType  E_OK/E_NOK.
 */
STD_ReturnType LCD_SetCursor(const LCD_ConfigType *addConfig, uint8_h row, uint8_h column);

/**
 * @brief  Clears the whole display and returns the cursor home.
 * @param  addConfig  Same configuration used at init.
 * @return STD_ReturnType  E_OK/E_NOK.
 */
STD_ReturnType LCD_Clear(const LCD_ConfigType *addConfig);

/**
 * @brief  Stores a custom 5x8 character pattern into one of CGRAM slots 0..7.
 * @param  addConfig  Same configuration used at init.
 * @param  location   CGRAM slot 0..7.
 * @param  pPattern   Pointer to 8 bytes describing the glyph rows; must not be NULL.
 * @return STD_ReturnType  E_OK/E_NOK.
 */
STD_ReturnType LCD_CreateCustomChar(const LCD_ConfigType *addConfig,
                                    uint8_h location, const uint8_h *pPattern);

#endif /* LCD_INTERFACE_H */
