#ifndef V_POINTER_H
#define V_POINTER_H
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
 * v-pointer.h
 */
#include "v-type.h"
/**
 * Pointers have a structure of a consecutive memory 
 * allocation, while C struct cannot be applied to this 
 * case of "inheritance" of pointer types.
 * Pointer types:
 * data: pointer referencing a single allocation
 *  sub-types: bool, int, float
 * buffer: pointer referencing an array of allocation
 *  sub-types: string, array, list
 * map: pointer referencing a map structure with key-value pairs
 *  sub-types: map
 * Pointer structure: [type:b8][*vals:b64][size:b8][len:b64][*keys:b64]
 *  data: "type", "val", "size"
 *  buffer: "type", "val", "size", "len"
 *  map: "type", "val", "size", "len", "keys"
 */
typedef void v_pointer_object;
// Size of each element, offset 1 byte for the "type" property
#define V_PTR_PROP_VAL_OFF  SIZE_U8
// Pointer towards the value, offset 8 bytes for the "val" pointer property
#define V_PTR_PROP_SIZE_OFF V_PTR_PROP_VAL_OFF + SIZE_U64
// Length of the buffer, offset 1 byte for the "size" property
#define V_PTR_PROP_LEN_OFF  V_PTR_PROP_SIZE_OFF + SIZE_U8
// Pointer towards the key list, offset 8 bytes for the "len" property
#define V_PTR_PROP_KEYS_OFF V_PTR_PROP_LEN_OFF + SIZE_U64
#define V_DATA_PTR_SIZE     V_PTR_PROP_LEN_OFF
#define V_BUFF_PTR_SIZE     V_PTR_PROP_KEYS_OFF
#define V_MAP_PTR_SIZE      V_PTR_PROP_KEYS_OFF + SIZE_U64
#define V_PTR_PROP_TYPE(pointer) * (u8 *) pointer
#define V_PTR_PROP_ALLOC(pointer) * (u64 *) ((u64) pointer + V_PTR_PROP_VAL_OFF)
#define V_PTR_PROP_SIZE(pointer) * (u16 *) ((u64) pointer + V_PTR_PROP_SIZE_OFF)
#define V_PTR_PROP_LEN(pointer) * (u64 *) ((u64) pointer + V_PTR_PROP_LEN_OFF)
#define V_PTR_PROP_KEYS(pointer) * (u64 *) ((u64) pointer + V_PTR_PROP_KEYS_OFF)
/**
 * The "linked" property is unique for the buffer pointer type, implements the 
 * sectional linked list for huge allocation of buffer, this property stores the 
 * pointer for the next sector of the buffer. As this property is positioned at 
 * the same location of the "keys" prop, the offset for "keys" is reused.
 */
#define V_PTR_PROP_LINKED(pointer) * (u64 *) ((u64) pointer + V_PTR_PROP_KEYS_OFF)

v_pointer_object * v_make_data_pointer(u8 type, u8 size);
v_pointer_object * v_make_buff_pointer(u8 type, u8 size, u64 len);
v_pointer_object * v_make_map_pointer();

#endif
