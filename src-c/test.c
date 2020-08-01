#include "Vexel.h"

int main() {
  v_initialize_heap(0xc000, 0x2000);
  u32 index;
  u8 *address;
  v_allocate_pointer(&index, &address, 0x3999);
  u32 index1;
  u8 *address1;
  v_allocate_pointer(&index1, &address1, 0x2000);
  printf("%llx, %llx\n", index, address);
  printf("%llx, %llx\n", index1, address1);
  return 0;
}
