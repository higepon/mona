#include "vargs.h"
#include "format.h"
#include "digitconv.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>

int __cs_make_a_fix_traits(struct fix_traits *tr, ConvSpec *cs, Arg* arg)
{
	if( cs->flag & FLAG_LEFT ) tr->left_align = true;
	if( cs->flag & FLAG_SIGNED ) tr->add_sign_flag = true;
	if( cs->flag & FLAG_SPACE ) tr->space = true;
	if( cs->flag & FLAG_SUB ) tr->another = true;
	if( cs->flag & FLAG_ZERO ) tr->zero_padding = true;
tr->field_width = cs->field_width;
	tr->prec = cs->precision;
	return 0;
}


int __cs_d_(char *s, size_t lim, size_t index, ConvSpec *cs, Arg* arg, int flag)
{
	char *sp;
	int slen, n;
	size_t size;
	struct fix_traits tr;
	memset(&tr, 0, sizeof(struct fix_traits));
	__cs_make_a_fix_traits(&tr, cs, arg);
	switch( cs->length_modifier )
	{
		case LMOD_CHAR: n = (int)arg->sc; break;
		case LMOD_SHORT:n = (int)arg->sh; break;
		case LMOD_LONG: n = (int)arg->sl; break;
		case LMOD_LONGLONG:
		fprintf(stderr, "ll modifier isn't supported."); assert(0);
		default: n = arg->si; break;
	}
	size = __nc_getStringLength((unsigned int)n, 10, &tr)+1;
	sp = (char*)malloc(size);
	slen = __nc_digitconv(sp, size, n, 10, &tr);
	if( slen >= lim ) slen = lim-1;
	if( slen != 0 ) memcpy(s+index, sp, slen);
	free(sp);
	return slen;
}
int __cs_o_(char *s, size_t lim, size_t index, ConvSpec *cs, Arg* arg, int flag)
{
	char *sp;
	int slen;
	unsigned int n;
	size_t size;
	struct fix_traits tr;
	memset(&tr, 0, sizeof(struct fix_traits));
	__cs_make_a_fix_traits(&tr, cs, arg);
	switch( cs->length_modifier )
	{
		case LMOD_CHAR: n = (int)arg->uc; break;
		case LMOD_SHORT:n = (int)arg->uh; break;
		case LMOD_LONG: n = (int)arg->ul; break;
		case LMOD_LONGLONG:
		fprintf(stderr, "ll modifier isn't supported."); assert(0);
		default: n = arg->ui; break;
	}
	size = __nc_getStringLength(n, 8, &tr)+1;
	sp = (char*)malloc(size);
	__nc_conv_digits(sp, size, n, 8, &tr);
	__nc_fix_format_string(sp, size, &tr);
	slen = strlen(sp);
	if( slen >= lim ) slen = lim-1;
	if( slen != 0 ) memcpy(s+index, sp, slen);
	free(sp);
	return slen;
}
int __cs_u_(char *s, size_t lim, size_t index, ConvSpec *cs, Arg* arg, int flag)
{
	char *sp;
	int slen;
	unsigned int n;
	size_t size;
	struct fix_traits tr;
	memset(&tr, 0, sizeof(struct fix_traits));
	__cs_make_a_fix_traits(&tr, cs, arg);
	switch( cs->length_modifier )
	{
		case LMOD_CHAR: n = (int)arg->uc; break;
		case LMOD_SHORT:n = (int)arg->uh; break;
		case LMOD_LONG: n = (int)arg->ul; break;
		case LMOD_LONGLONG:
		fprintf(stderr, "ll modifier isn't supported."); assert(0);
		default: n = arg->ui; break;
	}
	size = __nc_getStringLength(n, 10, &tr)+1;
	sp = (char*)malloc(size);
	__nc_conv_digits(sp, size, n, 10, &tr);
	__nc_fix_format_string(sp, size, &tr);
	slen = strlen(sp);
	if( slen >= lim ) slen = lim-1;
	if( slen != 0 ) memcpy(s+index, sp, slen);
	free(sp);
	return slen;
}
int __cs_x_(char *s, size_t lim, size_t index, ConvSpec *cs, Arg* arg, int flag)
{
	char *sp;
	int slen;
	unsigned int n;
	size_t size;
	struct fix_traits tr;
	memset(&tr, 0, sizeof(struct fix_traits));
	__cs_make_a_fix_traits(&tr, cs, arg);
	switch( cs->length_modifier )
	{
		case LMOD_CHAR: n = (int)arg->uc; break;
		case LMOD_SHORT:n = (int)arg->uh; break;
		case LMOD_LONG: n = (int)arg->ul; break;
		case LMOD_LONGLONG:
		fprintf(stderr, "ll modifier isn't supported."); assert(0);
		default: n = arg->ui; break;
	}
//	n = arg->ui;
	size = __nc_getStringLength(n, 16, &tr)+1;
	sp = (char*)malloc(size);
	__nc_conv_digits(sp, size, n, 16, &tr);
	__nc_fix_format_string(sp, size, &tr);
	slen = strlen(sp);
	if( slen >= lim ) slen = lim-1;
	if( slen != 0 ) memcpy(s+index, sp, slen);
	free(sp);
	return slen;
}
int __cs_X_(char *s, size_t lim, size_t index, ConvSpec *cs, Arg* arg, int flag)
{
	int result;
	char *p;
	int i;
	result = __cs_x_(s, lim, index, cs, arg, flag);
	p = s+index;
	for( i = 0 ; i < result ; i++ )
	{
		if( islower(*p) ) *p = toupper(*p);
		p++;
	}
	return result;
}
int __cs_f_(char *s, size_t lim, size_t index, ConvSpec *cs, Arg* arg, int flag)
{
}
int __cs_F_(char *s, size_t lim, size_t index, ConvSpec *cs, Arg* arg, int flag)
{
}
int __cs_e_(char *s, size_t lim, size_t index, ConvSpec *cs, Arg* arg, int flag)
{
}
int __cs_E_(char *s, size_t lim, size_t index, ConvSpec *cs, Arg* arg, int flag)
{
}
int __cs_g_(char *s, size_t lim, size_t index, ConvSpec *cs, Arg* arg, int flag)
{
}
int __cs_G_(char *s, size_t lim, size_t index, ConvSpec *cs, Arg* arg, int flag)
{
}
int __cs_a_(char *s, size_t lim, size_t index, ConvSpec *cs, Arg* arg, int flag)
{
}
int __cs_A_(char *s, size_t lim, size_t index, ConvSpec *cs, Arg* arg, int flag)
{
}
int __cs_c_(char *s, size_t lim, size_t index, ConvSpec *cs, Arg* arg, int flag)
{
	int c = arg->si;
	if( index+1 >= lim ) return 0;
	s[index] = (char)c;
	return 1;
}
int __cs_s_(char *s, size_t lim, size_t index, ConvSpec *cs, Arg* arg, int flag)
{
	char *sp = arg->p;
	int slen;

	slen = strlen(sp);
	if( slen >= lim ) slen = lim-1;
	if( slen != 0 )
	{
		memcpy(s+index, sp, slen);
	}
	return slen;
}
int __cs_p_(char *s, size_t lim, size_t index, ConvSpec *cs, Arg* arg, int flag)
{
	char *sp;
	int slen;
	unsigned int n;
	size_t size;
	struct fix_traits tr;
	memset(&tr, 0, sizeof(struct fix_traits));
	__cs_make_a_fix_traits(&tr, cs, arg);
	tr.another = true; tr.zero_padding = true; tr.prec = 8;
	n = arg->ui;
	size = __nc_getStringLength(n, 16, &tr)+1;
	sp = (char*)malloc(size);
	__nc_conv_digits(sp, size, n, 16, &tr);
	__nc_fix_format_string(sp, size, &tr);
	slen = strlen(sp);
	if( slen >= lim ) slen = lim-1;
	if( slen != 0 ) memcpy(s+index, sp, slen);
	free(sp);
	return slen;
}
int __cs_n_(char *s, size_t lim, size_t index, ConvSpec *cs, Arg* arg, int flag)
{
	assert(0);
}
int __cs_esc_(char *s,size_t lim,size_t index, ConvSpec *cs, Arg* arg, int flag)
{
	if( index+1 >= lim ) return 0;
	s[index] = '%';
	return 1;
}

int __cs_dummy_(char *s,size_t lim,size_t index, ConvSpec *cs, Arg* arg, int flag)
{
	return 0;
}

int (*convFuncs[FA_LENGTH+1])(char*, size_t, size_t, ConvSpec*, Arg*, int) =
{
	__cs_dummy_,
	__cs_d_, __cs_d_, __cs_o_, __cs_u_, __cs_x_, __cs_X_, __cs_f_, __cs_F_,
	__cs_e_, __cs_E_, __cs_g_, __cs_G_, __cs_a_, __cs_A_, __cs_c_, __cs_s_,
	__cs_p_, __cs_n_, __cs_esc_
};
