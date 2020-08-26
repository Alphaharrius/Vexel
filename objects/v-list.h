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

#define Ve_ELSIZE(addr) Ve_BPTR(addr + OFF_EL_SIZE)
#define Ve_LST_POS(addr) Ve_DPTR(addr + OFF_LST_POS)
#define Ve_LST_LEN(addr) Ve_DPTR(addr + OFF_LST_LEN)
#define Ve_LST_BDAT(addr) Ve_BPTR(addr + OFF_LST_DAT)
#define Ve_LST_WDAT(addr) Ve_WPTR(addr + OFF_LST_DAT)
#define Ve_LST_DDAT(addr) Ve_DPTR(addr + OFF_LST_DAT)
#define Ve_LST_QDAT(addr) Ve_QPTR(addr + OFF_LST_DAT)

#define SIZE_LST_OB  OFF_LST_DAT

/**
 * Create and write to an list object.
 * @param ptr: The object object reference used for the allocation.
 * @param type: The type of the list object, any other types beyond 
 *              the list domain will result in invalid param exception.
 * @param len: The length of the list to be allocated.
 * @return: Status of the operation.
 */
Ve_Err 
Ve_CreateListObject(VeObject **ob, 
                    u8 type, 
                    u32 len, 
                    u8 *d_ptr);

/**
 * Expand the list length by 1, if the maximum 
 * allocated length is reached, a new list with 
 * extra space will be allocated.
 * Not to be mistaken with Ve_ListPush, the 
 * latter is implemented using this method.
 * @param ob: The list object to be expanded.
 * @param addr: The address of the expanded element.
 * @return: Status of the operation.
 */
Ve_Err 
Ve_ListExpand(VeObject *ob, u8 **addr);

/**
 * Push a new element to a list of any type.
 * @param lst: The target list object.
 * @param ob: The element object to be pushed.
 * @return: Status of the operation.
 */
Ve_Err 
Ve_ListPush(VeObject *lst, VeObject *ob);

/**
 * Pop an element from a list of any type, create 
 * a new object to store the popped element.
 * @param lst: The target list object.
 * @param ob: The element object being popped.
 * @return: Status of the operation.
 */
Ve_Err 
Ve_ListPop(VeObject *lst, VeObject **ob);

/**
 * Concatenate two list of same type into a new list.
 * @param ob: The object address of the new list.
 * @param a_lst: The object of the head list.
 * @param b_lst: The object of the tail list.
 * @return: Status of the operation.
 */
Ve_Err 
Ve_ListConcatenate(VeObject **ob, VeObject *a_lst, VeObject *b_lst);

#endif
