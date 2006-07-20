#include <math.h>
#include <limits.h>
#include <float.h>

double exp(double x){

  int ix, n = 1;
  double ax = fabs(x), a = 1.0;
  double tmp1, tmp2;
  double result = 1.0;

  /* exp(2.3) = exp(2)*exp(0.3) */
  /* exp(x) = 1 + x^1/1! + x^2/2! + x^3/3! ... + x^k/k! */
  if(ax < DBL_EPSILON) return 1;
  ix = (int)floor(ax); /* integer part */
  ax -= ix;       /* fraction part */

  do{
    tmp1 = result;
    a *= ax/n;
    result += a;
    n++;
  } while(result != tmp1);

  a = NAPIER;
  while(ix > 0){
    if(ix&1){ /* ix is odd */
      if(x >= 0 && a > DBL_MAX/result) return HUGE_VAL;
      if(x < 0 && a > DBL_MAX/result) return 0;
      result *= a;
    }
    ix /= 2;
    a = a*a;
  }

  if(x >= 0) return result;
  else return 1.0/result;
}
