#include "d-conv.h"
#include <stdio.h>
/**
 * Copyright (c) 2019, 2020, Alphaharrius. All rights reserved.
 * 
 * d-conv.c
 */

#define D_EXP(h) (u16) ((h & (0x7FFULL << 52)) >> 52)
#define D_MAN(h) (u64) (h & (~0ULL >> 12) | (1ULL << 52))

typedef struct { int exp; u64 man; } StructDouble;

static double CACHE_POW[] = {
  1E1, 1E2, 1E3, 1E4, 1E5, 1E6, 1E7, 1E8, 
  1E9, 1E10, 1E11, 1E12, 1E13, 1E14, 1E15, 
  1E16, 1E17
};

static inline u8
generate_digits(u8 *str, u64 di, int exp)
{
  u8 *bse = str;
  str += 24;
  u8 di_len, gen_len;
  u8 skp = 0;

  while (di != 0) {
    u8 cd = di % 10;
    di /= 10;
    di_len++;
    if (cd) { skp = 1; }
    if (skp == 0) { continue; }
    *(--str) = '0' + cd;
    gen_len++;
  }

  *bse = *str++;
  *(bse + 1) = '.';
  if (gen_len > 1) {
    memcpy(bse + 2, str, gen_len - 1);
    str = bse + gen_len + 1;
  } else {
    str = bse + 2;
    *str++ = '0';
  }

  exp += di_len - 1;

  if (exp != 0) {
    *str++ = 'e';
    *str++ = exp > 0 ? '+' : '-';
    if (exp < 0) { exp = -exp; }
    u8 e0 = 0, e1 = 0, e2 = 0;
    e0 = exp % 10, exp /= 10;
    if (exp != 0) { e1 = exp % 10; exp /= 10;}
    if (exp != 0) { e2 = exp % 10; }
    if (e2)           { *str++ = '0' + e2; }
    if (e1 | e2)      { *str++ = '0' + e1; }
    if (e0 | e1 | e2) { *str++ = '0' + e0; }
  }

  *str = '\0';
  return (u8) (str - bse);
}

#define LOG2D10 0.3010299956639812

u8
Util_QuickDoubleConvert(u8 *str, double num)
{
  u64 u_num = *(u64 *) &num;

  u8 len = 0;
  if (u_num & (1ULL << 63)) { *str++ = '-', len++; }

  if (D_EXP(u_num) == (u16) 0x7FF) {
    if ((~0ULL >> 12) & u_num) {
      if (len > 0) { str--;}
      *str++ = 'n', *str++ = 'a', *str++ = 'n';
      return 3;
    }
    else { *str++ = 'i', *str++ = 'n', *str++ = 'f'; }
    return len + 3;
  }

  int exp = (int) D_EXP(u_num) - 1023;

  int pow = (int) ((double) exp * LOG2D10);
  int b_pow = pow;

  convert: ;
  if (pow > 0) {
    if (pow > 272)      { num *= 1E-272,  pow -= 272; } 
    else if (pow > 136) { num *= 1E-136,  pow -= 136; }
    else if (pow > 68)  { num *= 1E-68,   pow -= 68;  } 
    else if (pow > 34)  { num *= 1E-34,   pow -= 34;  } 
    else if (pow > 17)  { num *= 1E-17,   pow -= 17;  }
    
    if (17 > pow) {
      num *= CACHE_POW[17 - pow - 1];
      pow += 17 - pow;
    }

    if (pow == 17) { goto generate; }
    else { goto convert; }

  } else if (pow < 0) {
    if (pow < -272)       { num *= 1E289,   pow += 289; }
    else if (pow < -136)  { num *= 1E153,   pow += 153; }
    else if (pow < -68)   { num *= 1E85,    pow += 85;  }
    else if (pow < -34)   { num *= 1E51,    pow += 51;  }
    else if (pow < -17)   { num *= 1E34,    pow += 34;  }
    else { num *= 1E17, pow += 17; }

    if (pow >= 0) {
      num *= CACHE_POW[17 - pow - 1];
      pow += 17 - pow;
      goto generate;
    }

    else { goto convert; }

  } else {

    num *= CACHE_POW[17 - 1];
    pow += 17;
  }

  generate: ;

  u_num = *(u64 *) &num;
  exp = (int) D_EXP(u_num) - 1023;

  u64 di = D_MAN(u_num);
  if (exp > 52) {
    di <<= (exp - 52);
  } else {
    di >>= (52 - exp);
  }

  return len + generate_digits(str, di, b_pow - pow);
}
