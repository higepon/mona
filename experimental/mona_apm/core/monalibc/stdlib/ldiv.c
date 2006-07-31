#include <stdlib.h>

ldiv_t ldiv(long int numer, long int denom){

	ldiv_t result;
	long int anumer = labs(numer);
	long int adenom = labs(denom);

	result.quot = anumer / adenom;
	result.rem	= anumer % adenom;

	if((numer < 0 && denom > 0) || (numer > 0 && denom < 0)) result.quot = -result.quot;
	if(numer < 0) result.rem = -result.rem;

	return result;
}
