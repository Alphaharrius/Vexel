#include "Vexel.h"
/**
 * Copyright (c) 2019, 2020, Alphaharrius. All rights reserved.
 * 
 * v-heap.c
 * 
 * INFO: Please inline all logic into the methods 
 * within this file, as this file aims for the 
 * least overhead to ensure heap performance.
 * 
 * PROBLEM: Currently the Vexel Heap is just one huge 
 * pool body, which for each rearrangement, all I/O 
 * on the heap will be stalled and wait until the 
 * operation is complete.
 * 
 * FIX: Fragment the heap pool into seveal dozen of 
 * blocks (memory chunk for small object allocation), 
 * and one pool (a large memory chunk for other allocations).
 * In such way the rearrange operation can be performed 
 * on specific chunks but not on the whole heap space.
 * 
 * PROBLEM: Currently the Vexel Heap is not thread safe, 
 * where problem might arise if multiple thread is trying 
 * to perform heap operations.
 * 
 * FIX: The use of mutexs can use to lock specific 
 * operations for a single thread.
 * 
 * PROBLEM: The linked list algorithm to sort out 
 * the heap order might induce significant overhead 
 * in normal allocation and reallocations.
 * The linked list will also induces large memory 
 * overhead within the heap object space.
 * 
 * FIX: Remove the linked list implementation and 
 * use dual pivot quick sort by the address to 
 * sort the heap ordering.
 * 
 */

static inline void *
/**
 * Allocation of private memory chunks using the 
 * host system's allocation utilities, currently 
 * supports Windows and Unix based systems.
 */
_system_allocate_memory(u64 byte_size)
{
#ifdef WINDOWS
  return VirtualAlloc((LPVOID) NULL, 
                      (SIZE_T) byte_size, 
                      MEM_RESERVE | MEM_COMMIT, 
                      PAGE_READWRITE);
#elif UNIX
#endif
}

static inline u32
_system_free_memory(void *mem_addr)
{
#ifdef WINDOWS
  return (u32) VirtualFree((LPVOID) mem_addr, 
      0, MEM_RELEASE);
#elif UNIX
#endif
}

void 
v_initialize_heap(u64 heap_size, 
                  u64 ptr_table_size)
{
  /**
   * Check if the pointer table byte size is less than 
   * 50% of the heap byte size, terminate the virtua 
   * machine if false.
   */
  if (ptr_table_size << 1 > heap_size) {
    FATAL("the size of pointer table must be \
        smaller than 50% of the heap byte size");
  }

  u8 *heap_addr = _system_allocate_memory(heap_size);
  /**
   * Terminates the process if the heap allocation failed.
   */
  if (!heap_addr) {
    FATAL("unable to allocate heap memory");
  }

  /**
   * Offset the base address of allocable 
   * memory by the pointer table byte size.
   */
  v_heap.base_addr = heap_addr + ptr_table_size;
  /**
   * The allocation offset is initialized 
   * to the base address.
   */
  v_heap.pos_addr = v_heap.base_addr;

  v_heap.top_addr = heap_addr + heap_size - 1;

  v_heap.tot_size = heap_size;

  v_heap.ob_space_size = heap_size - ptr_table_size;
  
  struct v_ptr_table_object *ptr_table = &v_heap.ptr_table;
  ptr_table->base_ptr = V_PTR(heap_addr);
  u32 tot_ptr_cnt = ptr_table_size / sizeof(v_pointer_object);
  ptr_table->top_ptr = ptr_table->base_ptr + tot_ptr_cnt - 1;

  /**
   * Initialize the pointer table bytes to value zero as:
   *  0 => bytesize = 0,
   *  0 => mem_addr = 0 = ((void *) 0) = NULL
   */
  memset(heap_addr, 0x00, tot_ptr_cnt * sizeof(v_pointer_object));

  /**
   * The pointer table index 0 is reserved for the null pointer 
   * of the Vexel runtime, all references to null pointer will 
   * be pointed to the null pointer index position.
   */
  ptr_table->pos_ptr = ptr_table->base_ptr + 1;
}

v_err 
v_heap_allocate(v_pointer_object **ptr, u64 size) 
{
  /**
   * Allocation byte size is not allowed to be 
   * greater than the total avaliable byte size.
   */
  if (size > v_heap.ob_space_size) {
    return V_ERR_HEAP_OUT_OF_MEM;
  }

  v_pointer_object *p;
  struct v_ptr_table_object *ptr_table = &v_heap.ptr_table;
  /**
   * Return the next avaliable pointer if offset of 
   * the distribution pointer is yet to reach the 
   * top position.
   */
  if (ptr_table->pos_ptr <= ptr_table->top_ptr) {
    p = ptr_table->pos_ptr++;
  } 
  /**
   * Lookup the pointer table for unused pointer positions
   */
  else {
    p = ptr_table->base_ptr + 1;
    v_pointer_object *top_ptr = ptr_table->top_ptr;
    while (p->mem_addr != NULL) {
      /**
       * Throw exception if the table does not 
       * have any unused pointer positions.
       */
      if (++p == ptr_table->top_ptr) {
        return V_ERR_HEAP_NO_IDX;
      }
    }
  }

  /**
   * The allocator does not care about any unused 
   * holes before the allocation offset.
   */
  if (v_heap.pos_addr + size <= v_heap.top_addr) {
    p->mem_addr = v_heap.pos_addr;
    p->size = size;
    v_heap.pos_addr += size;
    *ptr = p;

    return V_ERR_NONE;
  } 
  else {
    return V_ERR_HEAP_OUT_OF_MEM;
  }
}

v_err 
v_heap_reallocate(v_pointer_object *ptr, u64 size) 
{
  /**
   * Allocation byte size is not allowed to be 
   * greater than the total avaliable byte size.
   */
  if (size > v_heap.ob_space_size) {
    return V_ERR_HEAP_OUT_OF_MEM;
  }

  /**
   * Cache the prior pointer information.
   */
  u8 *prior_addr = ptr->mem_addr;
  u64 prior_size = ptr->size;

  if (v_heap.pos_addr + size <= v_heap.top_addr) {
    ptr->mem_addr = v_heap.pos_addr;
    ptr->size = size;
    v_heap.pos_addr += size;
  } else {
    return V_ERR_HEAP_OUT_OF_MEM;
  }

  if (prior_size <= size) {
    memcpy(ptr->mem_addr, prior_addr, prior_size);
  } else {
    /**
     * The data will be trimmed to the 
     * new allocation size.
     */
    memcpy(ptr->mem_addr, prior_addr, size);
  }

  return V_ERR_NONE;
}

u8 v_is_null(v_pointer_object *ptr)
{
  if (ptr == V_PTR(v_heap.base_addr) || !ptr->mem_addr) {
    return TRUE;
  }

  return FALSE;
}
