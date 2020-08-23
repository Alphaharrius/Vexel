#include "Vexel.h"

void 
print_pointer(v_object *ptr) 
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

  v_object *list;
  v_err stat = v_make_list_object(&list, OB_LST_INT, 0, NULL);
  if (IS_ERR(stat)) {
    FATAL("failed to allocate list...");
  }

  for (u8 i = 0; i < 3; i++) {
    v_object *elem;
    stat = v_make_data_object(&elem, OB_INT, 0x11223344 + i);
    if (IS_ERR(stat)) {
      FATAL("failed to allocate data...");
    }
    stat = v_list_push(list, elem);
    if (IS_ERR(stat)) {
      FATAL("failed to push data to list...");
    }
  }

  v_object *ptr = v_heap.table.base;
  u8 i = 0; while (i < 5) {
    print_pointer(ptr + i++);
  }

  v_object *pop;
  stat = v_list_pop(list, &pop);
  if (IS_ERR(stat)) {
    FATAL("failed to pop data from the list...");
  }

  print_pointer(pop);
  printf("popped: %llx\n", *V_QDAT(pop->mem_addr));

  return 0;
}
