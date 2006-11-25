#ifndef _VARGS_H_
#define _VARGS_H_

#include <stdarg.h>
#include "format.h"

#define TYPEID_CHAR	1
#define TYPEID_UCHAR	2
#define TYPEID_SHORT	3
#define TYPEID_USHORT	4
#define TYPEID_INT	5
#define TYPEID_UINT	6
#define TYPEID_LONG	7
#define TYPEID_ULONG	8
#define TYPEID_LLONG	9
#define TYPEID_ULLONG	10
#define TYPEID_POINTER	11
#define TYPEID_FLOAT	12
#define TYPEID_DOUBLE	13
#define TYPEID_LDOUBLE	14
#define TYPEID_SIZE_T	15
#define TYPEID_SSIZE_T	16
#define TYPEID_INTMAX	17
#define TYPEID_UINTMAX	18
#define TYPEID_PTRDIFF	19

typedef union _arg
{
	signed char	sc;
	unsigned char	uc;
	signed short	sh;
	unsigned short	uh;
	signed int	si;
	unsigned int	ui;
	signed long	sl;
	unsigned long	ul;
	signed long long   sll;
	unsigned long long ull;
	double		d;
	float		f;
	void *p;
} Arg;

typedef struct _vargs
{
	int length;
	Arg *p;
} Vargs;

int init_vargs(Vargs **vargsp);
int free_vargs(Vargs *vargs);

int __nc_vargs_0(int *p, Format *fmt); // fmt to tid array.
int __nc_vargs_1(Vargs *v, Format *f, va_list ap); // read args.

#endif
