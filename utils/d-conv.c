#include "d-conv.h"
#include <stdio.h>
/**
 * Copyright (c) 2019, 2020, Alphaharrius. All rights reserved.
 * 
 * d-conv.c
 * This file implements the QuickDoubleConvert algorithm, 
 * this algorithm separates the integer and decimal parts 
 * to perform digit generation separately, it is then passed 
 * to the QuickUnsignedConvert for string generation.
 * 
 * Quick Double Convert Algorithm
 * 
 *  1.  If equals special values ( inf, -inf, nan, 0.0, -0.0 ), return.
 * 
 *  2.  Extract the integer and decimal part from the double qword separately.
 *        3.625 -> 0x400D000000000000
 *        integer => 11
 *        decimal => 101
 * 
 *  3.  Convert the integer part with QuickUnsignedConvert and push to string.
 * 
 *  4.  We will treat the decimal places of the double as an u64 integer, 
 *      the value will be incremented per bit position of the mantissa.
 *        mant[1] = 1 => 500000000000000000ULL
 *        mant[2] = 0 => 0UUL
 *        mant[3] = 1 => 125000000000000000ULL
 *        result      => 625000000000000000ULL
 *      The u64 value of each corresponding bit position is computed using 
 *      a shift and mod approach, for detail place see code comments.
 * 
 *  5.  The computation of decimal places induces significant overhead during 
 *      the digit generation of multiple bit positions, which occurs very frequently as 
 *      most double values are not of perfect values like ( 0.5, 0.25, 0.625 ), 
 *      but represented with a lot of mantissa bits.
 *      Therefore, the algorithm replaced the need for the digit generation for bit position 
 *      greater or equal to 20 ( double value have at max 52 bits for mantissa ), with 
 *      precalculated values, the amount of precomputated data is in favor to reduce memory 
 *      footprint of the method.
 *      The power 10 values for digit generations are also being precalculated to 
 *      remove the need for expensive big number divisions.
 */

/**
 * The precalculated values for the digit generation result for bit position 
 * greater or equal to 20, stored in u64 hexadecimal format.
 */
static u64 SIG_PRECMP[] = { 0xDE0B6B3A76,   0x6F05B59D3B,   0x3782DACE9E,   0x1BC16D674F, 
                            0xDE0B6B3A8,    0x6F05B59D4,    0x3782DACEA,    0x1BC16D675, 
                            0xDE0B6B3B,     0x6F05B59E,     0x3782DACF,     0x1BC16D68, 
                            0xDE0B6B4,      0x6F05B5A,      0x3782DAD,      0x1BC16D7, 
                            0xDE0B6C,       0x6F05B6,       0x3782DB,       0x1BC16E, 
                            0xDE0B7,        0x6F05C,        0x3782E,        0x1BC17, 
                            0xDE0C,         0x6F06,         0x3783,         0x1BC2, 
                            0xDE1,          0x6F1,          0x379,          0x1BD, 
                            0xDF };

/**
 * The precalculated values for the power 10 big integers, starting from 1e17
 */
static u64 POW10_PRECMP[] = { 0x16345785D8A0000,  0x2386F26FC10000,   0x38D7EA4C68000, 
                              0x5AF3107A4000,     0x9184E72A000,      0xE8D4A51000, 
                              0x174876E800,       0x2540BE400,        0x3B9ACA00, 
                              0x5F5E100,          0x989680,           0xF4240, 
                              0x186A0,            0x2710,             0x3E8, 
                              0x64,               0xA,                0x1 };

static inline void 
/**
 * The digit generation algorithm 
 * involves a shift & mod approach.
 */
generate_lower_digits(u64 *lo_di, u64 d_pos)
{
  u64 d = 1ULL << d_pos;
  u64 rmd = 1;
  u8 i = 0;
  u8 k_pr = 0;

  /**
   * This process is similar to a 
   * hand written division approach 
   * but by the power of 2.
   * Generation for digit beyond 18 
   * is trivial and prone to inaccurate 
   * conversion.
   */
  while (rmd != 0 && i < 18) {

    /**
     * Increase the dividen a power of 10 
     * as 1 in 1 / 2^pos division is insufficent 
     * to carry out the division.
     */
    rmd *= 10;

    /**
     * Carry out the division by 2^pos, 
     * replaced division by bit shifting 
     * to improve performance.
     * The value k is the generated digit 
     * from the current iteration.
     */
    u64 k = rmd >> d_pos;

    if (k) {

      *lo_di += k * POW10_PRECMP[i];
    }

    /**
     * Calculate the remainder of the 1 / 2^pos division, 
     * this loop will continue until the remainder is 0.
     */
    rmd %= d;

    i++;
  }
}

