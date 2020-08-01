#include "Vexel.h"

int main() {
  v_initialize_heap(0xc000, 0x2000);
  u32 ptr_idx;
  if (!v_make_var_object(OBJ_TYPE_INT, &ptr_idx, 100)) {
    FATAL("Failed to allocate integer...");
  }
  u8 *obj_address = V_POINTER_ADDRESS(ptr_idx);
  u8 obj_type = PROP_TYPE(obj_address);
  u64 obj_data = PROP_DATA(obj_address);
  printf("ptr_idx=%d\nobj_address=%llx\nobj_type=%d\nobj_data=%d\n", ptr_idx, obj_address, obj_type, obj_data);
  return 0;
}
