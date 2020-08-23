#include "core/Vexel.h"

u32 (*mutex_lock)(u32 *);
u32 (*mutex_unlock)(u32 *);

void initialize_mutex_lock () {
  char code[] = { 0xB8, 0x01, 0x00, 0x00, 0x00, 0x67, 0x87, 0x01, 0x83, 0xF8, 0x00, 0x75, 0xF3, 0xC3 };
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
  mutex_lock = (u32 (*)(u32 *)) buf;
  return;
}

void initialize_mutex_unlock () {
  char code[] = { 0x48, 0xC7, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x48, 0x89, 0x01, 0xC3 };
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
  mutex_unlock = (u32 (*)(u32 *)) buf;
  return;
}

int x = 0;
int flag = 0;
DWORD WINAPI run(void *id) {
  while (1) {
    mutex_lock(&flag);
    if (x < 250) {
      x += 1;
      printf("0, x=%d\n", x);
      mutex_unlock(&flag);
    } else {
      mutex_unlock(&flag);
      break;
    }
  }
  return 0;
}

DWORD WINAPI run1(void *id) {
  while (1) {
    mutex_lock(&flag);
    if (x < 250) {
      x += 1;
      printf("1, x=%d\n", x);
      mutex_unlock(&flag);
    } else {
      mutex_unlock(&flag);
      break;
    }
  }
  return 0;
}

DWORD WINAPI run2(void *id) {
  while (1) {
    mutex_lock(&flag);
    if (x < 250) {
      x += 1;
      printf("2, x=%d\n", x);
      mutex_unlock(&flag);
    } else {
      mutex_unlock(&flag);
      break;
    }
  }
  return 0;
}

int main() {
  initialize_mutex_lock();
  initialize_mutex_unlock();

  DWORD thread_id;
  HANDLE threads[3];
  threads[0] = CreateThread(NULL, 0, run, NULL, 0, &thread_id);
  threads[1] = CreateThread(NULL, 0, run1, NULL, 0, &thread_id);
  threads[2] = CreateThread(NULL, 0, run2, NULL, 0, &thread_id);
  WaitForMultipleObjects(3, threads, TRUE, INFINITE);
  printf("x=%d\n", x);
  return 0;
}
