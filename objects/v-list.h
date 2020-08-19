#ifndef V_LIST_H
#define V_LIST_H
#pragma once
/**
 * Copyright (c) 2019, 2020, Alphaharrius. All rights reserved.
 * 
 * v-list.h
 */

#include "../core/v-type.h"
#include "../core/v-error.h"
#include "../core/v-heap.h"

/**
 * TEMPORARY:
 * This will be a variable in the virtual machine.
 */
#define LST_MAX_LEN (u64) 0x100000000

#define OFF_EL_SIZE SIZE_8
#define OFF_LST_POS OFF_EL_SIZE + SIZE_8
#define OFF_LST_LEN OFF_LST_POS + SIZE_32
#define OFF_LST_DAT OFF_LST_LEN + SIZE_32

#define V_EL_SIZE(addr) V_BPTR(addr + OFF_EL_SIZE)
#define V_LST_POS(addr) V_DPTR(addr + OFF_LST_POS)
#define V_LST_LEN(addr) V_DPTR(addr + OFF_LST_LEN)
#define V_LST_BDAT(addr) V_BPTR(addr + OFF_LST_DAT)
#define V_LST_WDAT(addr) V_WPTR(addr + OFF_LST_DAT)
#define V_LST_DDAT(addr) V_DPTR(addr + OFF_LST_DAT)
#define V_LST_QDAT(addr) V_QPTR(addr + OFF_LST_DAT)

#define SIZE_LST_OB  OFF_LST_DAT

/**
 * Create and write to an list object.
 * @param ptr: The pointer object reference used for the allocation.
 * @param type: The type of the list object, any other types beyond 
 *              the list domain will result in invalid param exception.
 * @param len: The length of the list to be allocated.
 * @return: Status of the operation.
 */
v_err 
v_make_list_object( v_pointer_object **ptr, 
                    u8 type, 
                    u32 len, 
                    u8 *dptr);

/**
 * Expand the list length by 1, if the maximum 
 * allocated length is reached, a new list with 
 * extra space will be allocated.
 * Not to be mistaken with v_list_push, the 
 * latter is implemented using this method.
 * @param lst_ptr: The list pointer to be expanded.
 * @param addr: The address of the expanded element.
 * @return: Status of the operation.
 */
v_err 
v_list_expand(v_pointer_object *lst_ptr, u8 **addr);

/**
 * Push a new element to a list (static typed array), 
 * which includes byte (string), integer and float array.
 * @param lst_ptr: The target list pointer.
 * @param ptr: The element pointer to be pushed.
 * @return: Status of the operation.
 */
v_err 
v_list_push(v_pointer_object *lst_ptr, v_pointer_object *ptr);

/**
 * Pop an element from a list (static typed array), 
 * which includes byte (string), integer and float array. 
 * Create a new object to store the popped element.
 * @param lst_ptr: The target list pointer.
 * @return: Status of the operation.
 */
v_err 
v_list_pop(v_pointer_object *lst_ptr, v_pointer_object **ptr);

v_err 
v_list_concatenate(v_pointer_object **ob, v_pointer_object *ob_a, v_pointer_object *ob_b);

v_err 
v_list_unshift(v_pointer_object *lst_ptr, v_pointer_object *ptr);

v_err 
v_list_shift(v_pointer_object *lst_ptr, v_pointer_object **ptr);

#endif
