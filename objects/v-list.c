#include "../core/Vexel.h"
/**
 * Copyright (c) 2019, 2020, Alphaharrius. All rights reserved.
 * 
 * v-list.c
 * This file implements the list object, list objects represents 
 * the following types: integer list, float list, pointer list (array), 
 * byte/char list (strings).
 */

/**
 * The list size will grow by 1.5 times of 
 * the original allocated length to reduce 
 * reallocation on resizing.
 */
#define LST_LEN_GROW(tot_len) \
  tot_len = (tot_len << 1) - (tot_len >> 1);

v_err 
v_make_list_object( v_object **ob, 
                    u8 type, 
                    u32 len, 
                    u8 *d_ptr)
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
     * Size of char type is 1.
     */
    case OB_LST_CHR: el_size = SIZE_8; break;
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

  v_err stat;
  u64 ob_size = SIZE_LST_OB + el_size * tot_len;

  if (ob_size > v_heap.ob_space_size) {
    return V_ERR_HEAP_OUT_OF_MEM;
  }

  stat = v_heap_allocate(ob, ob_size);

  if (IS_ERR(stat)) {
    return stat;
  }

  u8 *ob_addr = (*ob)->mem_addr;
  *V_TYPE(ob_addr) = type;
  *V_EL_SIZE(ob_addr) = el_size;
  *V_LST_POS(ob_addr) = len;
  *V_LST_LEN(ob_addr) = tot_len;

  if (len && d_ptr) {
    memcpy(V_LST_BDAT(ob_addr), d_ptr, el_size * len);
  }

  return stat;
}

v_err 
v_list_expand(v_object *ob,
              u8 **addr)
{
  /**
   * Check if the list pointer is the global null 
   * pointer, which takes the base position.
   */
  if (v_is_null(ob)) {
    return V_ERR_OB_NULL;
  }

  u8 *lst_addr = ob->mem_addr;
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

    v_err stat = v_heap_reallocate(ob, new_size);
    if (IS_ERR(stat)) {
      return stat;
    }

    lst_addr = ob->mem_addr;
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
match_list_type(u8 ob_type, u8 type)
{
  switch (ob_type) {

    case OB_LST_INT: return type == OB_INT;
    case OB_LST_FLT: return type == OB_FLT;

    case OB_LST_CHR: return type == OB_CHR;

    case OB_LST_PTR: return TRUE;

    default: return FALSE;

  }
}

v_err 
v_list_push(v_object *lst, 
            v_object *ob)
{
  /**
   * Validate the element type to be pushed to 
   * the list, this could be replaced by a pre 
   * check instruction in later builds.
   */
  u8 *lst_addr = lst->mem_addr;
  u8 *ob_addr = ob->mem_addr;
  u8 lst_type = *V_TYPE(lst_addr);

  /**
   * As this is a static list, elements 
   * being pushed must be of same element 
   * type as the list.
   */
  if (!match_list_type(lst_type, *V_TYPE(ob_addr))) {
    return V_ERR_OB_TYP_UNMATCH;
  }

  v_err stat;
  u8 *el_addr;
  /**
   * Attempt to expand the list by 1.
   */
  stat = v_list_expand(lst, &el_addr);
  if (IS_ERR(stat)) {
    return stat;
  }

  switch (lst_type) {

    /**
     * Charactor type have size of 1 byte.
     */
    case OB_LST_CHR:
    *el_addr = *V_BDAT(ob_addr);
    break;

    /**
     * Integer and float type have size of 8 bytes.
     */
    case OB_LST_INT:
    case OB_LST_FLT:
    *V_QPTR(el_addr) = *V_QDAT(ob_addr);
    break;

    /**
     * As pointer list stores pointer, 
     * there is no need of extracting 
     * the data from the object to be 
     * pushed, set the data of the new 
     * list element to the address of 
     * the object pointer.
     */
    case OB_LST_PTR:
    *V_QPTR(el_addr) = (u64) ob;
    break;

    default: break;

  }

  return stat;
}

v_err 
v_list_pop( v_object *lst, 
            v_object **ob) 
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
  
  /**
   * The popped element: arr[len - 1],
   * pre-decrement the array length and 
   * set it as the new length, which is 
   * also the popped index.
   */
  u32 new_len = -- *V_LST_POS(lst_addr);

  u8 type;
  u64 dat;
  switch (*V_TYPE(lst_addr)) {

    case OB_LST_CHR:
    type = OB_CHR;
    /**
     * All data object are stored in a 8 byte memory chunk, 
     * to prevent being stored as a sign extended byte, 
     * casting the dat pointer to byte pointer.
     */
    *V_BPTR(&dat) = *(V_LST_BDAT(lst_addr) + new_len);
    break;

    case OB_LST_INT:
    type = OB_INT;
    dat = *(V_LST_QDAT(lst_addr) + new_len);
    break;

    case OB_LST_FLT:
    type = OB_FLT;
    dat = *(V_LST_QDAT(lst_addr) + new_len);
    break;

    case OB_LST_PTR:
    /**
     * The 8 byte data stored within the 
     * popped element is the address of 
     * pointer, which is also the popped 
     * pointer, return directly.
     */
    *ob = V_PTR(*(V_LST_QDAT(lst_addr) + new_len));
    return V_ERR_NONE;

    default: break;
  }

  /**
   * For data lists, we have to 
   * create a new object to store 
   * the popped element.
   */
  return v_make_data_object(ob, type, dat);
}

