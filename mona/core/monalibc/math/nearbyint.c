#include <math.h>

double nearbyint(double x)
{
	double r;
	asm volatile("frndint" : "=t"(r) : "0"(x));
	return r;
}
