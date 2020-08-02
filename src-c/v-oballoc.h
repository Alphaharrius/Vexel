#ifndef V_OBJECT_H
#define V_OBJECT_H

#include "v-type.h"

#define OBJ_TYPE_BOOL   0x01
#define OBJ_TYPE_INT    0x02
#define OBJ_TYPE_FLOAT  0x03
#define OBJ_TYPE_NATIVE 0x04

#define OBJ_TYPE_LIST     0x05
#define OBJ_TYPE_LIST_INT 0x06
#define OBJ_TYPE_LIST_FLT 0x07
#define OBJ_TYPE_LIST_STR 0x08
#define OBJ_TYPE_LIST_OPC 0x09
#define OBJ_TYPE_LIST_PTR 0x0a

#define OBJ_TYPE_MAP    0x0b
/**
 * ALL: Offset 1 byte, from the "type" property.
 */
#define PROP_SIZE_OFF SIZE_8
/**
 * VAR: Offset 1 byte, from the "type" property.
 */
#define PROP_DATA_OFF PROP_SIZE_OFF
/**
 * LIST: Offset 1 byte, from the "size" property.
 */
#define PROP_LEN_OFF  PROP_SIZE_OFF + SIZE_8
/**
 * LIST: Offset 8 bytes, from the "len" property.
 */
#define PROP_LINK_PTR_IDX_OFF PROP_LEN_OFF + SIZE_32
/**
 * LIST: Offset 4 bytes, from the "link_ptr_idx" property.
 */
#define PROP_DATA_ARRAY_OFF PROP_LINK_PTR_IDX_OFF + SIZE_32
/**
 * MAP: Offset 1 byte, from the "size" property.
 */
#define PROP_KEY_PTR_IDX_OFF  PROP_SIZE_OFF
/**
 * MAP: Offset 8 bytes, from the "key_ptr_idx" property.
 */
#define PROP_VAL_PTR_IDX_OFF  PROP_KEY_PTR_IDX_OFF + SIZE_32
/**
 * ALL: "type" property, defines the type of the object.
 *  The types of object:  
 *    VAR: boolean, integer, float, native
 *      - Singlar data storage object.
 *    LIST: string, integer list, float list, opcode list, pointer list
 *      - Continous data array storage object.
 *    MAP: map
 *      - Key-Value mapped object.
 */
#define PROP_TYPE(addr) (u8 *) addr
/**
 * LIST: "size" property, defines the size of each element in the list.
 */
#define PROP_SIZE(addr) (u8 *) (addr + PROP_SIZE_OFF)
/**
 * VAR: "data" property defines the data of the variable.
 */
#define PROP_DATA(addr) (u64 *) (addr + PROP_DATA_OFF)
/**
 * LIST: "len" property defines the length of the list.
 */
#define PROP_LEN(addr) (u32 *) (addr + PROP_LEN_OFF)
/**
 * LIST:  "link_ptr_idx" property defines the pointer index 
 *        of the next section of the list object, used for 
 *        lists those length longer than one block size.
 */
#define PROP_LINK_PTR_IDX(addr) (u32 *) (addr + PROP_LINK_PTR_IDX_OFF)
/**
 * LIST:  "data_array" property stores the data array of the list, 
 *        the size of this property is defined in the "len" property.
 */
#define PROP_DATA_ARRAY(addr) (u64 *) (addr + PROP_DATA_ARRAY_OFF)
/**
 * MAP: "key_ptr_idx" property stores the pointer index of the key list.
 */
#define PROP_KEY_PTR_IDX(addr) (u64 *) (addr + PROP_KEY_PTR_IDX_OFF)
/**
 * MAP: "val_ptr_idx" property stores the pointer index of the value list.
 */
#define PROP_VAL_PTR_IDX(addr) (u64 *) (addr + PROP_VAL_PTR_IDX_OFF)

#define SIZE_VAR_OBJ PROP_DATA_OFF + SIZE_64
#define SIZE_LIST_OBJ_BASE PROP_DATA_ARRAY_OFF
#define SIZE_MAP_OBJ PROP_VAL_PTR_IDX_OFF + SIZE_32

/**
 * Create a var object in the heap.
 * @param type: The type of the var object, no enforcement is in place.
 * @param ptr_idx: The u32 to store the pointer index of the object allocation.
 * @param data: The data to be stored in this object.
 * @return: Status of the operation in boolean.
 */
boo v_make_var_object(u8 type, u32 *ptr_idx, u64 data);
/**
 * Create a list object in the heap.
 * @param type: The type of the list object, must be one of the type list.
 * @param ptr_idx: The u32 to store the pointer index of the object allocation.
 * @param data: The data list to be stored in this list object, NULL is acceptable.
 * @param list_len: The length of the list elements.
 * @return: Status of the operation in boolean.
 */
boo v_make_list_object(u8 type, u32 *ptr_idx, u8 *data, u32 list_len);
/**
 * Create a map object in the heap.
 * @param ptr_idx: The u32 to store the pointer index of the object allocation.
 * @param map_len: The length of the map's key-value pair.
 * @return: Status of the operation in boolean.
 */
boo v_make_map_object(u32 *ptr_idx, u32 map_len);

#endif
