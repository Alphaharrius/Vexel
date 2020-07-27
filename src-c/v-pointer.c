#include "Vexel.h"

v_pointer_object * v_make_data_pointer(u8 type, u8 size) {
  v_pointer_object * pointer = malloc(V_DATA_PTR_SIZE);
  if (! pointer) 
    return pointer;
  V_PTR_PROP_TYPE(pointer) = type;
  V_PTR_PROP_SIZE(pointer) = size;
  return pointer;
}

v_pointer_object * v_make_buff_pointer(u8 type, u8 size, u64 len) {
  v_pointer_object * pointer = malloc(V_BUFF_PTR_SIZE);
  V_PTR_PROP_TYPE(pointer) = type;
  V_PTR_PROP_SIZE(pointer) = size;
  V_PTR_PROP_LEN(pointer) = len;
  return pointer;
}

v_pointer_object * v_make_map_pointer() {
  v_pointer_object * pointer = malloc(V_MAP_PTR_SIZE);
}
