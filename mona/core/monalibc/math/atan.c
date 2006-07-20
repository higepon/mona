#include <math.h>

double atan(double x)
{
    double result;
    asm volatile("fld1; fpatan" : "=t"(result) : "0" (x) : "st(1)");
    return result;
}
