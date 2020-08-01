#ifndef V_OBJECT_H
#define V_OBJECT_H

#include "v-type.h"

#define PROP_SIZE_OFF SIZE_U8
#define PROP_DATA_OFF PROP_SIZE_OFF
#define PROP_LEN_OFF  PROP_DATA_OFF
#define PROP_LINK_PTR_IDX_OFF PROP_LEN_OFF + SIZE_U64
#define PROP_DATA_PTR_OFF PROP_LINK_PTR_IDX_OFF + SIZE_U32
#define PROP_KEY_PTR_OFF  PROP_SIZE_OFF
#define PROP_VAL_PTR_OFF  PROP_KEY_PTR_OFF + SIZE_U64
#define PROP_TYPE(address) *(u8 *) address
#define PROP_SIZE(address) *(u8 *) (address + PROP_SIZE_OFF)
#define PROP_DATA(address) *(u64 *) (address + PROP_DATA_OFF)
#define PROP_LEN(address) *(u64 *) (address + PROP_LEN_OFF)
#define PROP_LINK_PTR_IDX(address) *(u32 *) (address + PROP_LINK_PTR_IDX_OFF)
#define PROP_DATA_PTR(address) *(u64 *) (address + PROP_DATA_PTR_OFF)
#define PROP_KEY_PTR(address) *(u64 *) (address + PROP_KEY_PTR_OFF)
#define PROP_VAL_PTR(address) *(u64 *) (address + PROP_VAL_PTR_OFF)

u8 *v_make_integer(u32 *ptr_idx, u64 data);
u8 *v_make_float(u32 *ptr_idx, u64 data);
u8 *v_make_buffer(u32 *ptr_idx, u64 buffer_size);
u8 *v_make_map(u32 *ptr_idx, u32 map_length);

#endif
