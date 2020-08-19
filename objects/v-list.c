#include "../core/Vexel.h"
/**
 * Copyright (c) 2019, 2020, Alphaharrius. All rights reserved.
 * 
 * v-list.c
 */

#define LST_LEN_GROW(tot_len) \
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
    case OB_LST_CHR:
    case OB_LST_BYT: el_size = SIZE_8; break;
    /**
     * Size of pointer, int and float type are 8.
     */
    case OB_LST_PTR:
    case OB_LST_INT:
    case OB_LST_FLT: el_size = SIZE_64; break;
    /**
     * Types out of the scope of list type 
     * will be rejected, this checking is 
     * for debug purpose only as the compiler 
     * will not generate invalid types.
     */
    default: return V_ERR_API_INV_CALL;
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
    LST_LEN_GROW(tot_len);
  }

  v_err status;
  u64 ob_size = SIZE_LST_OB + el_size * tot_len;

  if (ob_size > v_heap.ob_space_size) {
    return V_ERR_HEAP_OUT_OF_MEM;
  }

  status = v_heap_allocate(ptr, ob_size);

  if (status != V_ERR_NONE) {
    return status;
  }

  u8 *ob_addr = (*ptr)->mem_addr;
  *V_TYPE(ob_addr) = type;
  *V_EL_SIZE(ob_addr) = el_size;
  *V_LST_POS(ob_addr) = len;
  *V_LST_LEN(ob_addr) = tot_len;

  if (len && dptr) {
    memcpy(V_LST_BDAT(ob_addr), dptr, el_size * len);
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
  if (v_is_null(lst_ptr)) {
    return V_ERR_OB_NULL;
  }

  u8 *lst_addr = lst_ptr->mem_addr;
  /**
   * Check if the pointer is type of list.
   */
  if (!V_IS_LST(lst_addr)) {
    return V_ERR_OB_NOT_LST;
  }

  u8 el_size = *V_EL_SIZE(lst_addr);

  /**
   * Using pointer for better handling.
   */
  u32 *len = V_LST_POS(lst_addr);
  u32 tot_len = *V_LST_LEN(lst_addr);

  /**
   * Check if the length is smaller than 
   * the allocated total length.
   */
  if (*len >= tot_len) {
    /**
     * Allocate a new memory chunk for the 
     * list with extra allocated length.
     */
    LST_LEN_GROW(tot_len);
    u64 new_size = SIZE_LST_OB + tot_len * el_size;

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
    len = V_LST_POS(lst_addr);
    /**
     * Only the total length requires an update.
     */
    *V_LST_LEN(lst_addr) = tot_len;
  }

  /**
   * Set the return address to the last 
   * element address of the list.
   */
  *addr = V_LST_BDAT(lst_addr) + (*len)++ * el_size;

  return V_ERR_NONE;
}

static inline u8 
match_list_type(u8 lst_type, u8 type)
{
  switch (lst_type) {

    case OB_LST_INT: return type == OB_INT;
    case OB_LST_FLT: return type == OB_FLT;
    case OB_LST_BYT:
    case OB_LST_CHR: return type == OB_CHR;

    default: return FALSE;

  }
}

v_err 
v_list_push(v_pointer_object *lst_ptr, 
            v_pointer_object *ptr)
{
  /**
   * Validate the element type to be pushed to 
   * the list, this could be replaced by a pre 
   * check instruction in later builds.
   */
  u8 *lst_addr = lst_ptr->mem_addr;
  u8 *ob_addr = ptr->mem_addr;
  u8 lst_type = *V_TYPE(lst_addr);

  /**
   * As this is a static list, elements 
   * being pushed must be of same element 
   * type as the list.
   */
  if (!match_list_type(lst_type, *V_TYPE(ob_addr))) {
    return V_ERR_OB_TYP_UNMATCH;
  }

  v_err status;
  u8 *psh_addr;
  status = v_list_expand(lst_ptr, &psh_addr);

  if (status != V_ERR_NONE) {
    return status;
  }

  switch (lst_type) {

    case OB_LST_BYT:
    case OB_LST_CHR:
    *psh_addr = *V_BDAT(ob_addr);
    break;

    case OB_LST_INT:
    case OB_LST_FLT:
    *V_QPTR(psh_addr) = *V_QDAT(ob_addr);
    break;

    default: break;

  }

  return status;
}

v_err 
v_list_pop( v_pointer_object *lst, 
            v_pointer_object **ob) 
{
  if (v_is_null(lst)) {
    return V_ERR_OB_NULL;
  }

  u8 *lst_addr = lst->mem_addr;

  if (!V_IS_LST(lst_addr)) {
    return V_ERR_OB_NOT_LST;
  }

  if (*V_LST_POS(lst_addr) == 0) {
    *ob = V_NULL_PTR;
  }
  
  u32 new_len = -- *V_LST_POS(lst_addr);

  u8 type;
  u64 dat;
  switch (*V_TYPE(lst_addr)) {

    case OB_LST_BYT:
    case OB_LST_CHR:
    type = OB_CHR;
    *V_BPTR(&dat) = *(lst_addr + new_len);
    break;

    case OB_LST_INT:
    type = OB_INT;
    dat = *(V_LST_QDAT(lst_addr) + new_len);
    break;

    case OB_LST_FLT:
    type = OB_FLT;
    dat = *(V_LST_QDAT(lst_addr) + new_len);
    break;

    default: break;
  }

  return v_make_data_object(ob, type, dat);
}

v_err 
v_list_concatenate( v_pointer_object **ob, 
                    v_pointer_object *ob_a, 
                    v_pointer_object *ob_b)
{
  if (v_is_null(ob_a) || 
      v_is_null(ob_b)) {

    return V_ERR_OB_NULL;
  }

  u8 *a_addr = ob_a->mem_addr;
  u8 *b_addr = ob_b->mem_addr;

  u8 a_type = *V_TYPE(a_addr);
  if (!V_IS_LST(a_addr) || !V_IS_LST(b_addr) || 
      a_type != *V_TYPE(b_addr)) {

    return V_ERR_API_INV_CALL;
  }

  v_err status;
  u32 a_len = *V_LST_POS(a_addr);
  u32 b_len = *V_LST_POS(b_addr);

  if (*V_LST_POS(a_addr) == 0) {
    status = v_heap_allocate(ob, *V_LST_LEN(b_addr));

    if (status == V_ERR_NONE) {
      memcpy((*ob)->mem_addr, b_addr, b_len);
    }

    return status;
  }

  if (*V_LST_POS(b_addr) == 0) {
    status = v_heap_allocate(ob, *V_LST_LEN(a_addr));

    if (status == V_ERR_NONE) {
      memcpy((*ob)->mem_addr, a_addr, a_len);
    }

    return status;
  }

  status = v_make_list_object(ob, a_type, a_len + b_len, NULL);
  if (status != V_ERR_NONE) {
    return status;
  }

  u8 *ob_addr = (*ob)->mem_addr;
  u8 el_size = *V_EL_SIZE(a_addr);
  u64 a_size = a_len * el_size;

  memcpy( V_LST_BDAT(ob_addr), 
          V_LST_BDAT(a_addr), 
          a_size);

  memcpy( V_LST_BDAT(ob_addr) + a_size, 
          V_LST_BDAT(b_addr), 
          b_len * el_size);

  return status;
}

#undef LST_LEN_GROW
