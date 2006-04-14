#include <monapi/syscall.h>
#include <monalibc/stdio.h>
#include <monalibc/setjmp.h>
#include <monalibc/assert.h>

int setjmp(jmp_buf env)
{
	return 0;
}

void longjmp(jmp_buf env, int val)
{
	assert(0);
}
