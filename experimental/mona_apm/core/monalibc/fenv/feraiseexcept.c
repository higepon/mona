#include <fenv.h>

int feraiseexcept(int e)
{
	fexcept_t f;
	
	fegetexceptflag(&f, e);
	fesetexceptflag(&f, e);
	
	return 0;
}
