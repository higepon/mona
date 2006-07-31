#include <fenv.h>

int feupdateenv(const fenv_t *f)
{
	unsigned short sw;
	fenv_t *ft;
	fexcept_t fe;
	
	fegetexceptflag(&fe, FE_ALL_EXCEPT);    
	fesetenv(f);
	fe |= f->__status_word;
	fesetexceptflag(&fe, FE_ALL_EXCEPT);
	
	return 0;
}

