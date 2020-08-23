#include "core/Vexel.h"

u32 (*mutex_queue)(u32 *, u32 *);
u32 (*mutex_unqueue)(u32 *, u32 *, u32);

void initialize_mutex_lock () {
  char code[] = { 0xB8, 0x02, 0x00, 0x00, 0x00, 0xBB, 0x00, 0x00, 0x00, 0x00, 0x67, 0x87, 0x19, 0x83, 0xFB, 0x00, 0x74, 0xF3, 0xEB, 0x03, 0x67, 0x8B, 0x19, 0x39, 0xC3, 0x74, 0x12, 0x83, 0xF8, 0x02, 0x77, 0xF4, 0x67, 0x8B, 0x02, 0xFF, 0xC0, 0x67, 0x89, 0x02, 0x67, 0x89, 0x19, 0xEB, 0xE7, 0xBB, 0x01, 0x00, 0x00, 0x00, 0x67, 0x89, 0x19, 0xC3 };
  void *buf = VirtualAlloc(NULL, sizeof(code), MEM_COMMIT, PAGE_READWRITE);
  if (!buf) {
    printf("failed to alloc\n");
    return;
  }
  memcpy(buf, code, sizeof(code));
  u32 tmp;
  if (!VirtualProtect(buf, sizeof(code), PAGE_EXECUTE_READ, (PDWORD) &tmp)) {
    printf("failed to protect\n");
    return;
  }
  FlushInstructionCache(GetCurrentProcess(), buf, sizeof(code));
  mutex_queue = (u32 (*)(u32 *, u32 *)) buf;
  return;
}

void initialize_mutex_unlock () {
  char code[] = { 0xB8, 0x02, 0x00, 0x00, 0x00, 0xBB, 0x00, 0x00, 0x00, 0x00, 0x67, 0x87, 0x19, 0x83, 0xFB, 0x00, 0x74, 0xF3, 0xEB, 0x03, 0x67, 0x8B, 0x19, 0x39, 0xC3, 0x74, 0x12, 0x83, 0xF8, 0x02, 0x75, 0xF4, 0x67, 0x8B, 0x02, 0xFF, 0xC0, 0x67, 0x89, 0x02, 0x67, 0x89, 0x19, 0xEB, 0x0D, 0x83, 0xF8, 0x02, 0x75, 0x08, 0xBF, 0x01, 0x00, 0x00, 0x00, 0x67, 0x89, 0x39, 0xC3 };
  void *buf = VirtualAlloc(NULL, sizeof(code), MEM_COMMIT, PAGE_READWRITE);
  if (!buf) {
    printf("failed to alloc\n");
    return;
  }
  memcpy(buf, code, sizeof(code));
  u32 tmp;
  if (!VirtualProtect(buf, sizeof(code), PAGE_EXECUTE_READ, (PDWORD) &tmp)) {
    printf("failed to protect\n");
    return;
  }
  mutex_unqueue = (u32 (*)(u32 *, u32 *, u32)) buf;
  return;
}

unsigned int flag = 0x02;
unsigned int counter = 0x02;

DWORD WINAPI runnable() {
  u32 idx = mutex_queue(&flag, &counter);
  printf("idx=%d\n", idx);
  printf("counter=%d\n", counter);
  Sleep(2000);
  flag = idx + 1;
  return 0;
}

int main() {
  initialize_mutex_lock();
  initialize_mutex_unlock();

  // u32 idx = mutex_queue(&flag, &counter);
  // printf("Entering queue, idx=%d, flag=%d, counter=%d\n", idx, flag, counter);
  DWORD thread_id;
  HANDLE threads[5];
  threads[0] = CreateThread(NULL, 0, runnable, NULL, 0, &thread_id);
  threads[1] = CreateThread(NULL, 0, runnable, NULL, 0, &thread_id);
  threads[2] = CreateThread(NULL, 0, runnable, NULL, 0, &thread_id);
  threads[3] = CreateThread(NULL, 0, runnable, NULL, 0, &thread_id);
  threads[4] = CreateThread(NULL, 0, runnable, NULL, 0, &thread_id);
  WaitForMultipleObjects(5, threads, TRUE, INFINITE);

  return 0;
}

