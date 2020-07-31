#ifndef V_THREAD_H
#define V_THREAD_H
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
 * v-thread.h
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
