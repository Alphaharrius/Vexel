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
 * v-error.c
 */
void 
v_throw_error(  v_thread_object *thread, 
                v_frame_object *frame, 
                u64 opcode_line, 
                u16 error_type) {
  if (thread->es_pos == thread->es_roof) {
    thread->status = V_THREAD_STAT_SUSPEND;
    // TODO: Print all stack traces to the console
  } else {
    v_error_type *error = malloc(sizeof(v_error_type));
    error->error_thread = thread;
    error->error_frame = frame;
    error->opcode_line = opcode_line;
    error->error_type = error_type;
    *thread->es_pos ++ = *error;
  }
}
