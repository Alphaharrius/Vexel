#ifndef V_MALLOC_H
#define V_MALLOC_H

#include "v-type.h"

#ifdef OS_WINDOWS
#include <Windows.h>
#endif

#define V_HEAP_STORE_SIZE 0x4000

static struct _v_heap_object {
  void * pos;
  void * base;
  void * roof;
  u64 size;
  u32 store_cnt;
  u64 * stores_base;
} v_heap_object;

boo v_init_heap(u64 size);

#endif
