#ifndef V_OBECT_H
#define V_OBECT_H
#pragma once
/**
 * Copyright (c) 2019, 2020, Alphaharrius. All rights reserved.
 * 
 * v-object.h
 */

#include "../core/v-type.h"

#define OB_TYPE_DAT  0x10
#define OB_TYPE_LST  0x20
#define OB_TYPE_MAP  0x40

#define OB_BOO OB_TYPE_DAT + 0x00
#define OB_CHR OB_TYPE_DAT + 0x01
#define OB_INT OB_TYPE_DAT + 0x02
#define OB_FLT OB_TYPE_DAT + 0x03
#define OB_NAT OB_TYPE_DAT + 0x04

#define OB_LST_INT OB_TYPE_LST
#define OB_LST_FLT OB_TYPE_LST + 0x01
#define OB_LST_CHR OB_TYPE_LST + 0x02
#define OB_LST_BYT OB_TYPE_LST + 0x03
#define OB_LST_PTR OB_TYPE_LST + 0x04

#define OB_MAP OB_TYPE_MAP

#define Ve_BPTR(addr) (u8 *) (addr)
#define Ve_WPTR(addr) (u16 *) (addr)
#define Ve_DPTR(addr) (u32 *) (addr)
#define Ve_QPTR(addr) (u64 *) (addr)

#define Ve_TYPE(addr) Ve_BPTR(addr)

#define MASK_NOT_DAT 0xE0
#define MASK_NOT_LST 0xD0
#define MASK_NOT_MAP 0xB0

#define Ve_ISDAT(addr) \
  !(MASK_NOT_DAT & *Ve_TYPE(addr))

#define Ve_ISLST(addr) \
  !(MASK_NOT_LST & *Ve_TYPE(addr))

#define Ve_ISMAP(addr) \
  !(MASK_NOT_MAP & *Ve_TYPE(addr))

#endif
