#ifndef V_MALLOC_H
#define V_MALLOC_H
/**
 * Copyright (c) 2019, 2020, Alphaharrius. All rights reserved.
 */
#include "v-type.h"

#ifdef OS_WINDOWS
#include <Windows.h>
#endif
/**
 * Each block is 16KB in size.
 */
#define HEAP_BLOCK_SIZE 0x4000
/**
 * Basic block structure for the 
 * heap to split the allocation, 
 * which allows the usage of a 
 * more efficent collection 
 * algorithm.
 */
typedef struct {
  /**
   * Storing the base address of 
   * each block allows a faster 
   * access to block addresses 
   * without calculations.
   */
  u8 *base_addr;
  /**
   * New allocations will always 
   * be added to the end of the 
   * current position address, 
   * while this might sound 
   * inefficient, this improved 
   * allocation speed by offloading 
   * the computations to the garbrage 
   * collection logic.
   */
  u8 *pos_addr;
  /**
   * the roof address informs the allocator 
   * if there is still room for a incoming 
   * allocation by subtracting itself with 
   * the position address.
   */
  u8 *roof_addr;
} v_heap_block_object;

struct _heap_object {
  u8 *base_addr;
  u8 *roof_addr;
  u64 total_byte_size;
  v_heap_block_object *blocks;
  u32 block_cnt;
  struct _hyperspace_object {
    u64 *base_addr;
    u64 *roof_addr;
    u32 idx_pos;
    u32 max_idx;
  } hyperspace;
} v_heap;

/**
 * A small macro to convert the heap pointer index 
 * to the allocated memory address, this ensures 
 * the inlining of the operation.
 */
#define V_POINTER_addr(ptr_idx) \
  ptr_idx < v_heap.hyperspace.max_idx ? (u8 *) \
      *(v_heap.hyperspace.base_addr + ptr_idx) : NULL
/**
 * This method initializes the Vexel Heap for object data storage.
 * @param total_byte_size: The total byte size of the heap allocation.
 * @param hyperspace_byte_size: The byte size of the hyperspace, 
 *                              which stores the pointer index 
 *                              to address mapping table.
 * @return Boolean that indicates if the operation is successful.
 */
void v_initialize_heap(u64 total_byte_size, u64 hyperspace_byte_size);
/**
 * While the process's memory will be freed on 
 * termination, calling this function ensures 
 * a safer process exit.
 */
void v_free_heap();
/**
 * This method assigns the pointer index and address of the allocated bytes.
 * @param ptr_idx: An u32 pointer to store the assigned index.
 * @param alloc_addr:  A void pointer to be assigned to the 
 *                        pointer points to the allocated bytes.
 *                        This parameter can be NULL if the memory 
 *                        address is not useful for subsequent logic.
 * @param byte_size: The byte size to be allocated, no larger than block size.
 * @return Boolean that indicates if the operation is successful.
 */
boo v_allocate_pointer(u32 *ptr_idx, u8 **alloc_addr, u64 byte_size);

#endif
