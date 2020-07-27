#ifndef V_FRAME_H
#define V_FRAME_H
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
 * v-frame.h
 */
#include "v-type.h"
/**
 * Limits the call stack to 256 Kbytes
 * to prevent too much memory allocated
 * during runtime.
 */
#define V_CS_MAX_LEN 0x40000
/**
 * Limits the data stack to 1 Mbytes, 
 * any size over this limit may cause 
 * huge memory allocated during runtime 
 * as each function calls will create a 
 * new frame.
 */
#define V_DS_LEN     0x20000
/**
 * This is for the convenience of the 
 * garbage collection to detect pointers 
 * as root objects in the current frame.
 */
#define V_DD_DAT  0x01
#define V_DD_PTR  0x02
/**
 * This structure is used as an execution 
 * unit of a thread, or a execution host 
 * of a method object. This frame design 
 * allows multiple return values upon 
 * completion.
 */
typedef struct {
  /**
   * "pos" stores the current position 
   * of the call stack pointer.
   * 
   * "base" stores the base position 
   * of the call stack, error 
   * will be thrown when pointer
   * moved beyond this position.
   * 
   * "roof" stores the max position of the 
   * call stack, error will be thrown 
   * when pointer move beyond this
   * position.
   * 
   * "len" stores the length of the list 
   * of returned value from the execution.
   */
  u8 * cs_pos;
  u8 * cs_base;
  u8 * cs_roof;
  u8 * dd_pos;
  u8 * dd_base;
  u8 * dd_roof;
  u64 * ds_pos;
  u64 * ds_base;
  u64 * ds_roof;
  u64 * rs_pos;
  u64 * rs_base;
  u64 * rs_len;
} v_frame_object;

v_frame_object * v_make_frame();
void v_free_frame(v_frame_object * frame);
v_error_type v_load_opcode(v_frame_object * frame, u8 * dat_arr, u64 dat_len);

#endif
