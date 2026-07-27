#ifndef SEVEN_SEGMENT_INTERFACE_H
#define SEVEN_SEGMENT_INTERFACE_H

#include "../../Service/STD_Types.h"
#include "../../MCL/GPIO/gpio_interface.h"

/* ================================================================================
 *  SEVEN-SEGMENT DISPLAY DRIVER - PUBLIC INTERFACE (HAL)
 *  ------------------------------------------------------------------------------
 *  Drives one or more 7-segment digits through GPIO. Two wiring styles are
 *  supported:
 *    - Common Cathode : a segment lights when its pin is driven HIGH.
 *    - Common Anode   : a segment lights when its pin is driven LOW (inverted).
 *
 *  Two connection styles are supported:
 *    - Direct   : each of the 7 (or 8 with DP) segments has its own MCU pin.
 *    - BCD/7447 : four MCU pins feed a BCD-to-7-segment decoder (e.g. 7447/4511).
 *
 *  For multi-digit displays the segment lines are shared and each digit is
 *  enabled in turn (multiplexing) using per-digit "enable" pins.
 * ============================================================================== */

/* ---------------- Display Type (polarity) ---------------- */
/**
 * @brief Electrical type of the display, which decides the drive polarity.
 *  - SEVEN_SEG_COMMON_CATHODE : segment ON = pin HIGH.
 *  - SEVEN_SEG_COMMON_ANODE   : segment ON = pin LOW.
 */
typedef enum
{
    SEVEN_SEG_COMMON_CATHODE = 0,
    SEVEN_SEG_COMMON_ANODE   = 1
} SevenSeg_Type;

/* ---------------- Connection Style ---------------- */
/**
 * @brief How the digit is wired to the MCU.
 *  - SEVEN_SEG_CONNECTION_DIRECT : 7/8 segment pins driven individually.
 *  - SEVEN_SEG_CONNECTION_BCD    : 4 pins to an external BCD decoder IC.
 */
typedef enum
{
    SEVEN_SEG_CONNECTION_DIRECT = 0,
    SEVEN_SEG_CONNECTION_BCD    = 1
} SevenSeg_ConnectionType;

/* ---------------- Enable-line Active Level ---------------- */
/**
 * @brief Logic level that turns a digit's common (enable) line ON when
 *        multiplexing several digits.
 */
typedef enum
{
    SEVEN_SEG_ENABLE_ACTIVE_LOW  = 0,
    SEVEN_SEG_ENABLE_ACTIVE_HIGH = 1
} SevenSeg_EnableLevel;

/* ---------------- Configuration Structure ---------------- */
/**
 * @brief Describes how one seven-segment digit (or a decoder) is wired.
 * @var SevenSeg_ConfigType::type         Common-cathode vs common-anode (SevenSeg_Type).
 * @var SevenSeg_ConfigType::connection   Direct vs BCD decoder (SevenSeg_ConnectionType).
 * @var SevenSeg_ConfigType::dataPort     GPIO port carrying the segment/BCD lines
 *                                        (e.g. GPIO_PORTA). Direct mode uses pins 0..6
 *                                        (a..g) on this port; BCD mode uses pins 0..3.
 * @var SevenSeg_ConfigType::startPin     First pin on dataPort where the lines begin
 *                                        (segment 'a' or BCD bit 0).
 */
typedef struct
{
    SevenSeg_Type           type;
    SevenSeg_ConnectionType connection;
    uint8_h                 dataPort;
    uint8_h                 startPin;
} SevenSeg_ConfigType;

/* ================================================================================
 *  FUNCTION PROTOTYPES
 * ============================================================================== */

/**
 * @brief  Configures the segment/data pins as outputs and blanks the display.
 * @param  addConfig  Pointer to a populated configuration structure.
 * @return STD_ReturnType  E_OK/E_NOK (E_NOK on NULL pointer or bad port).
 */
STD_ReturnType SevenSeg_Init(const SevenSeg_ConfigType *addConfig);

/**
 * @brief  Shows a single decimal digit (0..9) on the configured display.
 *         In DIRECT mode it looks up the a..g segment pattern; in BCD mode it
 *         outputs the 4-bit binary value to the decoder. Polarity is handled
 *         automatically from the configured display type.
 * @param  addConfig  Same configuration used at init.
 * @param  digit      Value 0..9 to display.
 * @return STD_ReturnType  E_OK/E_NOK (E_NOK if digit > 9 or NULL pointer).
 */
STD_ReturnType SevenSeg_DisplayDigit(const SevenSeg_ConfigType *addConfig, uint8_h digit);

/**
 * @brief  Blanks the display (turns all segments off) without changing config.
 * @param  addConfig  Same configuration used at init.
 * @return STD_ReturnType  E_OK/E_NOK.
 */
STD_ReturnType SevenSeg_Clear(const SevenSeg_ConfigType *addConfig);

/**
 * @brief  Turns a digit's common/enable line ON (used when multiplexing digits).
 * @param  enablePort   GPIO port of the enable line.
 * @param  enablePin    Pin of the enable line.
 * @param  activeLevel  Level that enables the digit (SevenSeg_EnableLevel).
 * @return STD_ReturnType  E_OK/E_NOK.
 */
STD_ReturnType SevenSeg_EnableDigit(uint8_h enablePort, uint8_h enablePin,
                                    SevenSeg_EnableLevel activeLevel);

/**
 * @brief  Turns a digit's common/enable line OFF.
 * @param  enablePort   GPIO port of the enable line.
 * @param  enablePin    Pin of the enable line.
 * @param  activeLevel  Level that enables the digit (the OFF level is the opposite).
 * @return STD_ReturnType  E_OK/E_NOK.
 */
STD_ReturnType SevenSeg_DisableDigit(uint8_h enablePort, uint8_h enablePin,
                                     SevenSeg_EnableLevel activeLevel);

#endif /* SEVEN_SEGMENT_INTERFACE_H */
