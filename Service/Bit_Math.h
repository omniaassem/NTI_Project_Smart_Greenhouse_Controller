#ifndef BIT_MATH_H
#define BIT_MATH_H

#define SET_BIT(REG, BIT)   ((REG) |=  (1U << (BIT)))
#define CLR_BIT(REG, BIT)   ((REG) &= ~(1U << (BIT)))
#define TOG_BIT(REG, BIT)   ((REG) ^=  (1U << (BIT)))
#define GET_BIT(REG, BIT)   (((REG) >> (BIT)) & 1U)

#endif /* BIT_MATH_H */
