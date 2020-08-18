#ifndef V_OBECT_H
#define V_OBECT_H
#pragma once
/**
 * Copyright (c) 2019, 2020, Alphaharrius. All rights reserved.
 * 
 * v-object.h
 */

#include "v-type.h"

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

#define V_BPTR(addr) (u8 *) (addr)
#define V_WPTR(addr) (u16 *) (addr)
#define V_DPTR(addr) (u32 *) (addr)
#define V_QPTR(addr) (u64 *) (addr)

#define V_TYPE(addr) V_BPTR(addr)

#define V_NOT_DAT 0xE0
#define V_NOT_LST 0xD0
#define V_NOT_MAP 0xB0

#define V_IS_DAT(addr) \
  !(V_NOT_DAT & *V_TYPE(addr))

#define V_IS_LST(addr) \
  !(V_NOT_LST & *V_TYPE(addr))

#define V_IS_MAP(addr) \
  !(V_NOT_MAP & *V_TYPE(addr))

#endif
