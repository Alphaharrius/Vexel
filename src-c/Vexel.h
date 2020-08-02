#ifndef VEXEL_H
#define VEXEL_H
/**
 * Copyright (c) 2019, 2020, Alphaharrius. All rights reserved.
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
#include "v-oballoc.h"
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
