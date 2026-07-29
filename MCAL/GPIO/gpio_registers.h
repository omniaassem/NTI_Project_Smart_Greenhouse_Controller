#ifndef GPIO_REGISTERS_H
#define GPIO_REGISTERS_H

#include "../../Service/STD_Types.h"

#define GPIO_NUMBER_OF_PORTS    4
#define GPIO_NUMBER_OF_PINS     8

#define GPIO_PINA          (*(volatile u8 *)0x39)
#define GPIO_DDRA          (*(volatile u8 *)0x3A)
#define GPIO_PORTA_REG     (*(volatile u8 *)0x3B)

#define GPIO_PINB          (*(volatile u8 *)0x36)
#define GPIO_DDRB          (*(volatile u8 *)0x37)
#define GPIO_PORTB_REG     (*(volatile u8 *)0x38)

#define GPIO_PINC          (*(volatile u8 *)0x33)
#define GPIO_DDRC          (*(volatile u8 *)0x34)
#define GPIO_PORTC_REG     (*(volatile u8 *)0x35)

#define GPIO_PIND          (*(volatile u8 *)0x30)
#define GPIO_DDRD          (*(volatile u8 *)0x31)
#define GPIO_PORTD_REG     (*(volatile u8 *)0x32)


typedef enum
{
    PIN_LOW    = 0,
    PIN_HIGH   = 1
} GPIO_PINStatus;

typedef unsigned char      GPIO_PortStatus;

#endif // GPIO_REGISTERS_H

