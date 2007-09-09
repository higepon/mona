#include <monalibc/stdlib.h>
#include <monalibc.h>

#if 1
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
		if(fnc(tmp, stdv) < 0){
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
#else

// from uClib ssort
// http://www.uclibc.org/cgi-bin/viewcvs.cgi/trunk/uClibc/libc/stdlib/stdlib.c?rev=14605&view=markup
void qsort (void  *base,
            size_t nel,
            size_t width,
            int (*comp)(const void *, const void *))
{
      size_t wnel, gap, wgap, i, j, k;
      char *a, *b, tmp;

      wnel = width * nel;
      for (gap = 0; ++gap < nel;)
            gap *= 3;
      while ( gap /= 3 )
      {
            wgap = width * gap;
            for (i = wgap; i < wnel; i += width)
            {
                  for (j = i - wgap; ;j -= wgap)
                  {
                        a = j + (char *)base;
                        b = a + wgap;
                        if ( (*comp)(a, b) <= 0 )
                              break;
                        k = width;
                        do
                        {
                              tmp = *a;
                              *a++ = *b;
                              *b++ = tmp;
                        } while ( --k );
                        if (j < wgap)
                              break;
                  }
            }
      }
}
#endif
