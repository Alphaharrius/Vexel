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
 * v-heap.c
 */
#ifdef OS_WINDOWS
static inline void *
_virtual_alloc(u64 dw_size) {
  return VirtualAlloc((LPVOID) NULL, 
                      (SIZE_T) dw_size, 
                      MEM_RESERVE | MEM_COMMIT, 
                      PAGE_READWRITE);
}
static inline u32
_virtual_free(void *lp_address) {
  return (u32) VirtualFree((LPVOID) lp_address, 0, MEM_RELEASE);
}
#endif

/**
 * The void * return type of this function is temporary, 
 * as we do not know if the memset function will fail, 
 * which it will not on normal cases.
 */
static inline void *
_initialize_hyperspace( void *heap_alloc_ptr, 
                        u64 hyperspace_byte_size) {
  struct _hyperspace_object *hyperspace = &v_heap.hyperspace;
  hyperspace->idx_pos = 0;
  hyperspace->max_idx = hyperspace_byte_size / sizeof(void *);
  hyperspace->base_address = heap_alloc_ptr;
  hyperspace->roof_address = (void *) 
      ((u64) heap_alloc_ptr + hyperspace_byte_size - 1);
  return memset(heap_alloc_ptr, 0, hyperspace_byte_size);
}

static inline boo 
_initialize_heap_blocks(void *heap_alloc_ptr, 
                        u64 total_byte_size, 
                        u64 hyperspace_byte_size) {
  /**
   * The heap realm is the region to store the 
   * all of the objects of the current runtime, 
   * it stack behind the heap hyper space.
   */
  u64 realm_byte_size = total_byte_size - hyperspace_byte_size;
  /**
   * The heap realm base address is shifted by 
   * the hyper space byte size.
   */
  void *realm_base_address = (void *) 
      ((u64) heap_alloc_ptr + hyperspace_byte_size);
  /**
   * The heap block size will always be rounded 
   * down to the avaliable full blocks able to 
   * fit inside the realm space.
   */
  u32 heap_block_cnt = (u32) (realm_byte_size / HEAP_BLOCK_SIZE);
  v_heap_block_object *heap_blocks = 
      calloc(heap_block_cnt, sizeof(v_heap_block_object));
  if (!heap_blocks) {
    return false;
  }
  v_heap.block_cnt = heap_block_cnt;
  v_heap_block_object *current_block = heap_blocks;
  while (heap_block_cnt --) {
    current_block->base_address = realm_base_address;
    current_block->pos_address = realm_base_address;
    realm_base_address = (void *) 
        ((u64) realm_base_address + HEAP_BLOCK_SIZE);
    current_block->roof_address = (void *) 
        ((u64) realm_base_address - 1);
    current_block ++;
  }
  v_heap.blocks = heap_blocks;
  return true;
}

void 
v_initialize_heap(u64 total_byte_size, u64 hyperspace_byte_size) {
  u8 *heap_alloc_ptr = 
/**
 * Allocate the heap based on the type of the operating system, 
 * for Windows we will use VirtualAlloc, and for Unix-based we 
 * will use mmap.
 */
#ifdef OS_WINDOWS
    (u8 *) _virtual_alloc(total_byte_size);
#endif
  if (!heap_alloc_ptr) {
    goto error;
  }
  v_heap.total_byte_size = total_byte_size;
  v_heap.base_address = heap_alloc_ptr;
  v_heap.roof_address = (void *) 
      ((u64) heap_alloc_ptr + total_byte_size);
  if (!_initialize_hyperspace(heap_alloc_ptr, 
      hyperspace_byte_size)) {
    _virtual_free(heap_alloc_ptr);
    goto error;
  }
  if (!_initialize_heap_blocks(heap_alloc_ptr, 
      total_byte_size, hyperspace_byte_size)) {
    _virtual_free(heap_alloc_ptr);
    goto error;
  }
  return;
  error:
  FATAL("heap initialization failed...");
}

void v_free_heap() {
  free(v_heap.blocks);
  if (false !=
#ifdef OS_WINDOWS
  _virtual_free(v_heap.base_address)
#endif
  ) {
    // TODO: Handle error case of VirtualFree.
  }
}

boo 
/**
 * STEPS:
 * 1. Lookup for an avaliable pointer index.
 * 2. Allocate the requested byte size in the first avaliable block.
 */
v_allocate_pointer(u32 *ptr_idx, u8 **alloc_address, u64 byte_size) {
  /**
   * Allocation byte size bigger than block size is not allowed.
   */
  if (byte_size > HEAP_BLOCK_SIZE) {
    return false;
  }
  /**
   * Use of references to reduce address lookup, the "hyperspace" object 
   * of the heap is frequently accessed by the subsequent logic.
   */
  struct _hyperspace_object *hyperspace = &v_heap.hyperspace;
  /**
   * Temporary storage of the address of the pointer within the hyperspace.
   */
  u64 *ptr_address;
  /**
   * If the index position is not at the top, returns 
   * the index position to speed up lookup time.
   */
  if (hyperspace->idx_pos <= hyperspace->max_idx) {
    ptr_address = hyperspace->base_address + hyperspace->idx_pos;
    *ptr_idx = hyperspace->idx_pos ++;
  } else {
    /**
     * Lookup for empty pointer index for the allocation.
     */
    *ptr_idx = 0;
    ptr_address = hyperspace->base_address;
    /**
     * Unused pointers stores zero value => ((void *) 0) NULL.
     */
    while ((void *) *ptr_address == NULL) {
      ptr_address ++;
      *ptr_idx ++;
      if (*ptr_idx > hyperspace->max_idx) {
        return false;
      }
    }
  }
  /**
   * STEP:  Look for a avaliable lock with capacity 
   *        for the requested byte size.
   */
  u32 heap_block_idx = 0;
  /**
   * Use of references to reduce address lookup, the "blocks" list 
   * of the heap is frequently accessed by the subsequent logic.
   */
  v_heap_block_object *block = v_heap.blocks;
  while (heap_block_idx < v_heap.block_cnt) {
    /**
     * Select the block if its empty.
     */
    if (block->base_address == block->pos_address) {
      break;
    } 
    /**
     * Select the block if it will not 
     * overflow after the allocation.
     */
    else if ( (u64) block->pos_address + byte_size 
                < (u64) block->roof_address) {
      break;
    }
    block ++;
    heap_block_idx ++;
  }
  /**
   * No avaliable block for the allocation.
   */
  if (heap_block_idx > v_heap.block_cnt) {
    return false;
  }
  *ptr_address = (u64) block->pos_address;
  *alloc_address = block->pos_address;
  block->pos_address += byte_size;
  return true;
}
