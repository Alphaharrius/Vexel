#include "Vexel.h"

boo 
v_make_var_object(u8 type, u32 *ptr_idx, u64 data) {
  u8 *obj_addr;
  if (!v_allocate_pointer(ptr_idx, &obj_addr, SIZE_VAR_OBJ)) {
    return false;
  }
  *PROP_TYPE(obj_addr) = type;
  *PROP_DATA(obj_addr) = data;

  return true;
}

boo 
v_make_list_object( u8 type, u32 *ptr_idx, 
                    u8 *data, u64 data_size, 
                    u32 list_len) {
  /**
   * Initialize element size based on list type.
   */
  u8 elem_size;
  switch (type) {
    /**
     * Size of integers and floats are of 8 bytes.
     */
    case OBJ_TYPE_LIST_INT:
    case OBJ_TYPE_LIST_FLT:
    elem_size = SIZE_64;
    break;
    /**
     * Size of pointer indexs are of 4 bytes.
     */
    case OBJ_TYPE_LIST_PTR:
    elem_size = SIZE_32;
    break;
    /**
     * Size of chars and opcodes are of 1 byte.
     */
    case OBJ_TYPE_LIST_STR:
    case OBJ_TYPE_LIST_OPC:
    elem_size = SIZE_8;
    break;
    /**
     * Other types are not allowed.
     */
    default:
    return false;
  }
  u64 list_size = SIZE_LIST_OBJ_BASE + data_size;
  /**
   * List size cannot be greater than heap size.
   */
  if (list_size > v_heap.total_byte_size) {
    return false;
  }
  u32 ptr_idx;
  u8 *sec_addr;

#define SEC_ALLOC(list_size, data_size) \
  if (!v_allocate_pointer(&ptr_idx, &sec_addr, list_size)) { \
    return false; \
  } \
  *PROP_TYPE(sec_addr) = type; \
  *PROP_SIZE(sec_addr) = elem_size; \
  *PROP_LEN(sec_addr) = list_len; \
  if (data) { \
    memcpy(PROP_DATA_ARRAY(sec_addr), data, data_size); \
  } else { \
    memset(PROP_DATA_ARRAY(sec_addr), 0, data_size); \
  }

  /**
   * If the allocation size is less than heap size, 
   * allocate the whole section directly.
   */
  if (list_size < HEAP_BLOCK_SIZE) {
    SEC_ALLOC(list_size, data_size);
    return true;
  }

#define SEC_SIZE HEAP_BLOCK_SIZE - SIZE_LIST_OBJ_BASE
  /**
   * The length of elements of one sector.
   */
  u16 sec_len = SEC_SIZE / elem_size;
  /**
   * The required sector count for this list, the remaining 
   * elements will be allocated after this iteration.
   */
  u32 sec_cnt = list_len / sec_len;
  u8 *prior_sec_addr;

#define LINK_TO_PRIOR() \
  if (! prior_sec_addr) { \
    *PROP_LINK_PTR_IDX(prior_sec_addr) = ptr_idx; \
  }

  /**
   * Allocate all sectors except the last one.
   */
  while (sec_cnt --) {
    /**
     * If the allocation fails, the prior allocations will 
     * not be freed instantly, as we assumes the garbage 
     * collection will collect all unused allocations.
     */
    SEC_ALLOC(HEAP_BLOCK_SIZE, SEC_SIZE);
    /**
     * Offset the data pointer to the next segment position.
     */
    data += SEC_SIZE;
    LINK_TO_PRIOR();
    /**
     * Set the prior sector pointer to the current pointer.
     */
    prior_sec_addr = sec_addr;
  }
#undef SEC_SIZE

  u16 rem_len = list_len % sec_len;
  /**
   * Skip if no remaining items.
   */
  if (!rem_len) {
    return true;
  }
  u16 rem_size = rem_len * elem_size;
  SEC_ALLOC(SIZE_LIST_OBJ_BASE + rem_size, rem_size);
  LINK_TO_PRIOR();

  return true;
}

boo 
v_make_map_object(u32 *ptr_idx, u32 map_len) {
  u8 *map_addr;
  if (!v_allocate_pointer(ptr_idx, map_addr, SIZE_MAP_OBJ)) {
    return false;
  }
  *PROP_TYPE(map_addr) = OBJ_TYPE_MAP;

  u64 data_size = map_len * SIZE_64;
  
  u32 key_ptr_idx;
  if (!v_make_list_object(OBJ_TYPE_LIST_PTR, &key_ptr_idx, NULL, data_size, map_len)) {
    return false;
  }
  *PROP_KEY_PTR_IDX(map_addr) = key_ptr_idx;

  u32 val_ptr_idx;
  if (!v_make_list_object(OBJ_TYPE_LIST_PTR, &val_ptr_idx, NULL, data_size, map_len)) {
    return false;
  }
  *PROP_VAL_PTR_IDX(map_addr) = val_ptr_idx;

  return true;
}
