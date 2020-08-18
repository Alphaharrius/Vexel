#ifndef V_DATA_H
#define V_DATA_H
/**
 * Copyright (c) 2019, 2020, Alphaharrius. All rights reserved.
 * 
 * v-data.h
 */

#include "v-type.h"
#include "v-error.h"
#include "v-heap.h"

#define OFF_DAT SIZE_8

#define SIZE_DAT_OB OFF_DAT + SIZE_64

#define V_BDAT(addr) V_BPTR(addr + OFF_DAT)
#define V_WDAT(addr) V_WPTR(addr + OFF_DAT)
#define V_DDAT(addr) V_DPTR(addr + OFF_DAT)
#define V_QDAT(addr) V_QPTR(addr + OFF_DAT)

v_err 
v_make_data_object( v_pointer_object **ptr, 
                    u8 type, 
                    u64 dat);

#endif
