#ifndef V_OBJECT_H
#define V_OBJECT_H
#pragma once
/**
 * Copyright (c) 2019, 2020, Alphaharrius. All rights reserved.
 * 
 * v-object.h
 */

#include "v-type.h"

#define OBJ_TYPE_VAR  0x10
#define OBJ_TYPE_LST  0x20
#define OBJ_TYPE_MAP  0x40

#define OBJ_BOO OBJ_TYPE_VAR
#define OBJ_INT OBJ_TYPE_VAR + 0x01
#define OBJ_FLT OBJ_TYPE_VAR + 0x02
#define OBJ_NAT OBJ_TYPE_VAR + 0x03

#define OBJ_LST_INT OBJ_TYPE_LST
#define OBJ_LST_FLT OBJ_TYPE_LST + 0x01
#define OBJ_LST_STR OBJ_TYPE_LST + 0x02
#define OBJ_LST_OPC OBJ_TYPE_LST + 0x03
#define OBJ_LST_PTR OBJ_TYPE_LST + 0x04

#define OBJ_MAP OBJ_TYPE_MAP

#define V_BPTR(addr) (u8 *) (addr)
#define V_WPTR(addr) (u16 *) (addr)
#define V_DPTR(addr) (u32 *) (addr)
#define V_QPTR(addr) (u64 *) (addr)

#define V_TYPE(addr) V_BPTR(addr)

#define MASK_NOT_DAT  0xE0
#define MASK_NOT_LST  0xD0
#define MASK_NOT_MAP  0xB0

#define V_IS_DAT(addr) \
  !(MASK_NOT_DAT & *V_TYPE(addr))

#define V_IS_LST(addr) \
  !(MASK_NOT_LST & *V_TYPE(addr))

#define V_IS_MAP(addr) \
  !(MASK_NOT_MAP & *V_TYPE(addr))

#endif
