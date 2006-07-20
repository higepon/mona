#include <monalibc/stdlib.h>
#include <monalibc.h>

void qsort(void *base, size_t n, size_t size, int (*fnc)(const void*, const void*)){

	char *stdv = (char *)base; /* pointer to comparison standard value */
	char *tmp = stdv + size;
	char *right;
	size_t i, exchange = 0;

	if(n <= 1) return;

	if(n == 2){
		if(fnc(stdv, tmp) > 0) memswap(stdv, tmp, size);
		return;
	}

	right = stdv;
	for(i = 1; i < n; i++){
		if(fnc(tmp, stdv) > 0){
			exchange++;
			right += size;
			if(tmp != right) memswap(tmp, right, size);
		}
		tmp += size;
	}

	if(stdv != right) memswap(stdv, right, size);
	qsort(base, exchange, size, fnc);
	qsort(right + size, n - exchange - 1, size, fnc);

	return;
}