v_err 
v_list_concatenate( v_object **ob, 
                    v_object *a_lst, 
                    v_object *b_lst)
{
  if (v_is_null(a_lst) || 
      v_is_null(b_lst)) {

    return V_ERR_OB_NULL;
  }

  u8 *a_addr = a_lst->mem_addr;
  u8 *b_addr = b_lst->mem_addr;

  u8 a_type = *V_TYPE(a_addr);
  if (!V_IS_LST(a_addr) || !V_IS_LST(b_addr) || 
      /**
       * The lists to be concatenated must have 
       * the same type, as different list types 
       * have different element size.
       */
      a_type != *V_TYPE(b_addr)) {

    return V_ERR_API_INV_CALL;
  }

  v_err stat;
  u32 a_len = *V_LST_POS(a_addr);
  u32 b_len = *V_LST_POS(b_addr);

  /**
   * Handles the case which both lists are empty, 
   * create a new empty list.
   */
  if (!(a_len | b_len)) {
    return v_make_list_object(ob, a_type, 0, NULL);
  }

  /**
   * If A is empty, clone B as the new list.
   */
  else if (a_len == 0) {
    return vm_heap_clone(ob, b_lst);
  }

  /**
   * If B is empty, clone A as the new list.
   */
  else if (b_len == 0) {
    return vm_heap_clone(ob, a_lst);
  }

  /**
   * Create a new list with length of the total length of two lists, 
   * we must use v_make_list_object to create the list to confine 
   * standardize the length.
   */
  stat = v_make_list_object(ob, a_type, a_len + b_len, NULL);
  if (IS_ERR(stat)) {
    return stat;
  }

  u8 *ob_addr = (*ob)->mem_addr;
  u8 el_size = *V_EL_SIZE(a_addr);
  u64 a_size = a_len * el_size;

  /**
   * Copy the data bytes from A to 
   * the data head address new list.
   */
  memcpy( V_LST_BDAT(ob_addr), 
          V_LST_BDAT(a_addr), 
          a_size);

  /**
   * Copy the data bytes from B to 
   * the data address offsetted by 
   * the size of data from A.
   */
  memcpy( V_LST_BDAT(ob_addr) + a_size, 
          V_LST_BDAT(b_addr), 
          b_len * el_size);

  return stat;
}
