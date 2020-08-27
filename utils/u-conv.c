#include "u-conv.h"

static u8 chr_buff[10] = {  '0', '1', '2', '3', 
                            '4', '5', '6', '7', 
                            '8', '9' };

u8 
Util_QuickUnsignedConvert(u8 *str, u64 val)
{

  if (!val) {
    str[0] = chr_buff[0];
    return 1;
  }

  int n = 0;
  if (val >= 1000000000000000000ULL) {
    n = 19;
  } else if (val >= 100000000000000000ULL) {
    n = 18;
  } else if (val >= 10000000000000000ULL) {
    n = 17;
  } else if (val >= 1000000000000000ULL) {
    n = 16;
  } else if (val >= 100000000000000ULL) {
    n = 15;
  } else if (val >= 10000000000000ULL) {
    n = 14;
  } else if (val >= 1000000000000ULL) {
    n = 13;
  } else if (val >= 100000000000ULL) {
    n = 12;
  } else if (val >= 10000000000ULL) {
    n = 11;
  } else if (val >= 1000000000ULL) {
    n = 10;
  } else if (val >= 100000000ULL) {
    n = 9;
  } else if (val >= 10000000ULL) {
    n = 8;
  } else if (val >= 1000000ULL) {
    n = 7;
  } else if (val >= 100000ULL) {
    n = 6;
  } else if (val >= 10000ULL) {
    n = 5;
  } else if (val >= 1000ULL) {
    n = 4;
  } else if (val >= 100ULL) {
    n = 3;
  } else if (val >= 10ULL) {
    n = 2;
  } else if (val >= 1ULL) {
    n = 1;
  }

  u8 m = n;
  while (n) {
    u64 dd = 0;
    switch (n) {
      case 19: dd = val / 1000000000000000000ULL; break;
      case 18: dd = val / 100000000000000000ULL; break;
      case 17: dd = val / 10000000000000000ULL; break;
      case 16: dd = val / 1000000000000000ULL; break;
      case 15: dd = val / 100000000000000ULL; break;
      case 14: dd = val / 10000000000000ULL; break;
      case 13: dd = val / 1000000000000ULL; break;
      case 12: dd = val / 100000000000ULL; break;
      case 11: dd = val / 10000000000ULL; break;
      case 10: dd = val / 1000000000ULL; break;
      case  9: dd = val / 100000000ULL; break;
      case  8: dd = val / 10000000ULL; break;
      case  7: dd = val / 1000000ULL; break;
      case  6: dd = val / 100000ULL; break;
      case  5: dd = val / 10000ULL; break;
      case  4: dd = val / 1000ULL; break;
      case  3: dd = val / 100ULL; break;
      case  2: dd = val / 10ULL; break;
      case  1: dd = val; break;
      default: break;
    }

    str[m - n--] = chr_buff[dd % 10];
  }

  return m;
}
