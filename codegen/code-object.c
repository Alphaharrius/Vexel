#include "code-object.h"

code_object *
make_code_object(u64 size) 
{
  code_object *code_obj = malloc(sizeof(code_object));
  if (!code_obj) {
    return NULL;
  }

  u8 *code = calloc(size, SIZE_8);
  if (!code) {
    free(code_obj);
    return NULL;
  }
  
  code_obj->code = code;
  code_obj->pos = 0;

  return code_obj;
}
