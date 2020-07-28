#include "Vexel.h"

#ifdef OS_WINDOWS
static inline void * 
_virtual_alloc(void * lp_address, u64 dw_size) {
  return VirtualAlloc((LPVOID) lp_address, 
                      (SIZE_T) dw_size, 
                      MEM_RESERVE | MEM_COMMIT, 
                      PAGE_READWRITE);
}
static inline u32
_virtual_free(void * lp_address) {
  return (u32) VirtualFree((LPVOID) lp_address, 0, MEM_FREE);
}
#endif

boo v_init_heap(u64 size) {
  void * heap_allocation = _virtual_alloc(NULL, size);
  if (! heap_allocation)
    return FALSE;
  // Store count will always be rounded down.
  u32 store_cnt = (u32) (size / V_HEAP_STORE_SIZE);
  u64 * stores_base = calloc(store_cnt, SIZE_U64);
  if (! stores_base) {
    _virtual_free(heap_allocation);
    return FALSE;
  }
  v_heap_object.pos = heap_allocation;
  v_heap_object.base = heap_allocation;
  v_heap_object.roof = (void *) ((u64) heap_allocation + size);
  v_heap_object.size = size;
  v_heap_object.store_cnt = store_cnt;
  for (u32 cnt = 0; cnt < store_cnt; cnt ++) 
    * (stores_base + cnt) = (u64) heap_allocation + cnt * V_HEAP_STORE_SIZE;
  v_heap_object.stores_base = stores_base;
}
