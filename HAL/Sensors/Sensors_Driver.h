#ifndef SENSORS_DRIVER_H
#define SENSORS_DRIVER_H

#include "../../Service/STD_Types.h"

STD_ReturnType Sensors_Init(void);
STD_ReturnType Sensors_ReadRaw(uint16_h *tempRaw,
                              uint16_h *soilRaw,
                              uint16_h *lightRaw);
STD_ReturnType Sensors_ScaleTempC(uint16_h raw, uint8_h *tempC);
STD_ReturnType Sensors_ScalePct(uint16_h raw, uint8_h *percent);

#endif /* SENSORS_DRIVER_H */
