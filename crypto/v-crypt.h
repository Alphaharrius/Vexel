#ifndef V_CRYPT_H
#define V_CRYPT_H
#pragma once

#define WARN_UNSECURE_CRYPT \
  "native support to secure random generation " \
  "is unavaliable, fall back to insecure crypto " \
  "environment."

#define WARN_CRY_KEN_GEN_FAIL \
  "crypto-key generation failed, " \
  "fall back to default unsecure " \
  "predefined key..."

void Ve_InitializeCryptEnvironment(void);

void Interface_SysGetRandomBytes(u8 *bytes, u16 len);

void Interface_GetRandomBytes(u64 seed, u8 *bytes, u16 len);

u64 Interface_HashBytesSimple(u8 *bytes, u64 len);

#endif
