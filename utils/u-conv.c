#include "u-conv.h"
/**
 * Copyright (c) 2019, 2020, Alphaharrius. All rights reserved.
 * 
 * u-conv.c
 * This file implements the QuickUnsignedConvert algorithm.
 */

u8 
Util_QuickUnsignedConvert(u8 *str, u64 val, u8 disp, u8 pad)
{
#define U64_DIGITS 20

    if (disp > U64_DIGITS) {
        disp = U64_DIGITS;
    }

#undef U64_DIGITS

    u8 *base = str;
    str += disp;

    while (val != 0) {
        *--str = '0' + (u8) (val % 10);
        val /= 10;
    }

    size_t pad_size = str > base ? str - base : 0;
    if (pad) {

        memset(base, '0', pad_size);
        return disp;
    } 
    else {
        
        u8 len = disp - pad_size;
        memcpy(base, str, len);
        memset(base + len, 0, pad_size);

        return len;
    }
}
