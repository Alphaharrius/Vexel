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

#define PROP_DAT_OFF  0x01
#define SIZE_DAT_OBJ  PROP_DAT_OFF + SIZE_64

#define PROP_DAT(addr) (u64 *) (addr + PROP_DAT_OFF)

v_err 
v_make_data_object( v_pointer_object **ptr, 
                    u8 type, 
                    u64 dat);

#endif
