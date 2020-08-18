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

#define SIZE_DAT_OBJ  0x09

#define OFF_DAT SIZE_8

#define V_DAT(addr) (u64 *) (addr + OFF_DAT)

v_err 
v_make_data_object( v_pointer_object **ptr, 
                    u8 type, 
                    u64 dat);

#endif
