#ifndef V_ASSEMBLER_H
#define V_ASSEMBLER_H
#pragma once

#include <stdlib.h>
#include <stdio.h>

#include "../core/v-type.h"

typedef struct {
  u8 *code;
  u8 pos;
} code_object;

code_object *make_code_object(u64 size);

#endif