#include "d-conv.h"
/**
 * d-conv.c
 * This file implements the QuickDoubleConvert algorithm, 
 * this algorithm separates the integer and decimal parts 
 * to perform digit generation separately, it is then passed 
 * to the QuickUnsignedConvert for string generation.
 * 
 * TODO: This algorithm is yet to be optimized.
 */

#define D_2U64(d) (*(u64 *) &d)
#define D_SIGN(h) (u8) ((h & (1ULL << 63)) >> 63)
#define D_EXPO(h) (u16) ((h & (0x7FFULL << 52)) >> 52)
#define D_MANT(h) (u64) ((h << 11) | (1ULL << 63))

static inline void
generate_lower_digits(u64 *lo_di, u64 d_pos)
{
  u64 d = 1ULL << d_pos;
  u64 rmd = 1;
  u8 i = 0;
  u64 pow = 100000000000000000U;
  u8 k_pr = 0;

  while (rmd) {

    rmd *= 10;

    u64 k = rmd >> d_pos;

    *lo_di += k * pow;

    pow /= 10;

    rmd %= d;

    i++;
  }
}

u8
Util_QuickDoubleConvert(u8 *str, double val)
{
  u8 *str_base = str;

  u64 u_val = D_2U64(val);

  if (D_SIGN(u_val)) {
    *str++ = '-';
  }

  u16 expo = D_EXPO(u_val);
  if (!(expo ^ (u16) 0x7FF)) {

    if (0x000FFFFFFFFFFFFFULL & u_val) {

      memcpy(str_base, "nan", 3);

      return 3;
    }

    else {

      memcpy(str, "inf", 3);
      str += 3;

      return (u8) (str - str_base);
    }
  }

  int f_pos = (int) expo - 1023;

  u64 mant = D_MANT(u_val);
  if (!(~(1ULL << 63) & u_val)) {

    memcpy(str, "0.0", 3);
    str += 3;

    return str - str_base;
  }

  u64 hi_di = 0;

  if (f_pos >= 0) {

    f_pos++;

    hi_di = (mant & ~(~0ULL >> f_pos)) >> (64 - f_pos);
    mant <<= f_pos;
  }

  else if (f_pos < 0) {

    mant >>= (-f_pos - 1);
  }

  str += Util_QuickUnsignedConvert(str, hi_di);
  *str++ = '.';

  u64 lo_di = 0;
  u8 off = 64;
  u64 msk = 1ULL;

  if (mant) {
    while (off--) {

      if (msk & mant && off < 63) {
        
        generate_lower_digits(&lo_di, off + 1);
      }

      msk <<= 1;
    }

    u8 h_pad;
    if (lo_di >= 100000000000000000U) {
      h_pad = 0;
    } else if (lo_di >= 10000000000000000U) {
      h_pad = 1;
    } else if (lo_di >= 1000000000000000U) {
      h_pad = 2;
    } else if (lo_di >= 100000000000000U) {
      h_pad = 3;
    } else if (lo_di >= 10000000000000U) {
      h_pad = 4;
    } else if (lo_di >= 1000000000000U) {
      h_pad = 5;
    } else if (lo_di >= 100000000000U) {
      h_pad = 6;
    } else if (lo_di >= 10000000000U) {
      h_pad = 7;
    } else if (lo_di >= 1000000000U) {
      h_pad = 8;
    } else if (lo_di >= 100000000U) {
      h_pad = 9;
    } else if (lo_di >= 10000000U) {
      h_pad = 10;
    } else if (lo_di >= 1000000U) {
      h_pad = 11;
    } else if (lo_di >= 100000U) {
      h_pad = 12;
    } else if (lo_di >= 10000U) {
      h_pad = 13;
    } else if (lo_di >= 1000U) {
      h_pad = 14;
    } else if (lo_di >= 100U) {
      h_pad = 15;
    } else if (lo_di >= 10U) {
      h_pad = 16;
    } else if (lo_di >= 1U) {
      h_pad = 17;
    }

    memset(str, '0', (size_t) h_pad);
    str += h_pad;

    str += Util_QuickUnsignedConvert(str, lo_di);
  } else {

    *str++ = '0';
  }

  return (u8) (str - str_base);
}
