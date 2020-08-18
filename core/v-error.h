#ifndef V_ERROR_H
#define V_ERROR_H
#pragma once
/**
 * Copyright (c) 2019, 2020, Alphaharrius. All rights reserved.
 * 
 * v-error.h
 * This file implements the exception handler of the 
 * Vexel Virtual Machine for handling internal or 
 * runtime exceptions.
 */

#include "v-log.h"

#define EXIT_STAT_NONE   0
#define EXIT_STAT_FATAL  1
static void v_exit(u8 stat) {
  char text[48];
  if (sprintf(text, "process ended with status code: %d", stat) > 0) {
    LOG(COLOR_DEFAULT, "exit", text);
  } else {
    LOG(COLOR_DEFAULT, "exit", "process ended");
  }
  exit(stat);
}

#define FATAL(text) \
  LOG(COLOR_RED, "fatal", text); \
  v_exit(EXIT_STAT_FATAL);

typedef enum {
  V_ERR_NONE,
  V_ERR_API_INV_CALL,
  V_ERR_HEAP_OUT_OF_MEM,
  V_ERR_HEAP_NO_IDX,
  V_ERR_HEAP_NO_BLK,
  V_ERR_OB_NULL,
  V_ERR_OB_NOT_VAR,
  V_ERR_OB_NOT_LST,
  V_ERR_OB_NOT_MAP,
  V_ERR_OB_TYP_UNMATCH,
  V_ERR_LST_MAX_LEN
} v_err;

#define IS_ERR(status) status != V_ERR_NONE

#endif
