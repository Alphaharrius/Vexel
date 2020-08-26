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

Ve_Err 
Ve_CreateListObject( VeObject **ob, 
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
    return ERR_LST_MAX_LEN;
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
    default: return ERR_API_INV_CALL;
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

  Ve_Err stat;
  u64 ob_size = SIZE_LST_OB + el_size * tot_len;

  if (ob_size > Ve_Heap.ob_space_size) {
    return ERR_HEAP_OUT_OF_MEM;
  }

  stat = Ve_HeapAllocate(ob, ob_size);

  if (IS_ERR(stat)) {
    return stat;
  }

  u8 *ob_addr = (*ob)->mem_addr;
  *Ve_TYPE(ob_addr) = type;
  *Ve_ELSIZE(ob_addr) = el_size;
  *Ve_LST_POS(ob_addr) = len;
  *Ve_LST_LEN(ob_addr) = tot_len;

  if (len && d_ptr) {
    memcpy(Ve_LST_BDAT(ob_addr), d_ptr, el_size * len);
  }

  return stat;
}

Ve_Err 
Ve_ListExpand(VeObject *ob,
              u8 **addr)
{
  /**
   * Check if the list pointer is the global null 
   * pointer, which takes the base position.
   */
  if (Ve_IsNull(ob)) {
    return ERR_OB_NULL;
  }

  u8 *lst_addr = ob->mem_addr;
  /**
   * Check if the pointer is type of list.
   */
  if (!Ve_ISLST(lst_addr)) {
    return ERR_OB_NOT_LST;
  }

  u8 el_size = *Ve_ELSIZE(lst_addr);

  /**
   * Using pointer for better handling.
   */
  u32 *len = Ve_LST_POS(lst_addr);
  u32 tot_len = *Ve_LST_LEN(lst_addr);

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

    Ve_Err stat = Ve_HeapReallocate(ob, new_size);
    if (IS_ERR(stat)) {
      return stat;
    }

    lst_addr = ob->mem_addr;
    /**
     * Set the length pointer to 
     * the location within the 
     * new allocation.
     */
    len = Ve_LST_POS(lst_addr);
    /**
     * Only the total length requires an update.
     */
    *Ve_LST_LEN(lst_addr) = tot_len;
  }

  /**
   * Set the return address to the last 
   * element address of the list.
   */
  *addr = Ve_LST_BDAT(lst_addr) + (*len)++ * el_size;

  return ERR_NONE;
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

Ve_Err 
Ve_ListPush(VeObject *lst, 
            VeObject *ob)
{
  /**
   * Validate the element type to be pushed to 
   * the list, this could be replaced by a pre 
   * check instruction in later builds.
   */
  u8 *lst_addr = lst->mem_addr;
  u8 *ob_addr = ob->mem_addr;
  u8 lst_type = *Ve_TYPE(lst_addr);

  /**
   * As this is a static list, elements 
   * being pushed must be of same element 
   * type as the list.
   */
  if (!match_list_type(lst_type, *Ve_TYPE(ob_addr))) {
    return ERR_OB_TYP_UNMATCH;
  }

  Ve_Err stat;
  u8 *el_addr;
  /**
   * Attempt to expand the list by 1.
   */
  stat = Ve_ListExpand(lst, &el_addr);
  if (IS_ERR(stat)) {
    return stat;
  }

  switch (lst_type) {

    /**
     * Charactor type have size of 1 byte.
     */
    case OB_LST_CHR:
    *el_addr = *Ve_BDAT(ob_addr);
    break;

    /**
     * Integer and float type have size of 8 bytes.
     */
    case OB_LST_INT:
    case OB_LST_FLT:
    *Ve_QPTR(el_addr) = *Ve_QDAT(ob_addr);
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
    *Ve_QPTR(el_addr) = (u64) ob;
    break;

    default: break;

  }

  return stat;
}

