#include <math.h>

double log(double x){

  int n = 1;
  double tmp1, tmp2, result = 0;

  if(x <= 0) return 0;
  while(x >= 2){ /* to x<2 */ /* ln(a)=ln(a/2)+ln(2) */
    x /= 2;
    result += LN2;
  }
  while(x < 1){  /* to x>=1 */ /* ln(a)= ln(2a)-ln(2) */
    x *= 2;
    result -= LN2;
  }
  /* 1<=x<2(?): ln(x) = sigma (2/(2k+1))*(((x-1)/(x+1))^(2k+1)) */
  x = (x-1)/(x+1);
  tmp1 = 2*x;
  x = x*x;
  do{
    tmp2 = result;
    result += tmp1/n;
    tmp1 *= x;
    n += 2;
  } while(result != tmp2);

  return result;
}
