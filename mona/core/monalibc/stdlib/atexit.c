#include <stdlib.h>

static void (*atexit_table[ATEXIT_MAX])(void);

__attribute__((constructor)) void __nc_atexit_init(void)
{
	int i;
	for(i = 0 ; i < ATEXIT_MAX ; i++ )
		atexit_table[i] = NULL;
}

__attribute__((destructor)) void __nc_atexit_caller()
{
	int i;
	for( i = 0 ; i < ATEXIT_MAX ; i++ ) if( atexit_table[i] == NULL ) break;
	_logprintf("monalibc: __nc_atexit_caller: %d\n", i);
	i--;
	for(; i > -1 ; i-- ) atexit_table[i]();
}

/*
	__nc_atexit can register ATEXIT_MAX functions.
	__nc_atexit_caller calls functions in reverse order.
*/
int __nc_atexit(void (*func)(void))
{
	int i;
	for(i = 0 ; i <= ATEXIT_MAX ; i++ ) if( atexit_table[i] == NULL ) break;
	if( i == ATEXIT_MAX ) return -1;
	atexit_table[i] = func;
	return 0;
}
