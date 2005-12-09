#include "stdlib.h"
#include "assert.h"
#include <monapi/string.h>
#include <monalibc/ctype.h>

void abort()
{
	exit(1);
}

char* getenv(const char* NAME)
{
	return NULL;
}

int putenv(char *string)
{
	return 0;
}

double strtod(const char *STR, char **TAIL)
{
	return atof(STR);
}

// from hige3d/notmine.cpp
double atof(const char *s)
{
	double a = 0.0;
	int e = 0;
	int c;
	int minus;

	c = *s;
	if (c == '-')
	{
		s++;
		minus = -1;
	}
	else
	{
		minus = 1;
	}

	while ((c = *s++) != '\0' && isdigit(c)) {
		a = a*10.0 + (c - '0');
	}
	if (c == '.') {
		while ((c = *s++) != '\0' && isdigit(c)) {
			a = a*10.0 + (c - '0');
			e = e-1;
		}
	}
	if (c == 'e' || c == 'E') {
		int sign = 1;
		int i = 0;
		c = *s++;
		if (c == '+')
			c = *s++;
		else if (c == '-') {
			c = *s++;
			sign = -1;
		}
		while (isdigit(c)) {
			i = i*10 + (c - '0');
			c = *s++;
		}
		e += i*sign;
	}
	while (e > 0) {
		a *= 10.0;
		e--;
	}
	while (e < 0) {
		a *= 0.1;
		e++;
	}
	return a * minus;
}

static double s_seed = 0.314159265359;

int rand()
{
	s_seed *= 17.0;
	s_seed -= (double)(int)s_seed;
	return (int)(s_seed * RAND_MAX);
}

void srand(unsigned seed)
{
	s_seed = ((double)(seed % RAND_MAX)) / ((double)RAND_MAX);
}


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
