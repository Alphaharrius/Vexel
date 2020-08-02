#ifndef V_THREAD_H
#define V_THREAD_H
/**
 * Copyright (c) 2019, 2020, Alphaharrius. All rights reserved.
 */
#include "v-type.h"
#include "v-error.h"
#include "v-frame.h"

#define V_ES_LEN 64

#define V_THREAD_STAT_PAUSE   0x00
#define V_THREAD_STAT_STARTED 0x01
#define V_THREAD_STAT_SUSPEND 0x02

typedef struct {
  v_frame_object *frame;
  u8 *status;
  v_error_type *es_pos;
  v_error_type *es_base;
  v_error_type *es_roof;
} v_thread_object;

v_thread_object *v_make_thread();
void v_free_thread(v_thread_object *thread);

#endif
