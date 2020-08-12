#include "Vexel.h"

int main() {
  v_initialize_heap(0xf000, 0x2000);
  v_pointer_object *ptr;
  v_err stat = v_heap_allocate(&ptr, 0x1000);
  if (stat != V_ERR_NONE) {
    FATAL("Unable to allocate");
  }
  v_pointer_object *ptr1;
  stat = v_heap_allocate(&ptr1, 0x1200);
  if (stat != V_ERR_NONE) {
    FATAL("Unable to allocate");
  }
  stat = v_heap_reallocate(ptr, 0x1000);
  if (stat != V_ERR_NONE) {
    FATAL("Unable to reallocate");
  }
  printf("mem_addr=%llx\nsize=%llx\nnext=%d\n", ptr->mem_addr, ptr->size, ptr->next_idx);
  printf("mem_addr=%llx\nsize=%llx\nnext=%d\n", ptr1->mem_addr, ptr1->size, ptr1->next_idx);
  return 0;
}
