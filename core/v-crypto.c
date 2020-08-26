#include "../core/Vexel.h"

#define C_K_LEN 7

static u64 C_K[C_K_LEN] = {
  0xEB2DF331CD23AC43,
  0xCD23AC43BB4681C3,
  0xBB4681C3B54FCBAF,
  0xB54FCBAFDF08ED0B,
  0xDF08ED0BD913138F,
  0xD913138FE4FECE2D,
  0xE4FECE2DEB2DF331
};

void 
v_initialize_crypto(void) {

#ifdef WINDOWS

  HCRYPTPROV h_crypt_prov;
  u8 dat[C_K_LEN * SIZE_64];

  if (!CryptAcquireContext( &h_crypt_prov, 
                            NULL, 
                            NULL, 
                            PROV_RSA_FULL,
                            0)) {

    goto error;
  }

  if (!CryptGenRandom(h_crypt_prov, 
                      C_K_LEN * SIZE_64, 
                      dat)) {
    
    goto error;
  }
  
  u64 *rand = V_QPTR(dat);

  for (u8 i = 0; i < C_K_LEN; i++) {
    C_K[i] ^= *rand++;
  }

#endif

  return;

  error:
  FATAL("error during crypto key generation...");
}

u64 
vm_crypto_hash_bytes(u8 *dat, u32 len)
{
  u64 K[C_K_LEN] = {
    /**
     * Copy the crypto keys from 
     * the current process.
     */
    C_K[0],
    C_K[1],
    C_K[2],
    C_K[3],
    C_K[4],
    C_K[5],
    C_K[6]
  };

  /**
   * Initialize the hash code to the first crypto key.
   */
  u64 h = K[len % C_K_LEN];

#define ROT64(n) (h << n) | (h >> (64 - n))

  u8 i = 0, f;
  
  while (len--) {
    u8 c = *dat++;

    /**
     * Multiply the hash by the byte value 
     * to ensure the "randomness" provided 
     * by the unrecoverable carry bit of the 
     * qword arithematics.
     */
    h *= c;

    /**
     * Rotate the hash value by the byte value 
     * to mix up the combinations.
     */
    f = c % 8;
    h = ROT64(f);

    /**
     * Increment the hash by the crypto key 
     * selected by the byte value to ensure 
     * the crypto safety of the hash output.
     */
    u8 m = c % C_K_LEN;
    h += K[m];
    /**
     * Augment the keys to add combination 
     * relavence to the hash.
     */
    K[m] += h;

    /**
     * XOR the hash value by another 
     * crypto key selected by a value 
     * specific to the combination of 
     * the byte list to induce more 
     * randomness to the hash value.
     */
    i += f;
    m = i % C_K_LEN;
    h ^= K[m];
    /**
     * Augment the keys to add combination 
     * relavence to the hash.
     */
    if (len) {
      K[m] ^= h;
    }
  }

  /**
   * Mix the hash to generate 
   * a better randomness.
   */
  h = ROT64(i % 8);
  h ^= K[i % C_K_LEN];
  h = ROT64(i % 8);
  h += K[f % C_K_LEN];

#undef ROT64

  return h;
}

#undef CRYPT_KEY
#undef C_K_LEN
