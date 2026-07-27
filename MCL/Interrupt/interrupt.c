#include "../../Service/STD_Types.h"
#include "../../Service/Bit_Math.h"
#include "interrupt_registers.h"
#include "interrupt_interface.h"

/* ================================================================================
 *  EXTERNAL INTERRUPT DRIVER - IMPLEMENTATION SKELETON
 *  ------------------------------------------------------------------------------
 *  Each body below lists the ordered steps to implement the function. Replace the
 *  numbered comments with the actual register manipulation code.
 * ============================================================================== */

/*
 * Callback storage: one slot per external line (INT0, INT1, INT2).
 * TODO: define a static array initialised to NULL, e.g.
 *       static EXTI_CallBackType EXTI_CallBacks[EXTI_LINE_MAX] = { NULL, NULL, NULL };
 */


STD_ReturnType EXTI_Init(const EXTI_ConfigType *addConfig)
{
    /*
     * STEP 1: Validate the input.
     *   - If addConfig == NULL, return E_NOK.
     *   - If addConfig->line >= EXTI_LINE_MAX, return E_NOK.
     *
     * STEP 2: Program the sense control by calling
     *         EXTI_SetSenseControl(addConfig->line, addConfig->sense).
     *         If it returns E_NOK (unsupported sense for that line), forward E_NOK.
     *
     * STEP 3: Clear any stale pending flag for the line by writing 1 to its GIFR bit
     *         (INTF0/INTF1/INTF2) so an old event does not fire immediately.
     *
     * STEP 4: Enable the line by calling EXTI_Enable(addConfig->line).
     *
     * STEP 5: Return E_OK.
     */
    return E_NOK;
}


STD_ReturnType EXTI_Enable(EXTI_LineType line)
{
    /*
     * STEP 1: Validate line < EXTI_LINE_MAX (else E_NOK).
     * STEP 2: Set the matching enable bit in GICR:
     *         EXTI_INT0 -> INT0 bit, EXTI_INT1 -> INT1 bit, EXTI_INT2 -> INT2 bit.
     * STEP 3: Return E_OK.
     */
    return E_NOK;
}


STD_ReturnType EXTI_Disable(EXTI_LineType line)
{
    /*
     * STEP 1: Validate line < EXTI_LINE_MAX (else E_NOK).
     * STEP 2: Clear the matching enable bit in GICR (same mapping as EXTI_Enable).
     * STEP 3: Return E_OK.
     */
    return E_NOK;
}


STD_ReturnType EXTI_SetSenseControl(EXTI_LineType line, EXTI_SenseType sense)
{
    /*
     * STEP 1: Validate line < EXTI_LINE_MAX (else E_NOK).
     *
     * STEP 2: Handle each line separately:
     *
     *   INT0 (bits ISC01:ISC00 in MCUCR):
     *     - EXTI_SENSE_LOW_LEVEL  -> 00
     *     - EXTI_SENSE_ANY_CHANGE -> 01
     *     - EXTI_SENSE_FALLING    -> 10
     *     - EXTI_SENSE_RISING     -> 11
     *     Clear both bits first, then set according to the table.
     *
     *   INT1 (bits ISC11:ISC10 in MCUCR): same 2-bit encoding as INT0.
     *
     *   INT2 (single bit ISC2 in MCUCSR):
     *     - EXTI_SENSE_FALLING    -> clear ISC2
     *     - EXTI_SENSE_RISING     -> set   ISC2
     *     - LOW_LEVEL / ANY_CHANGE are NOT supported -> return E_NOK.
     *
     * STEP 3: Return E_OK (or E_NOK for an unsupported sense/line combination).
     */
    return E_NOK;
}


STD_ReturnType EXTI_SetCallBack(EXTI_LineType line, EXTI_CallBackType callBack)
{
    /*
     * STEP 1: Validate line < EXTI_LINE_MAX and callBack != NULL (else E_NOK).
     * STEP 2: Store 'callBack' in EXTI_CallBacks[line].
     * STEP 3: Return E_OK.
     *
     * NOTE: Provide the three ISRs here so each fires its stored callback:
     *       ISR(INT0_vect){ if (EXTI_CallBacks[EXTI_INT0]) EXTI_CallBacks[EXTI_INT0](); }
     *       ISR(INT1_vect){ ... }   ISR(INT2_vect){ ... }
     */
    return E_NOK;
}


void EXTI_EnableGlobalInterrupt(void)
{
    /*
     * STEP 1: Set the I-bit (bit 7) of SREG:
     *         SET_BIT(EXTI_SREG_REG, EXTI_GLOBAL_INT_BIT);   // same as sei()
     */
}


void EXTI_DisableGlobalInterrupt(void)
{
    /*
     * STEP 1: Clear the I-bit (bit 7) of SREG:
     *         CLR_BIT(EXTI_SREG_REG, EXTI_GLOBAL_INT_BIT);   // same as cli()
     */
}
