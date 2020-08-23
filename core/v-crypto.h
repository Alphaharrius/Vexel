#ifndef V_HASH_H
#define V_HASH_H
#pragma once
/**
 * Copyright (c) 2019, 2020, Alphaharrius. All rights reserved.
 * 
 * v-crypto.h
 */

#include "v-type.h"

/**
 * Initialize the process crypto environment, 
 * which consists of 7 crypto keys for vm usages. 
 * This method must be executed successfully 
 * for the vm to start up correctly, failed 
 * to execute results in the termination of 
 * current vm.
 */
void v_initialize_crypto(void);

/**
 * Hash a byte list with a given length into 64 bit 
 * hash by a byte-wise add-rotate-subsitute-xor process.
 * This hashing process is enhanced by native crypto 
 * random key generation, thus the hash code generation 
 * output is unique per vm process, and crypto-safe.
 * @param dat: The byte list to be hashed.
 * @param len: The length of the byte list.
 * @return: The hash code in qword.
 */
u64 vm_crypto_hash_bytes(u8 *dat, u32 len);

u64 vm_crypto_sys_random(void);

#endif
