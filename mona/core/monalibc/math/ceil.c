#include <math.h>

double ceil(double x){

  return -floor(-x); /* -[-x] */
}
