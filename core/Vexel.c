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
  u32 len = *Ve_LST_POS(ob->mem_addr);
  u64 *pos = Ve_LST_QDAT(ob->mem_addr);
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
  Ve_Err stat = Ve_CreateListObject(&list, OB_LST_INT, 0, NULL);
  if (IS_ERR(stat)) {
    Ve_FATAL("failed to allocate list...");
  }

  for (u8 i = 0; i < 50; i++) {
    VeObject *elem;
    stat = Ve_CreateDataObject(&elem, OB_INT, i);
    if (IS_ERR(stat)) {
      Ve_FATAL("failed to allocate data...");
    }
    stat = Ve_ListPush(list, elem);
    if (IS_ERR(stat)) {
      Ve_FATAL("failed to push data to list...");
    }
  }

  VeObject *ptr = Ve_PointerTable.base;
  u8 i = 0; while (i < 5) {
    print_pointer(ptr + i++);
  }

  VeObject *pop;
  stat = Ve_ListPop(list, &pop);
  if (IS_ERR(stat)) {
    Ve_FATAL("failed to pop data from the list...");
  }

  print_pointer(pop);
  printf("popped: %d\n", *Ve_QDAT(pop->mem_addr));
  printf("list_allocated_length: %d\n", *Ve_LST_LEN(list->mem_addr));

  debug_print_integer_list(list);

  return 0;
}
