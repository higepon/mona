#include <math.h>

double pow(double x, double y){

  long int n = y;
  double result = 1;

  if(x == 0){
    if(y <= 0) return 1; /* 0^y = 1 (y <= 0) */
    return 0;            /* 0^y = 0 (y > 0)  */
  }

  if(y == n){ /* y is integer */
    char sign = 0;
    if(n < 0){
      sign = 1;
      n = -n;
      x = 1/x;
    }
    while(1){
      if(n&1) result *= x; /* n is odd */
      n /= 2;
      if(n == 0) break;
      x *= x;
    }
    return result;
  }

  if(x < 0) return 0; /* error: result is imaginary */

  return exp(y*log(x));
}
