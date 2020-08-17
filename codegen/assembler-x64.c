#include "assembler-x64.h"

INLINE_VOID
emit_u8(code_object *code, u8 byte) {
  *(code->code + code->pos++) = byte;
}

INLINE_VOID
emit_u16(code_object *code, u16 word) {
  *(u16 *) (code->code + code->pos) = word;
  code->pos += SIZE_16;
}

INLINE_VOID
emit_u32(code_object *code, u32 dword) {
  *(u32 *) (code->code + code->pos) = dword;
  code->pos += SIZE_32;
}

INLINE_VOID
emit_u64(code_object *code, u64 qword) {
  *(u64 *) (code->code + code->pos) = qword;
  code->pos += SIZE_64;
}

INLINE_VOID
/**
 * The shared logic for both the 
 * register->memory and memory->register 
 * code generation.
 */
emit_rex(code_object *code,u8 op_rm,u8 op_r) {
  /**
   * Defined in the header file, 
   * all operand with the prefix 
   * macro of R_64 or belongs to 
   * the x64 register expansion 
   * will require a REX prefix.
   */
  if ((op_rm | op_r) & 0x18) {
    u8 rex = REX;

    /**
     * The REX.W bit is required 
     * for enabling 64 bit operand, 
     * the value (0x10) is given 
     * by the R_64 macros.
     */
    if ((op_rm | op_r) & 0x10) {
      rex |= REX_W;
    }

    /**
     * All x64 expanded registers 
     * have the common bit of (0x08), 
     * which will be placed in the 
     * REX.B for the RM operand.
     */
    if (op_rm & 0x08) {
      rex |= REX_B;
    }
    /**
     * And the REX.R bit 
     * for the R operand.
     */
    if (op_r & 0x08) {
      rex |= REX_R;
    }

    emit_u8(code, rex);
  }
}

INLINE_VOID 
op_en_mr_emit(code_object *code, u8 opc, u8 rm_mod, u8 op_rm, u8 op_r) {
  emit_rex(code, op_rm, op_r);
  emit_u8(code, opc);
  /**
   * ModRM:
   * [[rm_mod:2][op_rm:3][op_r:3]]
   */
  emit_u8(code, (rm_mod | ((op_rm & 0x07) << 3) | (op_r & 0x07)));
}

INLINE_VOID 
op_en_rm_emit(code_object *code, u8 opc, u8 rm_mod, u8 op_r, u8 op_rm) {
  emit_rex(code, op_rm, op_r);
  emit_u8(code, opc);
  /**
   * ModRM:
   * [[rm_mod:2][op_r:3][op_rm:3]]
   */
  emit_u8(code, (rm_mod | ((op_r & 0x07) << 3) | (op_rm & 0x07)));
}

INLINE_VOID
op_en_oi_imm8_emit(code_object *code, u8 opc, u8 op_r, u8 imm) {
  emit_u8(code, opc + op_r);
  emit_u8(code, imm);
}

INLINE_VOID
op_en_oi_imm16_emit(code_object *code, u8 opc, u8 op_r, u16 imm) {
  emit_u8(code, opc + op_r);
  emit_u16(code, imm);
}

INLINE_VOID
op_en_oi_imm32_emit(code_object *code, u8 opc, u8 op_r, u32 imm) {
  emit_u8(code, opc + op_r);
  emit_u32(code, imm);
}

INLINE_VOID
op_en_oi_imm64_emit(code_object *code, u8 opc, u8 op_r, u64 imm) {
  emit_u8(code, REX | REX_W);
  emit_u8(code, opc + op_r);
  emit_u64(code, imm);
}

INLINE_VOID
op_en_mi_header_emit(code_object *code, u8 opc, u8 rm_mod, u8 op_rm) {
  emit_rex(code, op_rm, 0x00);
  emit_u8(code, opc);
  emit_u8(code, (rm_mod | ((op_rm & 0x07) << 3)));
}

INLINE_VOID
add_rm16_r16(code_object *code, u8 rm_mod, u8 op_rm, u8 op_r) {
  emit_u8(code, 0x66);
  op_en_rm_emit(code, 0x01, rm_mod, op_rm, op_r);
}

