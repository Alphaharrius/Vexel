#include "Vexel.h"

void 
print_pointer(VeObject *ptr) 
{
  printf("{\n\tmem_addr : 0x%llx, \
          \n\tsize : 0x%llx, \
          \n}\n",
          ptr->mem_addr,
          ptr->size);
}

void 
debug_print_integer_list(VeObject *ob)
{
  u32 len = *V_LST_POS(ob->mem_addr);
  u64 *pos = V_LST_QDAT(ob->mem_addr);
  printf("[");
  for (u32 i = 0; i < len; i++) {
    printf("%d", *pos++);
    if (i != len - 1) {
      printf(",");
    }
  }
  printf("]\n");
}

int 
main() 
{
  Ve_InitializeHeap(0xf000, 0x2000);

  VeObject *list;
  Ve_Err stat = v_make_list_object(&list, OB_LST_INT, 0, NULL);
  if (IS_ERR(stat)) {
    FATAL("failed to allocate list...");
  }

  for (u8 i = 0; i < 50; i++) {
    VeObject *elem;
    stat = v_make_data_object(&elem, OB_INT, i);
    if (IS_ERR(stat)) {
      FATAL("failed to allocate data...");
    }
    stat = v_list_push(list, elem);
    if (IS_ERR(stat)) {
      FATAL("failed to push data to list...");
    }
  }

  VeObject *ptr = v_heap.table.base;
  u8 i = 0; while (i < 5) {
    print_pointer(ptr + i++);
  }

  VeObject *pop;
  stat = v_list_pop(list, &pop);
  if (IS_ERR(stat)) {
    FATAL("failed to pop data from the list...");
  }

  print_pointer(pop);
  printf("popped: %d\n", *V_QDAT(pop->mem_addr));
  printf("list_allocated_length: %d\n", *V_LST_LEN(list->mem_addr));

  debug_print_integer_list(list);

  return 0;
}
