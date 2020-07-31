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
 * v-frame.c
 */
#include "Vexel.h"

v_frame_object *
v_make_frame(u32 ret_len) {
  /**
   * Memory safe frame allocation method, 
   * this method uses procedural allocation 
   * validation to see if each essentials 
   * are correctly allocated, failed to 
   * do will deallocate all previous 
   * allocations.
   */
  v_frame_object *frame = malloc(sizeof(v_frame_object));
  if (!frame) return frame;
  u8 *dd_pos = calloc(V_DS_LEN, SIZE_U8);
  if (!dd_pos) {
    free(frame);
    return dd_pos;
  }
  u64 *ds_pos = calloc(V_DS_LEN, SIZE_U64);
  if (!ds_pos) {
    free(frame);
    free(dd_pos);
    return ds_pos;
  }
  u64 *rs_pos = calloc(ret_len, SIZE_U64);
  if (!rs_pos) {
    free(frame);
    free(dd_pos);
    free(ds_pos);
    return rs_pos;
  }
  frame->dd_pos = frame->dd_base = dd_pos;
  frame->dd_roof = dd_pos + V_DS_LEN;
  frame->ds_pos = frame->ds_base = ds_pos;
  frame->ds_roof = dd_pos + V_DS_LEN;
  frame->rs_pos = frame->rs_base = rs_pos;
  frame->rs_len = ret_len;
  return frame;
}

void 
v_free_frame(v_frame_object *frame) {
  if (!frame) 
    return;
  if (frame->cs_base);
    free(frame->cs_base);
  if (frame->dd_base) 
    free(frame->dd_base);
  if (frame->ds_base) 
    free(frame->ds_base);
  if (frame->rs_base) 
    free(frame->rs_base);
  free(frame);
}
