#include "Vexel.h"

void 
print_pointer(v_pointer_object *ptr) 
{
  printf("{\n\tmem_addr : 0x%llx, \
          \n\tsize : 0x%llx, \
          \n\tprior_idx : %d, \
          \n\tnext_idx : %d\n}",
          ptr->mem_addr,
          ptr->size, 
          ptr->prior_idx, 
          ptr->next_idx);
}

int 
main() 
{
  v_initialize_heap(0xf000, 0x2000);

  v_pointer_object *list;
  v_err status = v_make_list_object(&list, OBJ_LST_PTR, 0, NULL);
  if (status != V_ERR_NONE) {
    FATAL("failed to allocate list...");
  }

  for (u8 i = 0; i < 3; i++) {
    v_pointer_object *elem;
    status = v_make_data_object(&elem, OBJ_INT, 0x11223344);
    if (status != V_ERR_NONE) {
      FATAL("failed to allocate data...");
    }
    status = v_list_push(list, elem);
    if (status != V_ERR_NONE) {
      FATAL("failed to push data to list...");
    }
  }

  v_heap_reallocate(v_heap.ptr_table.start_ptr, 9);

  v_pointer_object *ptr = v_heap.ptr_table.start_ptr;
  while (ptr->next_idx) {
    print_pointer(ptr);
    ptr = v_heap.ptr_table.base_ptr + ptr->next_idx;
  }
  print_pointer(ptr);

  return 0;
}
