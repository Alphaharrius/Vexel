#include "d-conv.h"
#include <stdio.h>
/**
 * Copyright (c) 2019, 2020, Alphaharrius. All rights reserved.
 * 
 * d-conv.c
 * This file implements the QuickDoubleConvert algorithm for fast 
 * conversion of double value to printable string, 
 */

#define D_SGN(h) (u8) (h & (1ULL << 63))
#define D_EXP(h) (u16) ((h & (0x7FFULL << 52)) >> 52)
#define D_MAN(h) (u64) (h & (~0ULL >> 12) | (1ULL << 52))

#define LO_MSK (~0ULL >> 32)
#define HI_POS (1ULL << 52)
#define U64_TOP (1ULL << 63)

typedef struct { int exp; u64 man; } Double;

static Double POWER_10[] = {
  { .exp = 3, .man = 0x14000000000000 }, { .exp = 6, .man = 0x19000000000000 }, 
  { .exp = 9, .man = 0x1f400000000000 }, { .exp = 13, .man = 0x13880000000000 }, 
  { .exp = 16, .man = 0x186a0000000000 }, { .exp = 19, .man = 0x1e848000000000 }, 
  { .exp = 23, .man = 0x1312d000000000 }, { .exp = 26, .man = 0x17d78400000000 }, 
  { .exp = 29, .man = 0x1dcd6500000000 }, { .exp = 33, .man = 0x12a05f20000000 }, 
  { .exp = 36, .man = 0x174876e8000000 }, { .exp = 39, .man = 0x1d1a94a2000000 }, 
  { .exp = 43, .man = 0x12309ce5400000 }, { .exp = 46, .man = 0x16bcc41e900000 }, 
  { .exp = 49, .man = 0x1c6bf526340000 }, { .exp = 53, .man = 0x11c37937e08000 }, 
  { .exp = 56, .man = 0x16345785d8a000 }, { .exp = 59, .man = 0x1bc16d674ec800 }, 
  { .exp = 63, .man = 0x1158e460913d00 }, { .exp = 66, .man = 0x15af1d78b58c40 }, 
  { .exp = 69, .man = 0x1b1ae4d6e2ef50 }, { .exp = 73, .man = 0x10f0cf064dd592 }, 
  { .exp = 76, .man = 0x152d02c7e14af6 }, { .exp = 79, .man = 0x1a784379d99db4 }, 
  { .exp = 83, .man = 0x108b2a2c280290 }, { .exp = 86, .man = 0x14adf4b7320334 }, 
  { .exp = 89, .man = 0x19d971e4fe8401 }, { .exp = 93, .man = 0x1027e72f1f1281 }, 
  { .exp = 96, .man = 0x1431e0fae6d721 }, { .exp = 99, .man = 0x193e5939a08ce9 }, 
  { .exp = 102, .man = 0x1f8def8808b023 }, { .exp = 106, .man = 0x13b8b5b5056e16 }, 
  { .exp = 109, .man = 0x18a6e32246c99c }, { .exp = 112, .man = 0x1ed09bead87c03 }, 
  { .exp = 116, .man = 0x13426172c74d82 }, { .exp = 119, .man = 0x1812f9cf7920e2 }, 
  { .exp = 122, .man = 0x1e17b84357691a }, { .exp = 126, .man = 0x12ced32a16a1b0 }, 
  { .exp = 129, .man = 0x178287f49c4a1c }, { .exp = 132, .man = 0x1d6329f1c35ca3 }, 
  { .exp = 136, .man = 0x125dfa371a19e6 }, { .exp = 139, .man = 0x16f578c4e0a060 }, 
  { .exp = 142, .man = 0x1cb2d6f618c878 }, { .exp = 146, .man = 0x11efc659cf7d4b }, 
  { .exp = 149, .man = 0x166bb7f0435c9e }, { .exp = 152, .man = 0x1c06a5ec5433c6 }, 
  { .exp = 156, .man = 0x118427b3b4a05c }, { .exp = 159, .man = 0x15e531a0a1c873 }, 
  { .exp = 162, .man = 0x1b5e7e08ca3a90 }, { .exp = 166, .man = 0x111b0ec57e649a }, 
  { .exp = 169, .man = 0x1561d276ddfdc0 }, { .exp = 172, .man = 0x1aba4714957d30 }, 
  { .exp = 176, .man = 0x10b46c6cdd6e3e }, { .exp = 179, .man = 0x14e1878814c9ce }, 
  { .exp = 182, .man = 0x1a19e96a19fc42 }, { .exp = 186, .man = 0x105031e2503da9 }, 
  { .exp = 189, .man = 0x14643e5ae44d13 }, { .exp = 192, .man = 0x197d4df19d6058 }, 
  { .exp = 195, .man = 0x1fdca16e04b86e }, { .exp = 199, .man = 0x13e9e4e4c2f345 }, 
  { .exp = 202, .man = 0x18e45e1df3b016 }, { .exp = 205, .man = 0x1f1d75a5709c1c }, 
  { .exp = 209, .man = 0x13726987666192 }, { .exp = 212, .man = 0x184f03e93ff9f6 }
};

