#include "stdlib.h"
#include "assert.h"

#undef exit
static void(*_p_exit)(int code) = NULL;

void _mona_set_exit(void(*_exit)(int code))
{
	_p_exit = _exit;
}

void _mona_exit(int code)
{
	if (_p_exit == NULL)
	{
		exit(code);
	}
	else
	{
		(*_p_exit)(code);
	}
}
