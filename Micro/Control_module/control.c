#include "control.h"
#include "../../Service/STD_TYPES.h"
#include "../../HAL/Actuators/Actuators_Driver.h"
#include "../../HAL/Sensors/Sensors_Driver.h"

CONTROL_Status_t CTRL_Init(void)
{
    /* Initialize control loops and actuator state here */
    return CONTROL_OK;
}

CONTROL_Status_t CTRL_Update(void)
{
    /* Read sensors, apply hysteresis, and update actuators here */
    u16 rawTemp = 0u;
    u16 rawSoil = 0u;
    u16 rawLight = 0u;

    if (Sensors_ReadRaw(&rawTemp, &rawSoil, &rawLight) != E_OK)
    {
        return CONTROL_ERROR;
    }

    return CONTROL_OK;
}