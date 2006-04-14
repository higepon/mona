#include "stdio.h"
#include "setjmp.h"
#include "assert.h"

int setjmp(jmp_buf env)
{
	return 0;
}

void longjmp(jmp_buf env, int val)
{
	assert(0);
}