// INLINE_VOID
// /**
//  * Emit code for instructions that 
//  * requires operand of register and 
//  * register/memory encoded by ModRM. 
//  * This method handled the REX flag 
//  * set when x64 register expansions 
//  * are used.
//  */
// rxx_rmxx_emit(code_object *code, 
//               u8 opc, 
//               u8 rm_mod, 
//               u8 op_1, 
//               u8 op_2) 
// {
//   u8 rex = 0x00;
//   if ((op_1 | op_2) & 0x18) {
//     rex = REX;

//     if ((op_1 | op_2) & 0x10) {
//       rex |= REX_W;
//     }

// #define ADJ_REX(op) \
//   if (op & 0x08) { \
//     if (op & 0x20) { \
//       rex |= REX_R; \
//     } else if (op & 0x40) { \
//       rex |= REX_B; \
//     } \
//   }

//     ADJ_REX(op_1);
//     ADJ_REX(op_2);

// #undef ADJ_REX

// #define EMIT_BYTE(byte) \
//   *(code->code + code->pos++) = byte

//     EMIT_BYTE(rex);
//   }

//   EMIT_BYTE(opc);
//   EMIT_BYTE((rm_mod | 
//             ((op_1 & 0x07) << 3) | 
//             (op_2 & 0x07)));

// #undef EMIT_BYTE
// }

// INLINE_VOID
// mov_rm32_r32(code_object *code, u8 rm_mod, u8 des, u8 src) 
// {
//   rxx_rmxx_emit(code, 0x89, rm_mod, R_R(src), R_RM(des));
// }

// INLINE_VOID
// mov_r32_rm32(code_object *code, u8 rm_mod, u8 des, u8 src) 
// {
//   rxx_rmxx_emit(code, 0x8B, rm_mod, R_R(des), R_RM(src));
// }

// INLINE_VOID
// mov_rm64_r64(code_object *code, u8 rm_mod, u8 des, u8 src) 
// {
//   rxx_rmxx_emit(code, 0x89, rm_mod, R_R(R_64(src)), R_RM(R_64(des)));
// }

// INLINE_VOID
// mov_r64_rm64(code_object *code, u8 rm_mod, u8 des, u8 src) 
// {
//   rxx_rmxx_emit(code, 0x8B, rm_mod, R_R(R_64(des)), R_RM(R_64(src)));
// }

// INLINE_VOID
// x86_r32_imm32_emit( code_object *code, 
//                     u8 opc, 
//                     u8 des, 
//                     u32 imm) 
// {
//   *(code->code + code->pos++) = opc + des;

//   *(u32 *) (code->code + code->pos) = imm;
//   code->pos += SIZE_32;
// }

// INLINE_VOID
// x64_rxx_immxx_emit( code_object *code,
//                     u8 opc,
//                     u8 des,
//                     u64 imm) 
// {
//   if (!(des & 0x18)) {
//     return x86_r32_imm32_emit(code, opc, des, imm);
//   }

//   rxx_rmxx_emit(code, opc, RM_MOD_RTR, R_R(des), 0);
// }

// int main() {
//   code_object *code = make_code_object(64);
//   mov_r32_rm32(code, RM_MOD_PTR, R_AX, R_BX);
//   mov_rm32_r32(code, RM_MOD_PTR, R_AX, R_R8);
//   mov_rm64_r64(code, RM_MOD_PTR, R_BX, R_AX);
//   mov_r64_rm64(code, RM_MOD_PTR, R_R8, R_R9);
//   x64_rxx_immxx_emit(code, )
//   for (u8 i = 0x00; i < code->pos; i++) {
//     printf("0x%llx ", code->code[i]);
//   }
//   return 0;
// }
