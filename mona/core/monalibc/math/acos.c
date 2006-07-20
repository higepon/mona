#include <math.h>

/* acos = atan (sqrt(1 - x^2) / x) */
double acos(double x)
{
  double result;

  asm volatile("fld    %%st    \n"
               "fmul   %%st(0) \n"
               "fld1           \n"
               "fsubp          \n"
               "fsqrt          \n"
               "fxch   %%st(1) \n"
               "fpatan         \n"
               : "=t" (result) : "0" (x) : "st(1)");

  return result;
}
