#include <math.h>

double atan2(double y, double x)
{
    double result;
    asm volatile("fpatan" : "=t" (result) : "0" (x), "u" (y) : "st(1)");
    return result;
}
