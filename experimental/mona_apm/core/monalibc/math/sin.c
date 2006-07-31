#include <math.h>

double sin(double angle)
{
    double result;
    asm volatile ("fsin" : "=t"(result) : "0"(angle));
    return result;
}
