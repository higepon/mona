#include <math.h>
#include <stdlib.h>
#include "ieee_754.h"

union __double_int
{
  double v;
  uint64_t w;
};

double __nand()
{
  static union __double_int t;
  static int i = 0;
  if( !i )
  {
    t.w = 0x7ff8000000000000LL;
    i = 1;
  }
  return t.v;
}
