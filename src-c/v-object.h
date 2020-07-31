#ifndef V_OBJECT_H
#define V_OBJECT_H

#include "v-type.h"

#define PROP_SIZE_OFF SIZE_U8
#define PROP_DATA_OFF PROP_SIZE_OFF
#define PROP_LEN_OFF  PROP_DATA_OFF
#define PROP_LINK_PTR_OFF PROP_LEN_OFF + SIZE_U64
#define PROP_DATA_PTR_OFF PROP_LINK_PTR_OFF + SIZE_U64
#define PROP_KEY_PTR_OFF  PROP_SIZE_OFF
#define PROP_VAL_PTR_OFF  PROP_KEY_PTR_OFF + SIZE_U64
#define PROP_TYPE(address)      *(u8 *) address
#define PROP_SIZE(address)      *(u8 *) (address + PROP_SIZE_OFF)
#define PROP_DATA(address)      *(u64 *) (address + PROP_DATA_OFF)
#define PROP_LEN(address)       *(u64 *) (address + PROP_LEN_OFF)
#define PROP_LINK_PTR(address)  *(u64 *) (address + PROP_LINK_PTR_OFF)
#define PROP_DATA_PTR(address)  *(u64 *) (address + PROP_DATA_PTR_OFF)
#define PROP_KEY_PTR(address)   *(u64 *) (address + PROP_KEY_PTR_OFF)
#define PROP_VAL_PTR(address)   *(u64 *) (address + PROP_VAL_PTR_OFF)
#undef  PROP_SIZE_OFF
#undef  PROP_DATA_OFF
#undef  PROP_LEN_OFF
#undef  PROP_LINK_PTR_OFF
#undef  PROP_DATA_PTR_OFF
#undef  PROP_KEY_PTR_OFF
#undef  PROP_VAL_PTR_OFF

#endif
