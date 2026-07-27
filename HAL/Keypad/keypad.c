#include "../../Service/STD_Types.h"
#include "../../Service/Bit_Math.h"
#include "../../MCL/GPIO/gpio_interface.h"
#include "keypad_interface.h"

/* ================================================================================
 *  MATRIX KEYPAD DRIVER - IMPLEMENTATION
 *  ------------------------------------------------------------------------------
 *  Rows are driven as OUTPUTS, columns are read as INPUTS with pull-ups.
 *  One row is driven LOW at a time; the column that reads LOW identifies the key.
 * ============================================================================== */

/* Rough busy-loop delay (~1 ms at F_CPU = 16 MHz) used for debouncing. */
static void Keypad_DelayMs(uint16_h ms)
{
    volatile uint16_h outer;
    volatile uint16_h inner;

    for (outer = 0U; outer < ms; ++outer)
    {
        for (inner = 0U; inner < 1600U; ++inner)
        {
            /* burn cycles */
        }
    }
}


STD_ReturnType Keypad_Init(const Keypad_ConfigType *addConfig)
{
    uint8_h index;

    if ((addConfig == NULL) ||
        (addConfig->rowPort >= GPIO_NUMBER_OF_PORTS) ||
        (addConfig->colPort >= GPIO_NUMBER_OF_PORTS))
    {
        return E_NOK;
    }

    if ((addConfig->rowStartPin + KEYPAD_ROWS > GPIO_NUMBER_OF_PINS) ||
        (addConfig->colStartPin + KEYPAD_COLS > GPIO_NUMBER_OF_PINS))
    {
        return E_NOK;
    }

    /* Rows: OUTPUT, idle HIGH (inactive). */
    for (index = 0U; index < KEYPAD_ROWS; ++index)
    {
        if (GPIO_SetPinDirection(addConfig->rowPort,
                                 addConfig->rowStartPin + index,
                                 GPIO_OUTPUT) != E_OK)
        {
            return E_NOK;
        }
        (void)GPIO_SetPinValue(addConfig->rowPort,
                               addConfig->rowStartPin + index, 1U);
    }

    /* Columns: INPUT with internal pull-ups enabled (write 1 to PORT bit). */
    for (index = 0U; index < KEYPAD_COLS; ++index)
    {
        if (GPIO_SetPinDirection(addConfig->colPort,
                                 addConfig->colStartPin + index,
                                 GPIO_INPUT) != E_OK)
        {
            return E_NOK;
        }
        (void)GPIO_SetPinValue(addConfig->colPort,
                               addConfig->colStartPin + index, 1U);
    }

    return E_OK;
}


STD_ReturnType Keypad_GetKey(const Keypad_ConfigType *addConfig, uint8_h *pKey)
{
    uint8_h row;
    uint8_h col;

    if ((addConfig == NULL) || (pKey == NULL))
    {
        return E_NOK;
    }

    *pKey = KEYPAD_NO_KEY;

    for (row = 0U; row < KEYPAD_ROWS; ++row)
    {
        uint8_h r;

        /* Drive all rows HIGH (inactive). */
        for (r = 0U; r < KEYPAD_ROWS; ++r)
        {
            (void)GPIO_SetPinValue(addConfig->rowPort,
                                   addConfig->rowStartPin + r, 1U);
        }

        /* Activate the current row (LOW). */
        (void)GPIO_SetPinValue(addConfig->rowPort,
                               addConfig->rowStartPin + row, 0U);

        for (col = 0U; col < KEYPAD_COLS; ++col)
        {
            if (GPIO_GetPinStatus(addConfig->colPort,
                                  addConfig->colStartPin + col) == PIN_LOW)
            {
                /* Debounce: confirm the key is still pressed. */
                Keypad_DelayMs(20U);
                if (GPIO_GetPinStatus(addConfig->colPort,
                                      addConfig->colStartPin + col) == PIN_LOW)
                {
                    /* Wait for release so one press = one key. */
                    while (GPIO_GetPinStatus(addConfig->colPort,
                                             addConfig->colStartPin + col) == PIN_LOW)
                    {
                        /* hold */
                    }

                    *pKey = addConfig->keyMap[row][col];
                    return E_OK;
                }
            }
        }
    }

    return E_OK;
}


STD_ReturnType Keypad_WaitForKey(const Keypad_ConfigType *addConfig, uint8_h *pKey)
{
    if ((addConfig == NULL) || (pKey == NULL))
    {
        return E_NOK;
    }

    do
    {
        if (Keypad_GetKey(addConfig, pKey) != E_OK)
        {
            return E_NOK;
        }
    } while (*pKey == KEYPAD_NO_KEY);

    return E_OK;
}
