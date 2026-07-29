#ifndef GPIO_REGISTERS_H
#define GPIO_REGISTERS_H

#include "../../Service/STD_Type.h"

#define GPIO_NUMBER_OF_PORTS    4
#define GPIO_NUMBER_OF_PINS     8

// Data Direction Register A
#define GPIO_DDRA         (*(volatile uint8_t *) 0x3A) 
// Data Register A 
#define GPIO_PORTA_REG    (*(volatile uint8_t *) 0x3B) 
// Input Pins Address A
#define GPIO_PINA         (*(volatile uint8_t *) 0x39) 

// Data Direction Register B
#define GPIO_DDRB         (*(volatile uint8_t *) 0x37) 
// Data Register B 
#define GPIO_PORTB_REG    (*(volatile uint8_t *) 0x38) 
// Input Pins Address B
#define GPIO_PINB         (*(volatile uint8_t *) 0x36) 

// Data Direction Register C
#define GPIO_DDRC         (*(volatile uint8_t *) 0x34) 
// Data Register C 
#define GPIO_PORTC_REG    (*(volatile uint8_t *) 0x35) 
// Input Pins Address C
#define GPIO_PINC         (*(volatile uint8_t *) 0x33) 

// Data Direction Register D
#define GPIO_DDRD         (*(volatile uint8_t *) 0x31) 
// Data Register D 
#define GPIO_PORTD_REG    (*(volatile uint8_t *) 0x32) 
// Input Pins Address D
#define GPIO_PIND         (*(volatile uint8_t *) 0x30) 

#endif