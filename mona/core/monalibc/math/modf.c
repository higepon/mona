#include <math.h>

double modf(double x, double *buf)
{
    double a = fabs(x);
    int i = (int)x;

    double f = a - (double)i;

    *buf = x < 0 ? -i : i;

    return x < 0 ? -f : f;
}
