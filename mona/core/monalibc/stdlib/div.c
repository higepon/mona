#include <stdlib.h>

div_t div(int numer, int denom){

	div_t result;
	ldiv_t tmp = ldiv(numer, denom);

	result.quot = tmp.quot;
	result.rem	= tmp.rem;

	return result;
}
