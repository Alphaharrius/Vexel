#include "Vexel.h"
/**
 * Copyright (c) 2019, 2020, Alphaharrius. All rights reserved.
 */
v_thread_object *
v_make_thread() 
{
  v_thread_object *thread = malloc(sizeof(v_thread_object));
  if (!thread) {
    return thread;
  }
  
  v_frame_object *frame  = v_make_frame();
  if (!frame) {
    free(thread);
    return frame;
  }

  v_error_type *es_pos = calloc(V_ES_LEN, sizeof(v_error_type));
  if(!es_pos) {
    free(thread);
    free(frame);
    return es_pos;
  }

  thread->frame = frame;
  thread->es_pos = thread->es_base = es_pos;
  thread->es_roof = es_pos + V_ES_LEN;

  return thread;
}

void 
v_free_thread(v_thread_object *thread) {
  if (!thread) {
    return;
  }
  if (thread->frame) { 
    v_free_frame(thread->frame);
  }
  if (thread->es_base) {
    free(thread->es_base);
  }
  free(thread);
}