INLINE_VOID
add_r16_rm16(code_object *code, u8 rm_mod, u8 op_r, u8 op_rm) {
  emit_u8(code, 0x66);
  op_en_rm_emit(code, 0x03, rm_mod, op_r, op_rm);
}

INLINE_VOID
add_rm32_r32(code_object *code, u8 rm_mod, u8 op_rm, u8 op_r) {
  op_en_mr_emit(code, 0x01, rm_mod, op_rm, op_r);
}

INLINE_VOID
add_r32_rm32(code_object *code, u8 rm_mod, u8 op_r, u8 op_rm) {
  op_en_rm_emit(code, 0x03, rm_mod, op_r, op_rm);
}

INLINE_VOID
add_rm64_r64(code_object *code, u8 rm_mod, u8 op_rm, u8 op_r) {
  op_en_mr_emit(code, 0x01, rm_mod, R_64(op_rm), R_64(op_r));
}

INLINE_VOID
add_r64_rm64(code_object *code, u8 rm_mod, u8 op_r, u8 op_rm) {
  op_en_rm_emit(code, 0x03, rm_mod, R_64(op_r), R_64(op_rm));
}

INLINE_VOID
mov_rm16_r16(code_object *code, u8 rm_mod, u8 op_rm, u8 op_r) {
  emit_u8(code, 0x66);
  op_en_mr_emit(code, 0x89, rm_mod, op_rm, op_r);
}

INLINE_VOID
mov_r16_rm16(code_object *code, u8 rm_mod, u8 op_r, u8 op_rm) {
  emit_u8(code, 0x66);
  op_en_rm_emit(code, 0x8B, rm_mod, op_rm, op_r);
}

INLINE_VOID 
mov_rm32_r32(code_object *code, u8 rm_mod, u8 op_rm,u8 op_r) {
  op_en_mr_emit(code, 0x89, rm_mod, op_rm, op_r);
}

INLINE_VOID 
mov_r32_rm32(code_object *code, u8 rm_mod, u8 op_r, u8 op_rm) {
  op_en_rm_emit(code, 0x8B, rm_mod, op_r, op_rm);
}

INLINE_VOID 
mov_rm64_r64(code_object *code, u8 rm_mod, u8 op_rm, u8 op_r) {
  op_en_mr_emit(code, 0x89, rm_mod, R_64(op_rm), R_64(op_r));
}

INLINE_VOID 
mov_r64_rm64(code_object *code, u8 rm_mod, u8 op_r, u8 op_rm) {
  op_en_rm_emit(code, 0x8B, rm_mod, R_64(op_r), R_64(op_rm));
}

INLINE_VOID
mov_r8_imm8(code_object *code, u8 op_r, u8 imm) {
  op_en_oi_imm8_emit(code, 0xB0, op_r, imm);
}

INLINE_VOID
mov_r16_imm16(code_object *code, u8 op_r, u16 imm) {
  op_en_oi_imm16_emit(code, 0xB8, op_r, imm);
}

INLINE_VOID
mov_r32_imm32(code_object *code, u8 op_r, u32 imm) {
  op_en_oi_imm32_emit(code, 0xB8, op_r, imm);
}

INLINE_VOID
mov_r64_imm64(code_object *code, u8 op_r, u64 imm) {
  op_en_oi_imm64_emit(code, 0xB8, op_r, imm);
}

INLINE_VOID
mov_rm8_imm8(code_object *code, u8 rm_mod, u8 op_rm, u8 imm) {
  op_en_mi_header_emit(code, 0xC6, rm_mod, op_rm);
  emit_u8(code, imm);
}

INLINE_VOID
mov_rm16_imm16(code_object *code, u8 rm_mod, u8 op_rm, u16 imm) {
  op_en_mi_header_emit(code, 0xC7, rm_mod, op_rm);
  emit_u16(code, imm);
}

INLINE_VOID
mov_rm32_imm32(code_object *code, u8 rm_mod, u8 op_rm, u32 imm) {
  op_en_mi_header_emit(code, 0xC7, rm_mod, op_rm);
  emit_u32(code, imm);
}


