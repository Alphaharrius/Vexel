#include "../core/Vexel.h"
/**
 * Copyright (c) 2019, 2020, Alphaharrius. All rights reserved.
 * 
 * v-crypt.c
 * 
 * This file implements the crypto-environment for the process, 
 * which provides secure cryptography related operation using 
 * native approach provided by the host operating system, fall 
 * back to unsecured standard library provided cryptography 
 * approach if secure options is unavaliable.
 */

/**
 * On initialization, 23 secure crypto keys 
 * will be generated for the current process 
 * for the use of hashing operations, key 
 * mixins for pueudo-random number generation 
 * or other cryptography related logic.
 */
#define K_LEN 23
u64 cry_keys[K_LEN];

#if defined(_WIN32)

/**
 * Check if the Windows Cryptography 
 * (Next Generation) API is avaliable 
 * in the build environment.
 */
#if defined(__BCRYPT_H__)

#define NT_STAT_OK(stat) (((NTSTATUS) (stat)) >= 0)

#if !defined(BCRYPT_RNG_ALG_HANDLE)
#define BCRYPT_RNG_ALG_HANDLE (BCRYPT_ALG_HANDLE) 0x00000081
#endif

#endif

/**
 * Check if the Windows Cryptography 
 * API is avaliable in the build 
 * environment.
 */
#if defined(__WINCRYPT_H__)
static u64 cry_prov = 0;
#endif

static u8
windows_initialize_crypt(void)
{
#if defined(__BCRYPT_H__)

  return TRUE;

#elif defined(__WINCRYPT_H__)

  if (CryptAcquireContext(&cry_prov, NULL, NULL, PROV_RSA_FULL, 0)) {

    return TRUE;
  }

#endif // __WINCRYPT_H__

  /**
   * If both cryptography environment failed 
   * to initialize, all cryptography related 
   * operations will be performed by an unsecure 
   * seeded pueudo random number generator 
   * provided by the standard library.
   */
  return FALSE;
}

static inline u8
windows_system_generate_random(u8 *buf, u32 len)
{
  if (len > 256) {
    return FALSE;
  }

#if defined(__BCRYPT_H__)

  /**
   * Generate the random bytes using the next generation API.
   */
  if (NT_STAT_OK(BCryptGenRandom(BCRYPT_RNG_ALG_HANDLE, 
      buf, len, 0))) {
    
    return TRUE;
  }

#endif

#if defined(__WINCRYPT_H__)

  /**
   * Generate the random bytes using the old API.
   */
  if (cry_prov && CryptGenRandom(cry_prov, len, buf)) {

    return TRUE;
  }

#endif

  /**
   * If both environment is unavaliable, 
   * fall back to standard library function 
   * for the generation.
   */
  return FALSE;
}

#endif // WINDOWS

#if defined(__linux__)

#endif // __linux__

void
Ve_InitializeCryptEnvironment(void)
{
#if defined(_WIN32)

  if (!windows_initialize_crypt()) {
    goto warn;
  }

#endif

  /**
   * Once the secure approach fails, 
   * this flag force the key generation 
   * to use the standard library pueudo 
   * random generator.
   */
  u8 use_unsecure = FALSE;
  for (u8 i = 0; i < K_LEN; i++) {

    if (!use_unsecure) {

#if defined(_WIN32)
      if (!windows_system_generate_random(Ve_BPTR(cry_keys + i), SIZE_64)) {
#elif defined(__linux__)
#endif

        Ve_WARN(WARN_CRY_KEN_GEN_FAIL);
        use_unsecure = TRUE;
      } 
      else {
        continue;
      }

      if (use_unsecure) {
        /**
         * Fall back to a predefined random 
         * byte sequence inplace of the system 
         * generated secure random bytes.
         */
        cry_keys[i] = 0xEB2DF331CD23AC43;
      }
    }
  }

  return;

  warn:
  Ve_WARN(WARN_UNSECURE_CRYPT);
}

void
Interface_SysGetRandomBytes(u8 *bytes, u16 len)
{
  /**
   * Limits the length to 256 bytes 
   * to ensure the success of the 
   * random bytes retrieval from 
   * host sources.
   */
  if (len > 256) {
    len = 256;
  }

#if defined(_WIN32)
  if (!windows_system_generate_random(bytes, len)) {
#endif

    /**
     * TODO: Get entropy from current process.
     * If secure generation is not supported, 
     * fall back to a virtual random generation 
     * approach which use the current process 
     * activity as a source of randomness, examples 
     * of which can be the heap allocation distribution, 
     * or the distribution of heap object slots, etc.
     */

  }
}

#include <bcrypt.h>

int main(void) {

  Ve_InitializeCryptEnvironment();

  u64 x;
  Interface_SysGetRandomBytes(Ve_BPTR(&x), SIZE_64);
  printf("%llx\n", x);

}
