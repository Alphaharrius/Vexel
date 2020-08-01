#include "Vexel.h"

boo 
v_make_var_object(u8 type, u32 *ptr_idx, u64 data) {
  u32 ptr;
  u8 *obj_address;
  if (!v_allocate_pointer(ptr_idx, &obj_address, SIZE_VAR_OBJ)) {
    return false;
  }
  PROP_TYPE(obj_address) = type;
  PROP_DATA(obj_address) = data;
  return true;
}

boo v_make_list_object(u8 type, u32 *ptr_idx, u32 list_size);
boo v_make_map_object(u32 *ptr_idx, u32 map_length);
