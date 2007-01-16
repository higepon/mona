#include <fenv.h>
#include <string.h>

int fesetenv(const fenv_t *f)
{
	fenv_t *tmp;

	memcpy(tmp, f, sizeof(fenv_t));

	if( tmp == FE_PC64_ENV )
	{
		asm volatile("finit");
	}
	else if( tmp == FE_PC53_ENV )
	{
		tmp->__status_uint16_t = 0x2f;
		asm volatile("fldenv %0" : : "m"(*tmp));
	}
	else if( tmp == FE_DFL_ENV )
	{
		asm volatile("finit");
	}
	else
	{
		asm volatile("fldenv %0" : : "m"(*tmp));
	}
	return 0;
}
