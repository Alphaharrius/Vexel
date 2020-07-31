#ifndef V_MALLOC_H
#define V_MALLOC_H
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
 * v-malloc.h
 */
#include "v-type.h"

#ifdef OS_WINDOWS
#include <Windows.h>
#endif
/**
 * Each block is 16KB in size.
 */
#define HEAP_BLOCK_SIZE 0x4000
/**
 * Basic block structure for the 
 * heap to split the allocation, 
 * which allows the usage of a 
 * more efficent collection 
 * algorithm.
 */
typedef struct {
  /**
   * Storing the base address of 
   * each block allows a faster 
   * access to block addresses 
   * without calculations.
   */
  void *base_address;
  /**
   * New allocations will always 
   * be added to the end of the 
   * current position address, 
   * while this might sound 
   * inefficient, this improved 
   * allocation speed by offloading 
   * the computations to the garbrage 
   * collection logic.
   */
  void *pos_address;
  /**
   * the roof address informs the allocator 
   * if there is still room for a incoming 
   * allocation by subtracting itself with 
   * the position address.
   */
  void *roof_address;
} v_heap_block_object;

static struct _heap_object {
  void *base_address;
  void *roof_address;
  u64 total_byte_size;
  v_heap_block_object *blocks;
  u32 block_cnt;
  struct _hyperspace_object {
    u64 *base_address;
    u64 *roof_address;
    u32 idx_pos;
    u32 idx_max;
  } hyperspace;
} v_heap;

boo v_initialize_heap(u64 total_byte_size, u64 hyperspace_byte_size);
boo v_allocate_pointer(u32 *ptr_idx, void **alloc_address, u32 byte_size);
void *v_get_pointer_address(u32 ptr_idx);
void *v_pointer_roof_address(void *ptr_address);

#endif
