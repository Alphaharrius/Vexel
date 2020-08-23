#include "../core/Vexel.h"

#define CRYPT_KEYS_LEN 7

static u64 CRYPT_BUF[CRYPT_KEYS_LEN] = {
  0xEB2DF331CD23AC43,
  0xCD23AC43BB4681C3,
  0xBB4681C3B54FCBAF,
  0xB54FCBAFDF08ED0B,
  0xDF08ED0BD913138F,
  0xD913138FE4FECE2D,
  0xE4FECE2DEB2DF331
};

#define CRYPT_KEY(n) CRYPT_BUF[n % CRYPT_KEYS_LEN]

void 
v_initialize_crypto(void) {

#ifdef WINDOWS

  HCRYPTPROV h_crypt_prov;
  u8 dat[CRYPT_KEYS_LEN * SIZE_64];

  if (!CryptAcquireContext( &h_crypt_prov, 
                            NULL, 
                            NULL, 
                            PROV_RSA_FULL,
                            0)) {

    goto error;
  }

  if (!CryptGenRandom(h_crypt_prov, 
                      CRYPT_KEYS_LEN * SIZE_64, 
                      dat)) {
    
    goto error;
  }
  
  u64 *rand = V_QPTR(dat);

  for (u8 i = 0; i < CRYPT_KEYS_LEN; i++) {
    CRYPT_BUF[i] ^= *rand++;
  }

#endif

  return;

  error:
  FATAL("error during crypto key generation...");
}

u64 
vm_crypto_hash_bytes(u8 *dat, u32 len)
{
  /**
   * Initialize the hash code to the first crypto key.
   */
  u64 hash = CRYPT_KEY(CRYPT_KEYS_LEN);

  u8 i = 0, f;
  /**
   * For all bytes in the list, perform 
   * the add-rotate-subsitute-xor process.
   */
  while (len--) {
    char c = *dat++;

    /**
     * Increment the hash by the byte value 
     * to ensure the "randomness" provided 
     * by the unrecoverable carry bit of the 
     * qword arithematics.
     */
    hash += c;

    /**
     * Rotate the hash value by the byte value 
     * to mix up the combinations.
     */
    f = c % 8;
    hash = (hash << f) | (hash >> (64 - f));
    /**
     * Increment the hash by the crypto key 
     * selected by the byte value to ensure 
     * the crypto safety of the hash output.
     */
    hash += CRYPT_KEY(c);
    /**
     * XOR the hash value by another 
     * crypto key selected by a value 
     * specific to the combination of 
     * the byte list to induce more 
     * randomness to the hash value.
     */
    i += f;
    hash ^= CRYPT_KEY(i);
  }

  return hash;
}

#undef CRYPT_KEY
#undef CRYPT_KEYS_LEN
