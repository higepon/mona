#include <math.h>

double tan(double angle)
{
    double result;
    asm volatile ("fptan" : "=t"(result) : "0"(angle));
    return result;
}