static Double INV_POWER_10[] = {
  { .exp = -4, .man = 0x1999999999999a }, { .exp = -7, .man = 0x147ae147ae147b }, 
  { .exp = -10, .man = 0x10624dd2f1a9fc }, { .exp = -14, .man = 0x1a36e2eb1c432d }, 
  { .exp = -17, .man = 0x14f8b588e368f1 }, { .exp = -20, .man = 0x10c6f7a0b5ed8e }, 
  { .exp = -24, .man = 0x1ad7f29abcaf4a }, { .exp = -27, .man = 0x15798ee2308c3b }, 
  { .exp = -30, .man = 0x112e0be826d696 }, { .exp = -34, .man = 0x1b7cdfd9d7bdbd }, 
  { .exp = -37, .man = 0x15fd7fe1796497 }, { .exp = -40, .man = 0x119799812dea12 }, 
  { .exp = -44, .man = 0x1c25c268497683 }, { .exp = -47, .man = 0x16849b86a12b9c }, 
  { .exp = -50, .man = 0x1203af9ee75616 }, { .exp = -54, .man = 0x1cd2b297d889bd }, 
  { .exp = -57, .man = 0x170ef54646d497 }, { .exp = -60, .man = 0x12725dd1d243ac }, 
  { .exp = -64, .man = 0x1d83c94fb6d2ad }, { .exp = -67, .man = 0x179ca10c924224 }, 
  { .exp = -70, .man = 0x12e3b40a0e9b50 }, { .exp = -74, .man = 0x1e392010175ee6 }, 
  { .exp = -77, .man = 0x182db34012b252 }, { .exp = -80, .man = 0x1357c299a88ea8 }, 
  { .exp = -84, .man = 0x1ef2d0f5da7dda }, { .exp = -87, .man = 0x18c240c4aecb15 }, 
  { .exp = -90, .man = 0x13ce9a36f23c11 }, { .exp = -94, .man = 0x1fb0f6be50601b }, 
  { .exp = -97, .man = 0x195a5efea6b349 }, { .exp = -100, .man = 0x14484bfeebc2a1 }, 
  { .exp = -103, .man = 0x1039d665896881 }, { .exp = -107, .man = 0x19f623d5a8a735 }, 
  { .exp = -110, .man = 0x14c4e977ba1f5e }, { .exp = -113, .man = 0x109d8792fb4c4b }, 
  { .exp = -117, .man = 0x1a95a5b7f87a12 }, { .exp = -120, .man = 0x154484932d2e75 }, 
  { .exp = -123, .man = 0x11039d428a8b91 }, { .exp = -127, .man = 0x1b38fb9daa78e8 }, 
  { .exp = -130, .man = 0x15c72fb1552d86 }, { .exp = -133, .man = 0x116c262777579e }, 
  { .exp = -137, .man = 0x1be03d0bf225ca }, { .exp = -140, .man = 0x164cfda3281e3b }, 
  { .exp = -143, .man = 0x11d7314f534b62 }, { .exp = -147, .man = 0x1c8b821885456a }, 
  { .exp = -150, .man = 0x16d601ad376abb }, { .exp = -153, .man = 0x1244ce242c5562 }, 
  { .exp = -157, .man = 0x1d3ae36d13bbd0 }, { .exp = -160, .man = 0x17624f8a762fda }, 
  { .exp = -163, .man = 0x12b50c6ec4f315 }, { .exp = -167, .man = 0x1dee7a4ad4b822 }, 
  { .exp = -170, .man = 0x17f1fb6f10934e }, { .exp = -173, .man = 0x1327fc58da0f72 }, 
  { .exp = -177, .man = 0x1ea6608e29b250 }, { .exp = -180, .man = 0x18851a0b548ea6 }, 
  { .exp = -183, .man = 0x139dae6f76d885 }, { .exp = -187, .man = 0x1f62b0b257c0d5 }, 
  { .exp = -190, .man = 0x191bc08eac9a44 }, { .exp = -193, .man = 0x141633a556e1d0 }, 
  { .exp = -196, .man = 0x1011c2eaabe7da }, { .exp = -200, .man = 0x19b604aaaca62a }, 
  { .exp = -203, .man = 0x14919d5556eb55 }, { .exp = -206, .man = 0x10747ddddf22aa }, 
  { .exp = -210, .man = 0x1a53fc9631d110 }, { .exp = -213, .man = 0x150ffd44f4a740 }
};

