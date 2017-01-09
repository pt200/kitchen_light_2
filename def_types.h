#ifndef def_types_H
#define def_types_H

/************************************************************
* STANDARD BITS
************************************************************/

#define BIT0                (0x01)
#define BIT1                (0x02)
#define BIT2                (0x04)
#define BIT3                (0x08)
#define BIT4                (0x10)
#define BIT5                (0x20)
#define BIT6                (0x40)
#define BIT7                (0x80)
#define BIT8                (0x0100u)
#define BIT9                (0x0200u)
#define BITA                (0x0400u)
#define BITB                (0x0800u)
#define BITC                (0x1000u)
#define BITD                (0x2000u)
#define BITE                (0x4000u)
#define BITF                (0x8000u)

typedef unsigned char UREG;
typedef signed   char REG;

typedef unsigned short UREG16;
typedef signed   short REG16;

typedef unsigned long UREG32;
typedef signed   long REG32;


#define uint8  unsigned char
#define int8   signed char        

#define uint16 unsigned short
#define int16  signed short

#define uint32 unsigned long
#define int32  signed long

#define BOOL   char

#endif
