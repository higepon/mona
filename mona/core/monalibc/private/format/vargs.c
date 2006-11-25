#include <stdlib.h>
#include "format.h"
#include "vargs.h"

int init_vargs(Vargs **vargsp)
{
	*vargsp = malloc(sizeof(Vargs));
	if( *vargsp == NULL )
	{
		return 1;
	}

	memset(*vargsp, 0, sizeof(Vargs));

	return 0;
}

int free_vargs(Vargs *vargs)
{
	int i;

	free(vargs->p);
	free(vargs);
	return 0;
}

int read_vargs_by_format(Vargs *vargs, Format *format, va_list ap)
{
}

int __nc_vargs_0(int *p, Format *f)
{
	int pi;
	int i;
	int bt, m, a;

	pi = 0;
	for( i = 0 ; i < f->length ; i++ )
	{
		if( f->ins[i].flag != 2 ) continue;
		bt = f->ins[i].u.cs->conversion;
		m = f->ins[i].u.cs->length_modifier;
//	printf("debug: %s: bt = %d\n", __func__, bt);
		if( f->ins[i].u.cs->field_width_arg_index == pi )
		{
			p[pi++] = TYPEID_INT;
		}
		if( f->ins[i].u.cs->field_width_arg_index == NEXT_ARG_INDEX )
		{
			p[pi++] = TYPEID_INT;
		}
		if( f->ins[i].u.cs->precision_arg_index == pi &&
			f->ins[i].u.cs->precision_arg_index == NEXT_ARG_INDEX)
		{
		}
		switch(bt)
		{
			case CS_d:
			case CS_i:
				switch(m)
				{
					case LMOD_CHAR:
						a = TYPEID_CHAR;
						break;
					case LMOD_SHORT:
						a = TYPEID_SHORT;
						break;
					case LMOD_LONG:
						a = TYPEID_LONG;
						break;
					case LMOD_LONGLONG:
						a = TYPEID_LLONG;
						break;
					case LMOD_INTMAX:
						a = TYPEID_INTMAX;
						break;
					case LMOD_SIZE_T:
						a = TYPEID_SIZE_T;
						break;
					case LMOD_PTRDIFF_T:
						a = TYPEID_PTRDIFF;
						break;
					default: break;
				}
			case CS_o:
			case CS_u:
			case CS_x:
			case CS_X:
				switch(m)
				{
					case LMOD_CHAR:
						a = TYPEID_UCHAR;
						break;
					case LMOD_SHORT:
						a = TYPEID_USHORT;
						break;
					case LMOD_LONG:
						a = TYPEID_ULONG;
						break;
					case LMOD_LONGLONG:
						a = TYPEID_ULLONG;
						break;
					case LMOD_INTMAX:
						a = TYPEID_UINTMAX;
						break;
					case LMOD_SIZE_T:
						a = TYPEID_SIZE_T;
						break;
					case LMOD_PTRDIFF_T:
						a = TYPEID_PTRDIFF;
						break;
					default: break;
				}
			case CS_p:
			case CS_n:
			case CS_s:
				a = TYPEID_POINTER;
				break;
			case CS_c:
				a = TYPEID_INT;
				break;
			case CS_e:
			case CS_E:
			case CS_f:
			case CS_F:
			case CS_a:
			case CS_A:
				a = TYPEID_DOUBLE;
				break;
			case CS_ESCAPE:
				a = 0;
				break;
			default: break;
		}
		p[pi++] = a;
	}
	return 0;
}

int __nc_vargs_1(Vargs *v, Format *f, va_list ap)
{
	int length;
	int i;
	int *types;
	Arg *p;

	length = count_format(f, 2);
//printf("%s: length = %d\n", __func__, length);
	types = malloc(length*sizeof(int));
	if( types == NULL )
	{
		return -1;
	}
	if( __nc_vargs_0(types, f) != 0 )
	{
		free(types);
		return -1;
	}

	p = malloc(length * sizeof(Arg));
	if( p == NULL )
	{
		free(types);
		return -1;
	}

//printf("\n%s: read\n", __func__);
	for( i = 0 ; i < length ; i++ )
	{
	//	printf("debug: %s: type = %d\n", __func__, types[i]);
		switch(types[i])
		{
			case TYPEID_INT:
				p[i].si = va_arg(ap, int); break;
			case TYPEID_UINT:
				p[i].ui = va_arg(ap, unsigned int); break;
			case TYPEID_LONG:
				p[i].sl = va_arg(ap, long); break;
			case TYPEID_ULONG:
				p[i].ul = va_arg(ap, unsigned long); break;
			case TYPEID_LLONG:
				p[i].sll = va_arg(ap, long long); break;
			case TYPEID_ULLONG:
				p[i].ull = va_arg(ap,unsigned long long);break;
			case TYPEID_POINTER:
				p[i].p = va_arg(ap, void*); break;
			case TYPEID_DOUBLE:
				p[i].d = va_arg(ap, double); break;
			case 0: break;
			default: free(p); free(types); return 1;
		}
	}

	v->p = p;

	free(types);

	return 0;
}
