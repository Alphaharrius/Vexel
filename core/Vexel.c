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

void 
debug_print_integer_list(v_object *ob)
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
  v_initialize_crypto();
  // v_initialize_heap(0xf000, 0x2000);

  // v_object *list;
  // v_err stat = v_make_list_object(&list, OB_LST_INT, 0, NULL);
  // if (IS_ERR(stat)) {
  //   FATAL("failed to allocate list...");
  // }

  // for (u8 i = 0; i < 50; i++) {
  //   v_object *elem;
  //   stat = v_make_data_object(&elem, OB_INT, i);
  //   if (IS_ERR(stat)) {
  //     FATAL("failed to allocate data...");
  //   }
  //   stat = v_list_push(list, elem);
  //   if (IS_ERR(stat)) {
  //     FATAL("failed to push data to list...");
  //   }
  // }

  // v_object *ptr = v_heap.table.base;
  // u8 i = 0; while (i < 5) {
  //   print_pointer(ptr + i++);
  // }

  // v_object *pop;
  // stat = v_list_pop(list, &pop);
  // if (IS_ERR(stat)) {
  //   FATAL("failed to pop data from the list...");
  // }

  // print_pointer(pop);
  // printf("popped: %d\n", *V_QDAT(pop->mem_addr));
  // printf("list_allocated_length: %d\n", *V_LST_LEN(list->mem_addr));

  // debug_print_integer_list(list);

  printf("%llx\n", vm_crypto_hash_bytes("abcd", 4));
  printf("%llx\n", vm_crypto_hash_bytes("bbcd", 4));
  printf("%llx\n", vm_crypto_hash_bytes("cbcd", 4));
  printf("%llx\n", vm_crypto_hash_bytes("aacd", 4));
  printf("%llx\n", vm_crypto_hash_bytes("aace", 4));
  printf("%llx\n", vm_crypto_hash_bytes("aacf", 4));
  printf("%llx\n", vm_crypto_hash_bytes("a", 1));
  printf("%llx\n", vm_crypto_hash_bytes("b", 1));
  printf("%llx\n", vm_crypto_hash_bytes("c", 1));
  printf("%llx\n", vm_crypto_hash_bytes("d", 1));
  printf("%llx\n", vm_crypto_hash_bytes("e", 1));

  return 0;
}
