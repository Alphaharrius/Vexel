#include <stdio.h>
#include <stdint.h>
#include <Windows.h>

typedef uint32_t u32;
typedef uint64_t u64;

static void *(*virtual_alloc)(void *, u64, u32, u32) = (void *) &VirtualAlloc;

int main() {
  
  return 0;
}