static inline Double
convert(double num)
{
  u64 u_val = *(u64 *) &num;

  return (Double) {
    .exp = D_EXP(u_val) - 1023,
    .man = D_MAN(u_val)
  };
}

static inline Double
multiply(Double *a, Double *b)
{
  u64 a_hi = a->man >> 32;
  u64 a_lo = a->man & LO_MSK;
  u64 b_hi = b->man >> 32;
  u64 b_lo = b->man & LO_MSK;

  u64 man = a_hi * b_hi;
  int man_ov = 0;
  if (man & (1ULL << 41)) {
    man_ov = 1;
  }

  // printf("man: %llx, %d\n", man, man_ov);

  u64 lo_lo = a_lo * b_lo;

  u64 tmp = a_hi * b_lo + a_lo * b_hi + (lo_lo >> 32);

  man += (tmp >> 32);

  return (Double) {
    .exp = a->exp + b->exp + man_ov,
    .man = man
  };
}

static inline int
calculate_power(int exp)
{
  Double d_log = (Double) { .exp = -2, .man = 0x134413509F79FF };
  Double d_exp = convert((double) exp);

  // printf("EXP( %d, %llx )\n", d_exp.exp, d_exp.man);

  Double d_pow = multiply(&d_exp, &d_log);

  // printf("POW( %d, %llx )\n", d_pow.exp, d_pow.man);

  u64 man = d_pow.man;
  while (!(man & U64_TOP)) { man <<= 1; }

  return (int) (man >> (63 - d_pow.exp)) + 1;
}

u8
Util_QuickDoubleConvert(u8 *str, double num)
{
  Double d_num = convert(num);
  Double d_pow;
  int pow;

  if (52 > d_num.exp) {
    pow = calculate_power(52 - d_num.exp);
    if (pow > 64) {
      d_pow = POWER_10[63];
      pow -= 64;
    } else {
      d_pow = POWER_10[pow - 1];
    }
  } else if (52 < d_num.exp) {
    pow = calculate_power(d_num.exp - 52);
    if (pow > 64) {
      d_pow = INV_POWER_10[63];
      pow = -pow + 64;
    } else {
      d_pow = INV_POWER_10[pow - 1];
    }
  }

  Double prod = multiply(&d_num, &d_pow);
  u64 man = prod.man;
  while (!(man & U64_TOP)) { man <<= 1; }

  u64 di = man >> (63 - prod.exp);

  printf("pow: %d, exp: %d, man: %lld\n", pow, prod.exp, man >> (63 - prod.exp));
}