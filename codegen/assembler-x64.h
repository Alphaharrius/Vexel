#ifndef ASSEMBLER_X64_H
#define ASSEMBLER_X64_H
#pragma once

#include "../core/v-type.h"
#include "code-object.h"

#define REX   0x40
#define REX_W 0x08
#define REX_R 0x04
#define REX_X 0x02
#define REX_B 0x01

#define RM_MOD_PTR     0x00
#define RM_MOD_DISP8   0x40
#define RM_MOD_DISP32  0x80
#define RM_MOD_RTR     0xC0

#define R_64(RTR)  RTR | 0x10

#define R_AX  0x00
#define R_BX  0x03
#define R_CX  0x01
#define R_DX  0x02
#define R_SI  0x06
#define R_DI  0x07
#define R_R8  0x08
#define R_R9  0x09
#define R_R10 0x0A
#define R_R11 0x0B
#define R_R12 0x0C
#define R_R13 0x0D
#define R_R14 0x0E
#define R_R15 0x0F

#define INLINE_VOID static inline void

INLINE_VOID
add_rm16_r16(code_object *code, u8 rm_mod, u8 op_rm, u8 op_r);

INLINE_VOID
add_r16_rm16(code_object *code, u8 rm_mod, u8 op_r, u8 op_rm);

INLINE_VOID
add_rm32_r32(code_object *code, u8 rm_mod, u8 op_rm, u8 op_r);

INLINE_VOID
add_r32_rm32(code_object *code, u8 rm_mod, u8 op_r, u8 op_rm);

INLINE_VOID
add_rm64_r64(code_object *code, u8 rm_mod, u8 op_rm, u8 op_r);

INLINE_VOID
add_r64_rm64(code_object *code, u8 rm_mod, u8 op_r, u8 op_rm);

INLINE_VOID
mov_rm16_r16(code_object *code, u8 rm_mod, u8 op_rm, u8 op_r);

INLINE_VOID
mov_r16_rm16(code_object *code, u8 rm_mod, u8 op_r, u8 op_rm);

INLINE_VOID 
mov_rm32_r32(code_object *code, u8 rm_mod, u8 op_rm,u8 op_r);

INLINE_VOID 
mov_r32_rm32(code_object *code, u8 rm_mod, u8 op_r, u8 op_rm);

INLINE_VOID 
mov_rm64_r64(code_object *code, u8 rm_mod, u8 op_rm, u8 op_r);

INLINE_VOID 
mov_r64_rm64(code_object *code, u8 rm_mod, u8 op_r, u8 op_rm);

INLINE_VOID
mov_r8_imm8(code_object *code, u8 op_r, u8 imm);

INLINE_VOID
mov_r16_imm16(code_object *code, u8 op_r, u16 imm);

INLINE_VOID
mov_r32_imm32(code_object *code, u8 op_r, u32 imm);

INLINE_VOID
mov_r64_imm64(code_object *code, u8 op_r, u64 imm);

INLINE_VOID
mov_rm8_imm8(code_object *code, u8 rm_mod, u8 op_rm, u8 imm);

INLINE_VOID
mov_rm16_imm16(code_object *code, u8 rm_mod, u8 op_rm, u16 imm);

INLINE_VOID
mov_rm32_imm32(code_object *code, u8 rm_mod, u8 op_rm, u32 imm);

#endif