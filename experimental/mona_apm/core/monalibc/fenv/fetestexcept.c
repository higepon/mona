#include <fenv.h>

int fetestexcept(int e)
{
	fexcept_t f;
	
	fegetexceptflag(&f, e);
	return (int)f;
}
