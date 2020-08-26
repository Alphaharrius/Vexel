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
 * PROBLEM: The current allocation algorithm might be 
 * fast and accurate, but it will accumulates the usage 
 * of the heap until full loaded. The resulting effect 
 * is rapid call to garbage collection process which 
 * slows down the vm periodically.
 * 
 * FIX: Proposal of a new algorithm for memory allocation 
 * and collection, which consist of the sectored chunks as 
 * mentioned above, with a free-and-reserve approach in 
 * allocation of new objects.
 *  1.  Allocate new objects with the incremental-offset 
 *      approach until the top of the chunk is reached.
 *  2.  New allocations targeting a full chunk will be 
 *      redirected to other free chunks.
 *  3.  Perform a mark process on the accessed full chunk 
 *      to mark all unused pointers with allocated memory 
 *      chunks as reserved pointers.
 *  4.  Next object allocation targeting the marked chunk 
 *      will search for reserved pointers, if the allocated 
 *      memory size of the reserved pointer fits the new 
 *      size, the reserved pointer will be used for the 
 *      new allocation.
 *  5.  If step (4) failed to find any reserved pointers, 
 *      the allocation will be redirected, and the vm will 
 *      perform a full mark-sweep-compact garbage collection 
 *      on the memory chunk.
 */

static inline void *
/**
 * Allocation of private memory chunks using the 
 * host system's allocation utilities, currently 
 * supports Windows and Unix based systems.
 */
system_allocate_memory(u64 byte_size)
{
#if defined(WINDOWS)
  return VirtualAlloc((LPVOID) NULL, 
                      (SIZE_T) byte_size, 
                      MEM_RESERVE | MEM_COMMIT, 
                      PAGE_READWRITE);
#endif
}

static inline u32
system_free_memory(void *mem_addr)
{
#if defined(WINDOWS)
  return (u32) VirtualFree((LPVOID) mem_addr, 
      0, MEM_RELEASE);
#endif
}

void 
Ve_InitializeHeap(u64 heap_size, 
                  u64 table_size)
{
  /**
   * Check if the pointer table byte size is less than 
   * 50% of the heap byte size, terminate the virtua 
   * machine if false.
   */
  if (table_size << 1 > heap_size) {
    Ve_FATAL(WARN_INV_PTR_TABLE_SIZE);
  }

  u8 *heap_addr = system_allocate_memory(heap_size);
  /**
   * Terminates the process if the heap allocation failed.
   */
  if (!heap_addr) {
    Ve_FATAL(WARN_HEAP_ALLOC_FAILED);
  }

  /**
   * Offset the base address of allocable 
   * memory by the pointer table byte size.
   */
  u8 *base_addr = heap_addr + table_size;

  Ve_Heap = (VeHeap) {
    .base_addr = base_addr,
    /**
     * The allocation offset is initialized 
     * to the base address.
     */
    .pos_addr = base_addr,
    .top_addr = heap_addr + heap_size - 1,
    .tot_size = heap_size,
    .ob_space_size = heap_size - table_size
  };
  
  u32 tot_ptr_cnt = table_size / sizeof(VeObject);

  /**
   * Initialize the pointer table bytes to value zero as:
   *  0 => bytesize = 0,
   *  0 => mem_addr = 0 = ((void *) 0) = NULL
   */
  memset(heap_addr, 0x00, tot_ptr_cnt * sizeof(VeObject));

  Ve_PointerTable = (VePointerTable) {
    .base = Ve_PTR(heap_addr),
    .top = Ve_PTR(heap_addr) + tot_ptr_cnt - 1,
    /**
     * The pointer table index 0 is reserved for the null pointer 
     * of the Vexel runtime, all references to null pointer will 
     * be pointed to the null pointer index position.
     */
    .pos = Ve_PTR(heap_addr) + 1
  }
}

Ve_Err 
Ve_HeapAllocate(VeObject **ptr, u64 size) 
{
  /**
   * Allocation byte size is not allowed to be 
   * greater than the total avaliable byte size.
   */
  if (size > Ve_Heap.ob_space_size) {
    return ERR_HEAP_OUT_OF_MEM;
  }

  VeObject *p;
  /**
   * Return the next avaliable pointer if offset of 
   * the distribution pointer is yet to reach the 
   * top position.
   */
  if (Ve_PointerTable.pos <= Ve_PointerTable.top) {
    p = Ve_PointerTable.pos++;
  } 
  /**
   * Lookup the pointer table for unused pointer positions
   */
  else {
    p = Ve_PointerTable.base + 1;
    VeObject *top = Ve_PointerTable.top;
    while (p->mem_addr != NULL) {
      /**
       * Throw exception if the table does not 
       * have any unused pointer positions.
       */
      if (++p == Ve_PointerTable.top) {
        return ERR_HEAP_NO_IDX;
      }
    }
  }

  /**
   * The allocator does not care about any unused 
   * holes before the allocation offset.
   */
  if (Ve_Heap.pos_addr + size <= Ve_Heap.top_addr) {
    p->mem_addr = Ve_Heap.pos_addr;
    p->size = size;
    Ve_Heap.pos_addr += size;
    *ptr = p;

    return ERR_NONE;
  } 
  else {
    return ERR_HEAP_OUT_OF_MEM;
  }
}

Ve_Err 
Ve_HeapReallocate(VeObject *ptr, u64 size) 
{
  /**
   * Allocation byte size is not allowed to be 
   * greater than the total avaliable byte size.
   */
  if (size > Ve_Heap.ob_space_size) {
    return ERR_HEAP_OUT_OF_MEM;
  }

  /**
   * Cache the prior pointer information.
   */
  u8 *prior_addr = ptr->mem_addr;
  u64 prior_size = ptr->size;

  if (Ve_Heap.pos_addr + size <= Ve_Heap.top_addr) {
    ptr->mem_addr = Ve_Heap.pos_addr;
    ptr->size = size;
    Ve_Heap.pos_addr += size;
  } else {
    return ERR_HEAP_OUT_OF_MEM;
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

  return ERR_NONE;
}

Ve_Err 
Internal_HeapClone(VeObject **des, VeObject *src)
{
  Ve_Err stat;
  stat = Ve_HeapAllocate(des, src->size);

  if (IS_ERR(stat)) {
    return stat;
  }

  memcpy((*des)->mem_addr, src->mem_addr, src->size);

  return stat;
}

u8 Ve_IsNull(VeObject *ptr)
{
  if (ptr == Ve_NULLPTR || !ptr->mem_addr) {
    return TRUE;
  }

  return FALSE;
}
