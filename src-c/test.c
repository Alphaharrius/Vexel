#include "Vexel.h"

int main() {
  v_initialize_heap(0xf000, 0x2000);
  // u32 pi;
  // u8 *pa;
  // v_allocate_pointer(&pi, &pa, 0x456);
  // u32 ptr_idx;
  // if (!v_make_var_object(OBJ_TYPE_INT, &ptr_idx, 100)) {
  //   FATAL("Failed to allocate integer...");
  // }
  // u8 *obj_addr = V_PTR_ADDR(ptr_idx);
  // u8 obj_type = *PROP_TYPE(obj_addr);
  // u64 obj_data = *PROP_DATA(obj_addr);
  // printf("ptr_idx=%d\nobj_addr=%llx\nobj_type=%d\nobj_data=%d\n", ptr_idx, obj_addr, obj_type, obj_data);
  u32 list_idx;
  if (!v_make_list_object(OBJ_TYPE_LIST_STR, &list_idx, NULL, 0x5000)) {
    FATAL("Failed to allocate list...");
  }
  u8 * list_addr = V_PTR_ADDR(list_idx);
  u8 list_type = *PROP_TYPE(list_addr);
  u32 list_len = *PROP_LEN(list_addr);
  u32 list_link_ptr_idx = *PROP_LINK_PTR_IDX(list_addr);
  printf("list_idx=%d\nlist_addr=%llx\nlist_type=%d\nlist_len=%d\nlist_link_ptr_idx=%d\n", list_idx, list_addr, list_type, list_len, list_link_ptr_idx);
  return 0;
}