Ve_Err 
Ve_ListPop( VeObject *lst, 
            VeObject **ob) 
{
  if (Ve_IsNull(lst)) {
    return ERR_OB_NULL;
  }

  u8 *lst_addr = lst->mem_addr;

  if (!Ve_ISLST(lst_addr)) {
    return ERR_OB_NOT_LST;
  }

  if (*Ve_LST_POS(lst_addr) == 0) {
    *ob = Ve_NULLPTR;
  }
  
  /**
   * The popped element: arr[len - 1],
   * pre-decrement the array length and 
   * set it as the new length, which is 
   * also the popped index.
   */
  u32 new_len = -- *Ve_LST_POS(lst_addr);

  u8 type;
  u64 dat;
  switch (*Ve_TYPE(lst_addr)) {

    case OB_LST_CHR:
    type = OB_CHR;
    /**
     * All data object are stored in a 8 byte memory chunk, 
     * to prevent being stored as a sign extended byte, 
     * casting the dat pointer to byte pointer.
     */
    *Ve_BPTR(&dat) = *(Ve_LST_BDAT(lst_addr) + new_len);
    break;

    case OB_LST_INT:
    type = OB_INT;
    dat = *(Ve_LST_QDAT(lst_addr) + new_len);
    break;

    case OB_LST_FLT:
    type = OB_FLT;
    dat = *(Ve_LST_QDAT(lst_addr) + new_len);
    break;

    case OB_LST_PTR:
    /**
     * The 8 byte data stored within the 
     * popped element is the address of 
     * pointer, which is also the popped 
     * pointer, return directly.
     */
    *ob = Ve_PTR(*(Ve_LST_QDAT(lst_addr) + new_len));
    return ERR_NONE;

    default: break;
  }

  /**
   * For data lists, we have to 
   * create a new object to store 
   * the popped element.
   */
  return Ve_CreateDataObject(ob, type, dat);
}

Ve_Err 
Ve_ListConcatenate( VeObject **ob, 
                    VeObject *a_lst, 
                    VeObject *b_lst)
{
  if (Ve_IsNull(a_lst) || 
      Ve_IsNull(b_lst)) {

    return ERR_OB_NULL;
  }

  u8 *a_addr = a_lst->mem_addr;
  u8 *b_addr = b_lst->mem_addr;

  u8 a_type = *Ve_TYPE(a_addr);
  if (!Ve_ISLST(a_addr) || !Ve_ISLST(b_addr) || 
      /**
       * The lists to be concatenated must have 
       * the same type, as different list types 
       * have different element size.
       */
      a_type != *Ve_TYPE(b_addr)) {

    return ERR_API_INV_CALL;
  }

  Ve_Err stat;
  u32 a_len = *Ve_LST_POS(a_addr);
  u32 b_len = *Ve_LST_POS(b_addr);

  /**
   * Handles the case which both lists are empty, 
   * create a new empty list.
   */
  if (!(a_len | b_len)) {
    return Ve_CreateListObject(ob, a_type, 0, NULL);
  }

  /**
   * If A is empty, clone B as the new list.
   */
  else if (a_len == 0) {
    return Internal_HeapClone(ob, b_lst);
  }

  /**
   * If B is empty, clone A as the new list.
   */
  else if (b_len == 0) {
    return Internal_HeapClone(ob, a_lst);
  }

  /**
   * Create a new list with length of the total length of two lists, 
   * we must use Ve_CreateListObject to create the list to confine 
   * standardize the length.
   */
  stat = Ve_CreateListObject(ob, a_type, a_len + b_len, NULL);
  if (IS_ERR(stat)) {
    return stat;
  }

  u8 *ob_addr = (*ob)->mem_addr;
  u8 el_size = *Ve_ELSIZE(a_addr);
  u64 a_size = a_len * el_size;

  /**
   * Copy the data bytes from A to 
   * the data head address new list.
   */
  memcpy( Ve_LST_BDAT(ob_addr), 
          Ve_LST_BDAT(a_addr), 
          a_size);

  /**
   * Copy the data bytes from B to 
   * the data address offsetted by 
   * the size of data from A.
   */
  memcpy( Ve_LST_BDAT(ob_addr) + a_size, 
          Ve_LST_BDAT(b_addr), 
          b_len * el_size);

  return stat;
}
