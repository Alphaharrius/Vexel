#include "../core/Vexel.h"
/**
 * Copyright (c) 2019, 2020, Alphaharrius. All rights reserved.
 * 
 * v-data.c
 */

Ve_Err 
Ve_CreateDataObject( VeObject **ob, 
                    u8 type, 
                    u64 dat) 
{
  if (MASK_NOT_DAT & type) {
    return ERR_API_INV_CALL;
  }

  Ve_Err stat;
  stat = Ve_HeapAllocate(ob, SIZE_DAT_OB);
  if (IS_ERR(stat)) {
    return stat;
  }

  u8 * addr = (*ob)->mem_addr;
  *Ve_TYPE(addr) = type;
  
  switch (type) {

    case OB_BOO:
    case OB_CHR:
    *Ve_BDAT(addr) = (u8) dat;
    break;

    case OB_INT:
    case OB_FLT:
    case OB_NAT:
    *Ve_QDAT(addr) = dat;

    default: break;
  }

  return stat;
}
