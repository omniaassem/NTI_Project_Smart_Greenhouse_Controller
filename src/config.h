#ifndef CONFIG_H
#define CONFIG_H

#include "../MCAL/ADC/adc_interface.h"

/* Sensor ADC channels */
#define SENSOR_TEMP_CHANNEL   ADC_CHANNEL0
#define SENSOR_SOIL_CHANNEL   ADC_CHANNEL1
#define SENSOR_LIGHT_CHANNEL  ADC_CHANNEL2

/* ADC reference / prescaler for sensor sampling */
#define SENSOR_ADC_REFERENCE  ADC_REF_AVCC
#define SENSOR_ADC_PRESCALER  ADC_PRESCALER_64

#endif /* CONFIG_H */
