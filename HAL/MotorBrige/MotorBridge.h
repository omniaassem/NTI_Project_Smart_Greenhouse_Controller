#ifndef MOTOR_BRIDGE_H
#define MOTOR_BRIDGE_H

#include "STD_Types.h"

/**************************************************************************/
/* Function Prototypes                                                    */
/**************************************************************************/

STD_ReturnType BRIDGE_Init(void);
void BRIDGE_Forward(void);
void BRIDGE_Backward(void);
void BRIDGE_ForceStop(void);

#endif /* MOTOR_BRIDGE_H */