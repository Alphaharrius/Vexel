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
  /**
   * The pointer table is implemented 
   * as a double linked list, each pointer 
   * will carry the index to the prior 
   * and the next pointer.
   */
  u32 prior_idx;
  u32 next_idx;

} v_pointer_object;

/**
 * Cast an address into a pointer object.
 */
#define V_PTR(addr) ((v_pointer_object *) (addr))

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
  u64 obj_space_size;
  /**
   * The pointer table for accessing 
   * all allocated chunks.
   */
  struct v_ptr_table_object {
    /**
     * The first pointer of the table, 
     * it should carry the same address 
     * of the heap allocation, but of 
     * v_pointer_object type.
     */
    v_pointer_object *base_ptr;
    /**
     * The latest avaliable pointer for 
     * a new allocation.
     */
    v_pointer_object *pos_ptr;
    /**
     * The last pointer in the pointer table.
     */
    v_pointer_object *top_ptr;
    /**
     * The pointer table will only carry 
     * reference to the start and the end 
     * pointer of the Heap Pool.
     */
    v_pointer_object *start_ptr;
    v_pointer_object *end_ptr;

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
v_err v_heap_allocate(v_pointer_object **ptr, u64 size);

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
v_err v_heap_reallocate(v_pointer_object *ptr, u64 size);

/**
 * This method free the memory indicated by the pointer, 
 * but the actual allocation will not be freed until heap 
 * rearrange happens.
 * @param ptr: The pointer associated to the allocated memory.
 * @return: The status of the operation.
 */
v_err v_heap_free(v_pointer_object *ptr);

typedef enum {

  V_HEAP_REARR_FULL

} v_heap_rearr_strat;

v_err v_heap_rearrange(v_heap_rearr_strat strat);

#endif
