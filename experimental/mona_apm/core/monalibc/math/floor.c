#include <math.h>

double floor(double x)
{
    if (x < 0)
    {
        if (modf(-x, &x) != 0) x++;
        return -x;
    }
    modf(x, &x);
    return x;
}
