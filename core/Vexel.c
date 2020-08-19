#include "Vexel.h"

void 
print_pointer(v_pointer_object *ptr) 
{
  printf("{\n\tmem_addr : 0x%llx, \
          \n\tsize : 0x%llx, \
          \n}\n",
          ptr->mem_addr,
          ptr->size);
}

int 
main() 
{
  v_initialize_heap(0xf000, 0x2000);

  v_pointer_object *list;
  v_err status = v_make_list_object(&list, OB_LST_INT, 0, NULL);
  if (status != V_ERR_NONE) {
    FATAL("failed to allocate list...");
  }

  for (u8 i = 0; i < 3; i++) {
    v_pointer_object *elem;
    status = v_make_data_object(&elem, OB_INT, 0x11223344 + i);
    if (status != V_ERR_NONE) {
      FATAL("failed to allocate data...");
    }
    status = v_list_push(list, elem);
    if (status != V_ERR_NONE) {
      FATAL("failed to push data to list...");
    }
  }

  v_pointer_object *ptr = v_heap.ptr_table.base_ptr;
  u8 i = 0; while (i < 5) {
    print_pointer(ptr + i++);
  }

  v_pointer_object *pop;
  status = v_list_pop(list, &pop);
  if (status != V_ERR_NONE) {
    FATAL("failed to pop data from the list...");
  }

  print_pointer(pop);
  printf("popped: %llx\n", *V_QDAT(pop->mem_addr));

  return 0;
}
