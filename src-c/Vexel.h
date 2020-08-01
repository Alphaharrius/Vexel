#ifndef VEXEL_H
#define VEXEL_H
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
 * Vexel.h
 */
#include <stdio.h>
#ifndef NULL
#error "NULL is undefined in stdio.h"
#endif

#include <stdlib.h>
#include <string.h>

#include "sys_info.h"

#include "v-log.h"
#include "v-error.h"
#include "v-heap.h"
/**
 * Each vm will have one global accessible process. This process 
 * will contains an array of active threads, with the first thread 
 * as the main thread, all other threads will be collected if they 
 * does not have any references from the main thread and they have 
 * a suspended state.
 */
#include "v-thread.h"
/**
 * Each thread will contains one frame object as their execution 
 * unit, the frame object will be passed to the interpreter along 
 * with the thread object for evaluation.
 */
#include "v-frame.h"

#endif
