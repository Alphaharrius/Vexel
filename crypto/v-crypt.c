#include "../core/Vexel.h"

#if defined(WINDOWS)

/**
 * Initializing the handles for the Windows 
 * Cryptography (Next Generation) API.
 */
#if defined(__BCRYPT_H__)
static void *BCRY_HANDLE = NULL;
#endif

#if defined(__WINCRYPT_H__)
static u64 CRY_PROV = 0;
#endif

static u8
windows_initialize_crypt(void)
{
#if defined(__BCRYPT_H__)
#define NT_STAT_OK(stat) (((NTSTATUS) (stat)) >= 0)
#define NT_STAT_FAIL (NTSTATUS) 0xC0000001L

  if (NT_STAT_OK( BCryptOpenAlgorithmProvider(&BCRY_HANDLE, 
                  BCRYPT_SHA256_ALGORITHM, NULL, 0))) {
    return TRUE;
  }

#endif // __BCRYPT_H__

#if defined(__WINCRYPT_H__)

  if (CryptAcquireContext(&CRY_PROV, NULL, NULL, PROV_RSA_FULL, 0)) {

    return TRUE;
  }

#endif // __WINCRYPT_H__

  return FALSE;
}

static u8
windows_system_generate_random(u8 *buf, u32 len)
{
  if (len > 256) {
    return FALSE;
  }

#if defined(__BCRYPT_H__)

  if (BCRY_HANDLE &&  NT_STAT_OK(BCryptGenRandom(BCRY_HANDLE, 
                      buf, len, BCRYPT_USE_SYSTEM_PREFERRED_RNG))) {
    
    return TRUE;
  }

#endif

#if defined(__WINCRYPT_H__)

  if (CRY_PROV && CryptGenRandom(CRY_PROV, len, buf)) {

    return TRUE;
  }

#endif

  return FALSE;
}

#endif // WINDOWS

int main(void) {

  printf("%llx\n", windows_initialize_crypt());

  return 0;

}