#define D_2U64(d) (*(u64 *) &d) /** Convert double to u64 */
#define D_SIGN(h) (u8) ((h & (1ULL << 63)) >> 63) /** Extract sign */
#define D_EXPO(h) (u16) ((h & (0x7FFULL << 52)) >> 52) /** Extract exponent */
#define D_MANT(h) (u64) ((h << 11) | (1ULL << 63)) /** Extract mantissa */

u8
Util_QuickDoubleConvert(u8 *str, double val)
{
  u8 *str_base = str;

  u64 u_val = D_2U64(val);

  if (D_SIGN(u_val)) {
    *str++ = '-';
  }

  u16 expo = D_EXPO(u_val);
  /**
   * IEEE-754 defines a value to be special 
   * if its exponent equals 0x7FF.
   */
  if (expo == 0x7FF) {

    /**
     * IEEE-754 defines a special value to 
     * be "nan" (Not a number) if its mantissa 
     * is not zero.
     */
    if ((~0ULL >> 12) & u_val) {
      
      /**
       * The "nan" value does not have a sign.
       */
      memcpy(str_base, "nan", 3);

      return 3;
    }
    /**
     * IEEE-754 defines a special value to 
     * be "inf" (Not a number) if its mantissa 
     * is zero.
     */
    else {

      /**
       * The "inf" values does carries a sign.
       */
      memcpy(str, "inf", 3);
      str += 3;

      return (u8) (str - str_base);
    }
  }

  /**
   * The floating point offset of a IEEE-754 
   * double is calculated by subtracting the 
   * exponent by 1023.
   */
  int f_pos = (int) expo - 1023;

  u64 mant = D_MANT(u_val);
  /**
   * The value is zero if it have zero valued mantissa.
   */
  if (!(~(1ULL << 63) & u_val)) {

    /**
     * The zero value does carries a sign.
     */
    memcpy(str, "0.0", 3);
    str += 3;

    return str - str_base;
  }

  u64 hi_di = 0;

  /**
   * If floating point offset is positive, 
   * this value carries a integer part.
   */
  if (f_pos >= 0) {

    f_pos++;

    /**
     * Extract integer part by shifting off the decimal places.
     */
    hi_di = (mant & ~(~0ULL >> f_pos)) >> (64 - f_pos);
    /**
     * Extract the decimal part by shifting off the integer part.
     */
    mant <<= f_pos;
  }

  /**
   * If floating point offset is negative, 
   * this value does not carry a integer part.
   */
  else if (f_pos < 0) {

    /**
     * Shift the mantissa to its correct position 
     * after the floating point offset.
     */
    mant >>= (-f_pos - 1);
  }

  /**
   * Generate the integer part with 20 places.
   */
  if (hi_di) {
    str += Util_QuickUnsignedConvert(str, hi_di, 20, 0);
  } 
  else {
    *str++ = '0';
  }
  /**
   * Place in the floating point.
   */
  *str++ = '.';

  u64 lo_di = 0;
  u8 off = 52; /** Scan from last bit pos */
  u64 msk = 1ULL << 12; /** Mask to check if bit is set */
  u8 d_pos;

  if (mant) {

    while (off--) {
        if (msk & mant) {

            d_pos = off + 1;

            if (d_pos >= 20) {

                /**
                 * Apply the corresponding precomputed 
                 * significant digits of the bit position 
                 * to reduce overhead.
                 */
                lo_di += SIG_PRECMP[d_pos - 20];
            }
            else {

                generate_lower_digits(&lo_di, d_pos);
            }
        }

        msk <<= 1;
    }

    /**
     * Generate the decimal places with at max 18 significant 
     * figures and zero padding in front.
     */
    str += Util_QuickUnsignedConvert(str, lo_di, 18, 1);

  } else {

    /**
     * Write 0 to the decimal places if mantissa is 0.
     */
    *str++ = '0';
  }

  return (u8) (str - str_base);
}
