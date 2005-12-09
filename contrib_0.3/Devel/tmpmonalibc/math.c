typedef union
{
	double d;
	struct
	{
		long low;
		long hi;
	};
	struct
	{
		long low:32;
		int hi:20;
		signed int exponent:11;
		int sign:1;
	};
} doublebit;

double fabs(double x)
{
	return ((doublebit)x).hi & 0x7fffffffLL;
}

// Damepo implementation.
double frexp(double x, int *exp)
{
	double n = ((doublebit)x).low + (double)((doublebit)x).hi*(double)0xffffffff;

	*exp = ((doublebit)x).exponent;

	return (double)n;
}

double fmod(double x, double y)
{
	double n = (double)(int)(x / y);
	return x - n * y;
}

// test code
#if 0
#include <stdio.h>

#define tBIT(x) printf("%f = %e = (-1)^%d*%f*2^%d\n", x, x,		\
			((doublebit)x).sign,				\
			((doublebit)x).low				\
			+(double)((doublebit)x).hi*(double)0xffffffff,	\
			((doublebit)x).exponent);

#define tFABS(x) printf("fabs(%e) is %e.\n", x, fabs(x))
#define tFREXP(x) do{							\
			int e;						\
			double r;					\
			r = frexp(x, &e);				\
			printf("frexp(%f) is %f*2^%d.\n", x, r, e);	\
			}while(0);

int main()
{
	doublebit t;
	t.sign = 0;
	tBIT(1.0);
	tFABS(-1.0);
	tFABS(-0.0);
	tFABS(-1.0);
	tFABS(-0.0);
	tFABS(-135246245746.0346431);
	tFABS((double)-0xffffff);

	tFREXP(-1.0);
	tFREXP(-0.0);
	tFREXP(-1.0);
	tFREXP(-0.0);
	tFREXP(-135246245746.0346431);
	tFREXP((double)-0xfffffffffffffLL*0.000001);
	tFREXP(2560.0);

	return 0;
}

#endif
