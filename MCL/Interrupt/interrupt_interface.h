#ifndef INTERRUPT_INTERFACE_H
#define INTERRUPT_INTERFACE_H

#include "../../Service/STD_Types.h"
#include "interrupt_registers.h"

/* ================================================================================
 *  EXTERNAL INTERRUPT (EXTI) DRIVER - PUBLIC INTERFACE (ATmega32)
 *  ------------------------------------------------------------------------------
 *  Configures and controls the three external interrupt lines INT0/INT1/INT2.
 *  Typical usage:
 *      1) EXTI_SetCallBack(EXTI_INT0, myHandler);
 *      2) EXTI_Init(&cfg);            // sets sense control + enables the line
 *      3) EXTI_EnableGlobalInterrupt();
 *  When the configured edge/level occurs, the driver's ISR calls myHandler().
 * ============================================================================== */

/* ---------------- Interrupt Lines ---------------- */
/**
 * @brief Selects which external interrupt line an API call targets.
 *  - EXTI_INT0 : pin PD2, supports level + edge sensing.
 *  - EXTI_INT1 : pin PD3, supports level + edge sensing.
 *  - EXTI_INT2 : pin PB2, edge sensing only (falling or rising).
 */
typedef enum
{
    EXTI_INT0 = 0,
    EXTI_INT1 = 1,
    EXTI_INT2 = 2,
    EXTI_LINE_MAX      /* Sentinel used for range checking - not a real line */
} EXTI_LineType;

/* ---------------- Sense (Trigger) Control ---------------- */
/**
 * @brief Chooses the signal condition that triggers the interrupt.
 *  - EXTI_SENSE_LOW_LEVEL  : Fires while the pin is held low  (INT0/INT1 only).
 *  - EXTI_SENSE_ANY_CHANGE : Fires on any logic change        (INT0/INT1 only).
 *  - EXTI_SENSE_FALLING    : Fires on a high-to-low transition (all lines).
 *  - EXTI_SENSE_RISING     : Fires on a low-to-high transition (all lines).
 * @note For INT2 only EXTI_SENSE_FALLING and EXTI_SENSE_RISING are valid.
 */
typedef enum
{
    EXTI_SENSE_LOW_LEVEL  = 0,
    EXTI_SENSE_ANY_CHANGE = 1,
    EXTI_SENSE_FALLING    = 2,
    EXTI_SENSE_RISING     = 3
} EXTI_SenseType;

/* ---------------- Configuration Structure ---------------- */
/**
 * @brief Parameters consumed by EXTI_Init().
 * @var EXTI_ConfigType::line   External interrupt line to configure (EXTI_LineType).
 * @var EXTI_ConfigType::sense  Trigger condition for that line (EXTI_SenseType).
 */
typedef struct
{
    EXTI_LineType  line;
    EXTI_SenseType sense;
} EXTI_ConfigType;

/* ---------------- Callback Pointer Type ---------------- */
/**
 * @brief Type of the user handler invoked from the EXTI ISR. Runs in interrupt
 *        context, so keep it short and avoid blocking calls.
 */
typedef void (*EXTI_CallBackType)(void);

/* ================================================================================
 *  FUNCTION PROTOTYPES
 * ============================================================================== */

/**
 * @brief  Configures the sense control for the selected line and enables it.
 * @param  addConfig  Pointer to a populated configuration structure.
 * @return STD_ReturnType  E_OK on success; E_NOK on NULL pointer, invalid line,
 *                         or an unsupported sense mode for that line (e.g. level
 *                         sensing requested on INT2).
 */
STD_ReturnType EXTI_Init(const EXTI_ConfigType *addConfig);

/**
 * @brief  Enables (unmasks) the selected external interrupt line in GICR.
 * @param  line  Interrupt line to enable.
 * @return STD_ReturnType  E_OK/E_NOK.
 */
STD_ReturnType EXTI_Enable(EXTI_LineType line);

/**
 * @brief  Disables (masks) the selected external interrupt line in GICR.
 * @param  line  Interrupt line to disable.
 * @return STD_ReturnType  E_OK/E_NOK.
 */
STD_ReturnType EXTI_Disable(EXTI_LineType line);

/**
 * @brief  Updates only the trigger condition (sense control) of a line without
 *         changing its enable state.
 * @param  line   Interrupt line to reconfigure.
 * @param  sense  New trigger condition.
 * @return STD_ReturnType  E_OK/E_NOK (E_NOK if sense unsupported for the line).
 */
STD_ReturnType EXTI_SetSenseControl(EXTI_LineType line, EXTI_SenseType sense);

/**
 * @brief  Registers the callback invoked when the given line fires.
 * @param  line      Interrupt line the callback belongs to.
 * @param  callBack  Pointer to a void(void) function; must not be NULL.
 * @return STD_ReturnType  E_OK/E_NOK.
 */
STD_ReturnType EXTI_SetCallBack(EXTI_LineType line, EXTI_CallBackType callBack);

/**
 * @brief  Sets the global interrupt enable bit (I-bit of SREG). Equivalent to sei().
 */
void EXTI_EnableGlobalInterrupt(void);

/**
 * @brief  Clears the global interrupt enable bit (I-bit of SREG). Equivalent to cli().
 */
void EXTI_DisableGlobalInterrupt(void);

#endif /* INTERRUPT_INTERFACE_H */
