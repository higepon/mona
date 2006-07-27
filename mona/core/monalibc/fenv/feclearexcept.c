#include <fenv.h>

int feclearexcept(int e)
{
	fenv_t ft;
	
	fegetenv(&ft);
	ft.__status_word &= ~e;
	fesetenv(&ft);
	
	return 0;
}
