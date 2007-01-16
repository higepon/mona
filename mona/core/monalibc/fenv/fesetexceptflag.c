#include <fenv.h>

int fesetexceptflag(const fexcept_t *f, int e)
{
	fenv_t ft;
	unsigned short sw;
	
	fegetenv(&ft);
	sw = *f;
	sw &= e;
	ft.__status_uint16_t = sw;
	fesetenv(&ft);
	
	return 0;
}
