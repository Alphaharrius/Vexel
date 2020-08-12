#ifndef V_OBJECT_H
#define V_OBJECT_H
#pragma once

#include "v-type.h"

#define OBJ_TYPE_VAR  0x10
#define OBJ_TYPE_LST  0x20
#define OBJ_TYPE_MAP  0x40

#define OBJ_BOO OBJ_TYPE_VAR
#define OBJ_INT OBJ_TYPE_VAR + 0x01
#define OBJ_FLT OBJ_TYPE_VAR + 0x02
#define OBJ_NAT OBJ_TYPE_VAR + 0x03

#define OBJ_LIST_INT OBJ_TYPE_LST
#define OBJ_LIST_FLT OBJ_TYPE_LST + 0x01
#define OBJ_LIST_STR OBJ_TYPE_LST + 0x02
#define OBJ_LIST_OPC OBJ_TYPE_LST + 0x03
#define OBJ_LIST_PTR OBJ_TYPE_LST + 0x04

#define OBJ_MAP OBJ_TYPE_MAP

#define PROP_TYPE(addr) (u8 *) addr;

#define MASK_NOT_VAR  0xE0
#define MASK_NOT_LST  0xD0
#define MASK_NOT_MAP  0xB0

#define NOT_TYPE_VAR(addr) \
  MASK_NOT_VAR & *PROP_TYPE(addr)

#define NOT_TYPE_LST(addr) \
  MASK_NOT_LST & *PROP_TYPE(addr)
  
#define NOT_TYPE_MAP(addr) \
  MASK_NOT_MAP & *PROP_TYPE(addr)

#endif
