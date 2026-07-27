#ifndef KEYPAD_INTERFACE_H
#define KEYPAD_INTERFACE_H

#include "../../Service/STD_Types.h"
#include "../../MCL/GPIO/gpio_interface.h"

/* ================================================================================
 *  MATRIX KEYPAD DRIVER - PUBLIC INTERFACE (HAL)
 *  ------------------------------------------------------------------------------
 *  Scans an R x C matrix keypad (commonly 4x4) through GPIO. Scanning technique:
 *    - Rows are driven as OUTPUTS, columns are read as INPUTS with pull-ups.
 *    - The driver activates one row at a time (drives it LOW) and checks which
 *      column reads LOW; that intersection identifies the pressed key.
 *  A value equal to KEYPAD_NO_KEY means no key is currently pressed.
 * ============================================================================== */

/* ---------------- Geometry ---------------- */
#define KEYPAD_ROWS          4
#define KEYPAD_COLS          4

/**
 * @brief Sentinel returned when no key is pressed during a scan.
 */
#define KEYPAD_NO_KEY        0xFF

/* ---------------- Configuration Structure ---------------- */
/**
 * @brief Describes the pins the keypad is wired to and the character layout.
 * @var Keypad_ConfigType::rowPort     GPIO port carrying the row lines (driven).
 * @var Keypad_ConfigType::rowStartPin First row pin on rowPort; rows occupy
 *                                     rowStartPin .. rowStartPin + KEYPAD_ROWS-1.
 * @var Keypad_ConfigType::colPort     GPIO port carrying the column lines (read).
 * @var Keypad_ConfigType::colStartPin First column pin on colPort; columns occupy
 *                                     colStartPin .. colStartPin + KEYPAD_COLS-1.
 * @var Keypad_ConfigType::keyMap      Row-major table of the character printed on
 *                                     each key, e.g.
 *                                       {{'7','8','9','/'},
 *                                        {'4','5','6','*'},
 *                                        {'1','2','3','-'},
 *                                        {'C','0','=','+'}}
 */
typedef struct
{
    uint8_h rowPort;
    uint8_h rowStartPin;
    uint8_h colPort;
    uint8_h colStartPin;
    uint8_h keyMap[KEYPAD_ROWS][KEYPAD_COLS];
} Keypad_ConfigType;

/* ================================================================================
 *  FUNCTION PROTOTYPES
 * ============================================================================== */

/**
 * @brief  Configures the row pins as outputs (idle HIGH) and the column pins as
 *         inputs, ready for scanning.
 * @param  addConfig  Pointer to a populated configuration structure.
 * @return STD_ReturnType  E_OK/E_NOK (E_NOK on NULL pointer or bad port).
 */
STD_ReturnType Keypad_Init(const Keypad_ConfigType *addConfig);

/**
 * @brief  Performs one non-blocking scan of the matrix and returns the mapped
 *         character of the first pressed key, or KEYPAD_NO_KEY if none is pressed.
 * @param  addConfig  Same configuration used at init.
 * @param  pKey       Pointer that receives the pressed key's character (or
 *                    KEYPAD_NO_KEY); must not be NULL.
 * @return STD_ReturnType  E_OK if a scan completed, E_NOK on NULL pointer.
 */
STD_ReturnType Keypad_GetKey(const Keypad_ConfigType *addConfig, uint8_h *pKey);

/**
 * @brief  Blocking read: repeatedly scans until a key is pressed, then returns it.
 *         Optionally waits for release / applies debounce internally.
 * @param  addConfig  Same configuration used at init.
 * @param  pKey       Pointer that receives the pressed key's character; must not be NULL.
 * @return STD_ReturnType  E_OK/E_NOK (E_NOK on NULL pointer).
 */
STD_ReturnType Keypad_WaitForKey(const Keypad_ConfigType *addConfig, uint8_h *pKey);

#endif /* KEYPAD_INTERFACE_H */
