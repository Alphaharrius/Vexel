#include "Vexel.h"
/**
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

  v_heap.obj_space_size = heap_size - ptr_table_size;
  
  struct v_ptr_table_object *ptr_table = &v_heap.ptr_table;
  ptr_table->base_ptr = V_PTR(heap_addr);
  u32 tot_ptr_cnt = ptr_table_size / sizeof(v_pointer_object);
  ptr_table->top_ptr = ptr_table->base_ptr + tot_ptr_cnt - 1;

  /**
   * As there are no allocation just after 
   * the initialization, init both start 
   * and end pointer as NULL.
   */
  ptr_table->start_ptr = NULL;
  ptr_table->end_ptr = NULL;

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
  if (size > v_heap.obj_space_size) {
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
   * If the end pointer of the table 
   * linked list is NULL, this must 
   * be the first allocation of the 
   * heap pool.
   */
  if (!ptr_table->end_ptr) {
    ptr_table->start_ptr = p;
    ptr_table->end_ptr = p;
  } 
  /**
   * Else link the current pointer idx 
   * to the end of the linked list.
   */
  else {
    ptr_table->end_ptr->next_idx = 
        p - ptr_table->base_ptr;
    /**
     * Set the current pointer to 
     * the end of the linked list.
     */
    ptr_table->end_ptr = p;
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
  if (size > v_heap.obj_space_size) {
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

v_err 
v_heap_free(v_pointer_object *ptr) 
{
  /**
   * Freeing null pointer is not allowed.
   */
  if (!ptr->mem_addr) {
    return V_ERR_OBJ_NULL;
  }

  struct v_ptr_table_object *ptr_table = &v_heap.ptr_table;
  /**
   * The freeing of pointer is just 
   * removing it from the linked list.
   * If the prior and the end index are 
   * both zero, this is the only pointer 
   * in the linked list.
   */
  v_pointer_object *link_ptr;
  if (ptr->prior_idx & ptr->next_idx == 0) {
    ptr_table->start_ptr = NULL;
    ptr_table->end_ptr = NULL;
  }
  /**
   * If the prior index is 0, this 
   * pointer must be the start of 
   * the linked list.
   */
  else if (ptr->prior_idx == 0) {
    link_ptr = ptr_table->base_ptr + ptr->next_idx;
    link_ptr->prior_idx = 0;
    ptr_table->start_ptr = link_ptr;
  } 
  /**
   * If the next index is zero, 
   * this must be the end of the 
   * linked list.
   */
  else if (ptr->next_idx == 0) {
    link_ptr = ptr_table->base_ptr + ptr->prior_idx;
    link_ptr->next_idx = 0;
    ptr_table->end_ptr = link_ptr;
  }
  /**
   * Else the pointer must be elements 
   * within the linked list.
   */
  else {
    u32 prior_idx = ptr->prior_idx;
    u32 next_idx = next_idx;
    link_ptr = ptr_table->base_ptr + prior_idx;
    link_ptr->next_idx = next_idx;
    link_ptr = ptr_table->base_ptr + next_idx;
    link_ptr->prior_idx = next_idx;
  }

  return V_ERR_NONE;
}

v_err 
v_heap_rearrange(v_heap_rearr_strat strat) 
{
  struct v_ptr_table_object *ptr_table = &v_heap.ptr_table;
  v_pointer_object *ptr = ptr_table->start_ptr;

  if (ptr) {
    u8 *pos_addr = v_heap.base_addr;
    while (ptr != ptr_table->base_ptr) {
      if (ptr->mem_addr != pos_addr) {
        memcpy(pos_addr, ptr->mem_addr, ptr->size);
        ptr->mem_addr = pos_addr;
        pos_addr += ptr->size;
      }
      ptr = ptr_table->base_ptr + ptr->next_idx;
    }
    v_heap.pos_addr = pos_addr;
  }

  return V_ERR_NONE;
}
