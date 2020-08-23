#ifndef V_HEAP_H
#define V_HEAP_H
/**
 * Copyright (c) 2019, 2020, Alphaharrius. All rights reserved.
 * 
 * v-heap.h
 * This file defines the interface of the 
 * Vexel Heap implementation.
 */

#include "v-type.h"
#include "v-error.h"

/**
 * The basic unit of pointer index 
 * in the heap pointer table, it 
 * can be used to access the 
 * allocated heap memory.
 */
typedef struct {
  /**
   * The heap memory address 
   * pointed by the pointer 
   * object.
   */
  u8 *mem_addr;
  /**
   * The allocated byte size of 
   * the allocated heap memory.
   */
  u64 size;

} v_object;

/**
 * Cast an address into a pointer object.
 */
#define V_PTR(addr) ((v_object *) (addr))

/**
 * A macro for retrieving the global null pointer.
 */
#define V_NULL_PTR V_PTR(v_heap.base_addr)

struct v_heap_object {
  /**
   * The starting address of the 
   * heap object space.
   */
  u8 *base_addr;
  /**
   * The allocation offset of the 
   * heap object space.
   */
  u8 *pos_addr;
  /**
   * The address of the last byte 
   * of the heap allocation.
   */
  u8 *top_addr;
  /**
   * The total byte size of 
   * the heap allocation.
   */
  u64 tot_size;
  /**
   * The total avaliable byte size 
   * for object allocation.
   */
  u64 ob_space_size;
  /**
   * The pointer table for accessing 
   * all allocated chunks.
   */
  struct v_ptr_table_object {
    /**
     * The first pointer of the table, 
     * it should carry the same address 
     * of the heap allocation, but of 
     * v_object type.
     */
    v_object *base;
    /**
     * The latest avaliable pointer for 
     * a new allocation.
     */
    v_object *pos;
    /**
     * The last pointer in the pointer table.
     */
    v_object *top;

  } ptr_table;
  
} v_heap;

/**
 * This method initializes the Vexel Heap for object data storage.
 * @param heap_size: The total byte size of the heap allocation.
 * @param ptr_table_size: The byte size of the hyperspace, 
 *                        which stores the pointer index 
 *                        to address mapping table.
 */
void v_initialize_heap(u64 heap_size, u64 ptr_table_size);

/**
 * This method assigns the pointer to the address of the allocated bytes.
 * @param ptr: The address of the pointer associated to the memory allocation.
 * @param byte_size: The byte size to be allocated, no larger than block size.
 * @return: The status of the operation.
 */
v_err v_heap_allocate(v_object **ptr, u64 size);

/**
 * This method reallocates the memory indicated by the pointer 
 * to a new position with a new byte size. 
 * Please be noted that this method will not free the existing allocation, 
 * the allocation is supposed to be freed by the heap compact logic from 
 * the garbage collection process.
 * @param ptr: The pointer associated to the allocated memory.
 * @param byte_size: The byte size to be allocated, no larger than block size.
 * @return: The status of the operation.
 */
v_err v_heap_reallocate(v_object *ptr, u64 size);

/**
 * This method clones the object indicated by the 
 * pointer into a new allocated pointer.
 * This is an internal method, no object validation 
 * is provided, please use with caution.
 * @param des: The destination pointer address.
 * @param src: The source pointer.
 * @return: The status of the operation.
 */
v_err vm_heap_clone(v_object **des, v_object *src);

/**
 * This method free the memory indicated by the pointer, 
 * but the actual allocation will not be freed until heap 
 * rearrange happens.
 * @param ptr: The pointer associated to the allocated memory.
 * @return: The status of the operation.
 */
v_err v_heap_free(v_object *ptr);

/**
 * This method is a safe check if a pointer is null. 
 * There are two cases which this is valid, first is 
 * if the pointer points to the global null reference 
 * (The base pointer); or the pointer's address is NULL, 
 * which is highly unlikely but probable via external 
 * native plugins or within development builds.
 * @param: The pointer to be checked.
 * @return: Boolean value of the checking.
 */
u8 v_is_null(v_object *ptr);

#endif
