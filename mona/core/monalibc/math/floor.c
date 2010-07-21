#include <math.h>

double floor(double x)
{
    if (isnan(x) || isinf(x) || fpclassify(x) == FP_ZERO) {
        return x;
    }
    if (x < 0)
    {
        if (modf(-x, &x) != 0) x++;
        return -x;
    }
    modf(x, &x);
    return x;
}
