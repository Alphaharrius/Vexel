#ifndef V_ERROR_H
#define V_ERROR_H
/**
 * Copyright (c) 2019, 2020, Alphaharrius. All rights reserved.
 */
#include "v-type.h"
/**
 * Used on critical error, the 
 * process will be terminated.
 */
#define FATAL(message) \
  PRINT(COLOR_RED, "fatal", message); \
  exit(1);

typedef struct {
  u16 error_type;
  u64 opcode_line;
  struct v_thread_object *error_thread;
  struct v_frame_object *error_frame;
} v_error_type;

void v_throw_error( struct v_thread_object *thread, 
                    struct v_frame_object *frame, 
                    u64 opcode_line, 
                    u16 error_type);

#endif
