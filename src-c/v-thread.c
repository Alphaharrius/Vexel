#include "Vexel.h"
/**
 * Copyright (c) 2019, 2020, Alphaharrius. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE includes.
 * 
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements. See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership. The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 * 
 *  http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * 
 * v-thread.c
 */
v_thread_object *
v_make_thread() {
  v_thread_object *thread = malloc(sizeof(v_thread_object));
  if (!thread) 
    return thread;
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
  if (!thread) 
    return;
  if (thread->frame) 
    v_free_frame(thread->frame);
  if (thread->es_base) 
    free(thread->es_base);
  free(thread);
}
