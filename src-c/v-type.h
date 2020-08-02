#ifndef V_TYPE_H
#define V_TYPE_H
/**
 * Copyright (c) 2019, 2020, Alphaharrius. All rights reserved.
 */
#include <stdint.h>

typedef uint8_t   u8;
typedef uint16_t  u16;
typedef uint32_t  u32;
typedef uint64_t  u64;

#define SIZE_8   0x01
#define SIZE_16  0x02
#define SIZE_32  0x04
#define SIZE_64  0x08

typedef u8 boo;
#define true  (boo) 0xff
#define false (boo) 0x00

#endif
