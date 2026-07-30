#include "../../Service/STD_Types.h"
#include "../../Service/Bit_Math.h"
#include "interrupt_registers.h"
#include "interrupt_interface.h"

/* ================================================================================
 *  EXTERNAL INTERRUPT DRIVER - IMPLEMENTATION
 * ============================================================================== */

/* ISR Vector definitions for ATmega32 GCC Compiler */
void __vector_1(void) __attribute__((signal)); /* INT0 ISR */
void __vector_2(void) __attribute__((signal)); /* INT1 ISR */
void __vector_3(void) __attribute__((signal)); /* INT2 ISR */

/* Array to store callback function pointers for each line (INT0, INT1, INT2) */
static EXTI_CallBackType EXTI_CallBacks[EXTI_LINE_MAX] = { NULL, NULL, NULL };

STD_ReturnType EXTI_Init(const EXTI_ConfigType *addConfig)
{
    STD_ReturnType local_Status = E_OK;

    /* STEP 1: Validate input */
    if ((addConfig == NULL) || (addConfig->line >= EXTI_LINE_MAX))
    {
        local_Status = E_NOK;
    }
    else
    {
        /* STEP 2: Configure sense control */
        if (EXTI_SetSenseControl(addConfig->line, addConfig->sense) == E_NOK)
        {
            local_Status = E_NOK;
        }
        else
        {
            /* STEP 3: Clear any pending stale flag by writing 1 to it */
            switch (addConfig->line)
            {
                case EXTI_INT0: SET_BIT(EXTI_GIFR_REG, EXTI_INTF0_BIT); break;
                case EXTI_INT1: SET_BIT(EXTI_GIFR_REG, EXTI_INTF1_BIT); break;
                case EXTI_INT2: SET_BIT(EXTI_GIFR_REG, EXTI_INTF2_BIT); break;
                default: break;
            }

            /* STEP 4: Enable the interrupt line */
            (void)EXTI_Enable(addConfig->line);
        }
    }

    return local_Status;
}

STD_ReturnType EXTI_Enable(EXTI_LineType line)
{
    STD_ReturnType local_Status = E_OK;

    if (line >= EXTI_LINE_MAX)
    {
        local_Status = E_NOK;
    }
    else
    {
        switch (line)
        {
            case EXTI_INT0: SET_BIT(EXTI_GICR_REG, EXTI_INT0_BIT); break;
            case EXTI_INT1: SET_BIT(EXTI_GICR_REG, EXTI_INT1_BIT); break;
            case EXTI_INT2: SET_BIT(EXTI_GICR_REG, EXTI_INT2_BIT); break;
            default: local_Status = E_NOK; break;
        }
    }

    return local_Status;
}

STD_ReturnType EXTI_Disable(EXTI_LineType line)
{
    STD_ReturnType local_Status = E_OK;

    if (line >= EXTI_LINE_MAX)
    {
        local_Status = E_NOK;
    }
    else
    {
        switch (line)
        {
            case EXTI_INT0: CLR_BIT(EXTI_GICR_REG, EXTI_INT0_BIT); break;
            case EXTI_INT1: CLR_BIT(EXTI_GICR_REG, EXTI_INT1_BIT); break;
            case EXTI_INT2: CLR_BIT(EXTI_GICR_REG, EXTI_INT2_BIT); break;
            default: local_Status = E_NOK; break;
        }
    }

    return local_Status;
}

