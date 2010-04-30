#include <stdlib.h>

void *bsearch(const void *key, const void *base, size_t n, size_t size, int (*fnc)(const void *, const void *)){

	char *result;
	int cmp;
	size_t lo = 0, mid, hi = n - 1;

	do{
		mid = (lo + hi)/2;
		result = (char *)base + size*mid;
		cmp = fnc(key, result);
		if(cmp == 0){			 /* key == result */
			while((result > (char*)base) && !fnc(key, result - size)) result -= size; /* search lower result */
			return result;
		} else if(cmp < 0){ /* key < result */
			if(mid == 0)
				break; /* hi becomes -1, but it's unsigned, so while condition is not enough */
			hi = mid - 1;
		} else {	/* cmp > 0 : key > result */
			lo = mid + 1;
		}
	} while(lo <= hi);

	result = NULL;
	return result;
}
