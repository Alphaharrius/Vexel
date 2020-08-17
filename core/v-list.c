#include "Vexel.h"
/**
 * Copyright (c) 2019, 2020, Alphaharrius. All rights reserved.
 * 
 * v-list.c
 */

#define LST_TLEN_GROW(tot_len) \
  tot_len = (tot_len << 1) - (tot_len >> 1);

v_err 
v_make_list_object( v_pointer_object **ptr, 
                    u8 type, 
                    u32 len, 
                    u8 *dptr)
{
  /**
   * TEMPORARY:
   * The current implementation 
   * of list will only allow list 
   * length up to 2^32 to ensure 
   * arrays can be allocated.
   */
  if (len > LST_MAX_LEN) {
    return V_ERR_LST_MAX_LEN;
  }

  u8 el_size;
  /**
   * The size of list element will only be 
   * of the following types as list object 
   * have restricted child types.
   */
  switch (type) {
    /**
     * Size of char and byte type are 1.
     */
    case OBJ_LST_STR:
    case OBJ_LST_OPC: el_size = SIZE_8; break;
    /**
     * Size of pointer index is 4.
     */
    case OBJ_LST_PTR: el_size = SIZE_32; break;
    /**
     * Size of int and float type are 8.
     */
    case OBJ_LST_INT:
    case OBJ_LST_FLT: el_size = SIZE_64; break;
    /**
     * Types out of the scope of list type 
     * will be rejected, this checking is 
     * for debug purpose only as the compiler 
     * will not generate invalid types.
     */
    default: return V_ERR_API_INV_PARAM;
  }

  /**
   * The initial total length 
   * of a list object is 2.
   */
  u32 tot_len = 2;
  while (len > tot_len) {
    /**
     * The total length of list is 
     * calculated 1.5x of 2, this 
     * value will grow until the 
     * list length is satisfied.
     */
    LST_TLEN_GROW(tot_len);
  }

  v_err status;
  u64 obj_size = SIZE_LST_OBJ + el_size * tot_len;
  if (obj_size > v_heap.obj_space_size) {
    return V_ERR_HEAP_OUT_OF_MEM;
  }

  status = v_heap_allocate(ptr, obj_size);
  if (status != V_ERR_NONE) {
    return status;
  }

  u8 *obj_addr = (*ptr)->mem_addr;
  *PROP_TYPE(obj_addr) = type;
  *PROP_ESIZE(obj_addr) = el_size;
  *PROP_LEN(obj_addr) = len;
  *PROP_TLEN(obj_addr) = tot_len;

  if (len && dptr) {
    memcpy(PROP_DPTR(obj_addr), dptr, el_size * len);
  }

  return status;
}

v_err 
v_list_expand(v_pointer_object *lst_ptr,
              u8 **addr)
{
  /**
   * Check if the list pointer is the global null 
   * pointer, which takes the base position.
   */
  if (is_null_pointer(lst_ptr)) {
    return V_ERR_OBJ_NULL;
  }

  u8 *lst_addr = lst_ptr->mem_addr;
  /**
   * Check if the pointer is type of list.
   */
  if (NOT_TYPE_LST(lst_addr)) {
    return V_ERR_OBJ_NOT_LST;
  }

  u8 el_size = *PROP_ESIZE(lst_addr);

  /**
   * Using pointer for better handling.
   */
  u32 *len = PROP_LEN(lst_addr);
  u32 tot_len = *PROP_TLEN(lst_addr);

  /**
   * Check if the length is smaller than 
   * the allocated total length.
   */
  if (*len >= tot_len) {
    /**
     * Allocate a new memory chunk for the 
     * list with extra allocated length.
     */
    LST_TLEN_GROW(tot_len);
    u64 new_size = SIZE_LST_OBJ + tot_len * el_size;
    v_err status = v_heap_reallocate(lst_ptr, new_size);
    if (status != V_ERR_NONE) {
      return status;
    }

    lst_addr = lst_ptr->mem_addr;
    /**
     * Set the length pointer to 
     * the location within the 
     * new allocation.
     */
    len = PROP_LEN(lst_addr);
    /**
     * Only the total length requires an update.
     */
    *PROP_TLEN(lst_addr) = tot_len;
  }

  /**
   * Set the return address to the last 
   * element address of the list.
   */
  *addr = PROP_DPTR(lst_addr) + (*len)++ * el_size;

  return V_ERR_NONE;
}

static inline u8 
match_type(u8 lst_type, u8 type)
{
  switch (lst_type) {

    case OBJ_LST_INT: return type == OBJ_INT;

    case OBJ_LST_FLT: return type == OBJ_FLT;

    case OBJ_LST_PTR: return TRUE;

    case OBJ_LST_STR:
    case OBJ_LST_OPC:
    default: return FALSE;

  }
}

v_err 
v_list_push(v_pointer_object *lst_ptr, 
            v_pointer_object *ptr)
{
  u8 *lst_addr = lst_ptr->mem_addr;
  u8 *obj_addr = ptr->mem_addr;
  u8 lst_type = *PROP_TYPE(lst_addr);
  if (!match_type(lst_type, *PROP_TYPE(obj_addr))) {
    return V_ERR_OBJ_TYP_UNMATCH;
  }

  v_err status;
  u8 *psh_addr;
  status = v_list_expand(lst_ptr, &psh_addr);
  if (status != V_ERR_NONE) {
    return status;
  }

  switch (lst_type) {

    case OBJ_LST_INT:
    case OBJ_LST_FLT: 
    *(u64 *) psh_addr = *PROP_DAT(obj_addr); 
    break;

    case OBJ_LST_PTR:
    *(u32 *) 
    psh_addr = ptr - v_heap.ptr_table.base_ptr;
    break;

    default:
    break;

  }

  return status;
}

v_err 
v_list_pop( v_pointer_object *lst_ptr, 
            v_pointer_object **ptr) 
{
  if (is_null_pointer(lst_ptr)) {
    return V_ERR_OBJ_NULL;
  }


}
