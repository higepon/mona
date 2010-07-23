#include <math.h>
#include "ieee_754.h"

int signbit_d(double x)
{
       union __nc_ieee_754_double u;
       u.value = x;
       return u.n.sign;
}

int signbit_f(float x)
{
       union __nc_ieee_754_single u;
       u.value = x;
       return u.n.sign;
}
