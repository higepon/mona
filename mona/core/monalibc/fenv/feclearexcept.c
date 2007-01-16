#include <fenv.h>

int feclearexcept(int e)
{
	fenv_t ft;
	
	fegetenv(&ft);
	ft.__status_uint16_t &= ~e;
	fesetenv(&ft);
	
	return 0;
}
