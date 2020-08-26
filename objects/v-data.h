#ifndef V_DATA_H
#define V_DATA_H
/**
 * Copyright (c) 2019, 2020, Alphaharrius. All rights reserved.
 * 
 * v-data.h
 */

#include "../core/v-type.h"
#include "../core/v-error.h"
#include "../core/v-heap.h"

#define OFF_DAT SIZE_8

#define SIZE_DAT_OB OFF_DAT + SIZE_64

#define Ve_BDAT(addr) Ve_BPTR(addr + OFF_DAT)
#define Ve_WDAT(addr) Ve_WPTR(addr + OFF_DAT)
#define Ve_DDAT(addr) Ve_DPTR(addr + OFF_DAT)
#define Ve_QDAT(addr) Ve_QPTR(addr + OFF_DAT)

Ve_Err 
Ve_CreateDataObject(VeObject **ob, 
                    u8 type, 
                    u64 dat);

#endif
