#ifndef STD_TYPES_H
#define STD_TYPES_H

typedef unsigned char       uint8;
typedef unsigned short      uint16;
typedef unsigned long       uint32;
typedef unsigned long long  uint64;

typedef signed char         sint8;
typedef signed short        sint16;
typedef signed long         sint32;
typedef signed long long    sint64;

typedef float               float32;
typedef double              float64;

typedef unsigned char       uint8_h;
typedef unsigned short      uint16_h;
typedef unsigned long       uint32_h;
typedef unsigned long long  uint64_h;

typedef unsigned char       u8;
typedef unsigned short      u16;
typedef unsigned long       u32;
typedef unsigned long long  u64;

#define NULL ((void *)0)

typedef enum
{
    E_OK    = 0,
    E_NOK   = 1
} STD_ReturnType;

#endif /* STD_TYPES_H */