STD_ReturnType EXTI_SetSenseControl(EXTI_LineType line, EXTI_SenseType sense)
{
    STD_ReturnType local_Status = E_OK;

    if (line >= EXTI_LINE_MAX)
    {
        local_Status = E_NOK;
    }
    else
    {
        switch (line)
        {
            case EXTI_INT0:
                /* Clear existing sense bits for INT0 */
                CLR_BIT(EXTI_MCUCR_REG, EXTI_ISC00_BIT);
                CLR_BIT(EXTI_MCUCR_REG, EXTI_ISC01_BIT);

                switch (sense)
                {
                    case EXTI_SENSE_LOW_LEVEL:   /* 00 -> Already cleared */ break;
                    case EXTI_SENSE_ANY_CHANGE: SET_BIT(EXTI_MCUCR_REG, EXTI_ISC00_BIT); break; /* 01 */
                    case EXTI_SENSE_FALLING:    SET_BIT(EXTI_MCUCR_REG, EXTI_ISC01_BIT); break; /* 10 */
                    case EXTI_SENSE_RISING:     
                        SET_BIT(EXTI_MCUCR_REG, EXTI_ISC00_BIT); 
                        SET_BIT(EXTI_MCUCR_REG, EXTI_ISC01_BIT); 
                        break; /* 11 */
                    default: local_Status = E_NOK; break;
                }
                break;

            case EXTI_INT1:
                /* Clear existing sense bits for INT1 */
                CLR_BIT(EXTI_MCUCR_REG, EXTI_ISC10_BIT);
                CLR_BIT(EXTI_MCUCR_REG, EXTI_ISC11_BIT);

                switch (sense)
                {
                    case EXTI_SENSE_LOW_LEVEL:   /* 00 -> Already cleared */ break;
                    case EXTI_SENSE_ANY_CHANGE: SET_BIT(EXTI_MCUCR_REG, EXTI_ISC10_BIT); break; /* 01 */
                    case EXTI_SENSE_FALLING:    SET_BIT(EXTI_MCUCR_REG, EXTI_ISC11_BIT); break; /* 10 */
                    case EXTI_SENSE_RISING:     
                        SET_BIT(EXTI_MCUCR_REG, EXTI_ISC10_BIT); 
                        SET_BIT(EXTI_MCUCR_REG, EXTI_ISC11_BIT); 
                        break; /* 11 */
                    default: local_Status = E_NOK; break;
                }
                break;

            case EXTI_INT2:
                /* INT2 only supports Falling and Rising edges */
                if (sense == EXTI_SENSE_FALLING)
                {
                    CLR_BIT(EXTI_MCUCSR_REG, EXTI_ISC2_BIT);
                }
                else if (sense == EXTI_SENSE_RISING)
                {
                    SET_BIT(EXTI_MCUCSR_REG, EXTI_ISC2_BIT);
                }
                else
                {
                    /* Low level & Any change are not supported on INT2 */
                    local_Status = E_NOK;
                }
                break;

            default:
                local_Status = E_NOK;
                break;
        }
    }

    return local_Status;
}

STD_ReturnType EXTI_SetCallBack(EXTI_LineType line, EXTI_CallBackType callBack)
{
    STD_ReturnType local_Status = E_OK;

    if ((line >= EXTI_LINE_MAX) || (callBack == NULL))
    {
        local_Status = E_NOK;
    }
    else
    {
        EXTI_CallBacks[line] = callBack;
    }

    return local_Status;
}

void EXTI_EnableGlobalInterrupt(void)
{
    SET_BIT(EXTI_SREG_REG, EXTI_GLOBAL_INT_BIT);
}

void EXTI_DisableGlobalInterrupt(void)
{
    CLR_BIT(EXTI_SREG_REG, EXTI_GLOBAL_INT_BIT);
}

/* ================================================================================
 *  INTERRUPT SERVICE ROUTINES (ISRs)
 * ============================================================================== */

/* ISR for External Interrupt 0 (PD2) */
void __vector_1(void)
{
    if (EXTI_CallBacks[EXTI_INT0] != NULL)
    {
        EXTI_CallBacks[EXTI_INT0]();
    }
}

/* ISR for External Interrupt 1 (PD3) */
void __vector_2(void)
{
    if (EXTI_CallBacks[EXTI_INT1] != NULL)
    {
        EXTI_CallBacks[EXTI_INT1]();
    }
}

/* ISR for External Interrupt 2 (PB2) */
void __vector_3(void)
{
    if (EXTI_CallBacks[EXTI_INT2] != NULL)
    {
        EXTI_CallBacks[EXTI_INT2]();
    }
}
