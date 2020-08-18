#include "Vexel.h"
/**
 * Copyright (c) 2019, 2020, Alphaharrius. All rights reserved.
 * 
 * v-data.c
 */

v_err 
v_make_data_object( v_pointer_object **ptr, 
                    u8 type, 
                    u64 dat) 
{
  v_err stat;
  stat = v_heap_allocate(ptr, SIZE_DAT_OBJ);
  if (IS_ERR(stat)) {
    return stat;
  }

  u8 * addr = (*ptr)->mem_addr;
  *V_TYPE(addr) = type;
  *V_DAT(addr) = dat;

  return stat;
}
