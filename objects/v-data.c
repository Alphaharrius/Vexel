#include "../core/Vexel.h"
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
  if (V_NOT_DAT & type) {
    return V_ERR_API_INV_CALL;
  }

  v_err stat;
  stat = v_heap_allocate(ptr, SIZE_DAT_OB);
  if (IS_ERR(stat)) {
    return stat;
  }

  u8 * addr = (*ptr)->mem_addr;
  *V_TYPE(addr) = type;
  
  switch (type) {

    case OB_BOO:
    case OB_CHR:
    *V_BDAT(addr) = (u8) dat;
    break;

    case OB_INT:
    case OB_FLT:
    case OB_NAT:
    *V_QDAT(addr) = dat;

    default: break;
  }

  return stat;
}
