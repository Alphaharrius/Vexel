#ifndef V_LIST_H
#define V_LIST_H
#pragma once
/**
 * Copyright (c) 2019, 2020, Alphaharrius. All rights reserved.
 * 
 * v-list.h
 */

#include "v-type.h"
#include "v-error.h"
#include "v-heap.h"

/**
 * TEMPORARY:
 * This will be a variable in the virtual machine.
 */
#define LST_MAX_LEN (u64) 0x100000000

#define OFF_ELSIZE SIZE_8
#define OFF_LSTPOS OFF_ELSIZE + SIZE_8
#define OFF_LSTLEN OFF_LSTPOS + SIZE_32
#define OFF_LSTDAT OFF_LSTLEN + SIZE_32

#define V_ELSIZE(addr) (u8 *) (addr + OFF_ELSIZE)
#define V_LSTPOS(addr) (u32 *) (addr + OFF_LSTPOS)
#define V_LSTLEN(addr) (u32 *) (addr + OFF_LSTLEN)
#define V_LSTDAT(addr) (u8 *) (addr + OFF_LSTDAT)

#define SIZE_LST_OBJ  OFF_LSTDAT

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
 * Push a new element to the list, the extra case 
 * of pointer lists (arrays) is incorporated into 
 * this method.
 * @param lst_ptr: The list pointer to be expanded.
 * @param ptr: The element pointer to be pushed.
 */
v_err 
v_list_push(v_pointer_object *lst_ptr, v_pointer_object *ptr);

v_err 
v_list_pop(v_pointer_object *lst_ptr, v_pointer_object **ptr);

v_err 
v_list_concatenate(v_pointer_object *lst_ptr_a, v_pointer_object *lst_ptr_b);

v_err 
v_list_unshift(v_pointer_object *lst_ptr, v_pointer_object *ptr);

v_err 
v_list_shift(v_pointer_object *lst_ptr, v_pointer_object **ptr);

#endif
