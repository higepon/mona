#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include "digitconv.h"

int __nc_count_digits(int n, int x)
{
	int i = 0;
	n = abs(n);
	while(n)
	{
		n /= x; i++;
	}
	return i;
}

int __nc_conv_digits(char *s, size_t size, unsigned int n, int x, const struct fix_traits *tr_)
{
	int digits, c, i;
	char *p = s;
	struct fix_traits tr;

	memcpy(&tr, tr_, sizeof(struct fix_traits));
	digits = __nc_count_digits(n, x);
	if( tr.another )
	{
		tr.add_sign_flag = false;
		if( x == 8 )
			*p++ = '0';
		else if( x == 16 )
		{
			*p++ = '0'; *p++ = 'x';
		}
	}
	if( tr.add_sign_flag )
	{
		if( tr.sign_flag ) *p++ = '-';
		else if( !tr.sign_flag ) *p++ = '+';
	}
	if( !tr.sign_flag && tr.space )
	{
		*p++ = ' ';
	}
	for( i = 0 ; i < tr.prec-digits ; i++ )
	{
		*p++ = '0';
	}

	p += digits;
	if( p > s+size ) return -1;
	*p = '\0';
	while(n)
	{
		c = n%x;
		if( c < 10 ) c += 0x30; else c+= 0x57;
		*--p = c;
		n /= x;
	}

	return p-s;
}

char *__nc_fix_format_string(char *s, size_t size, const struct fix_traits *tr_)
{
	struct fix_traits tr;
	int slen, i, c;
	char *p = s;

	memcpy(&tr, tr_, sizeof(struct fix_traits));
	slen = strlen(s);

	if( tr.field_width != 0 && tr.field_width >= slen )
	{
		if( tr.zero_padding ) c = '0'; else c = ' ';
		if( tr.left_align ) p += slen;
		else memmove(p+(tr.field_width-slen), p, slen+1);
		for( i = 0 ; i < tr.field_width-slen ; i++ ) *p++ = c;
		if( tr.left_align ) *p = '\0';
	}
	return s;
}

int __nc_digitconv(char *s, size_t size, int n, int x, const struct fix_traits *tr_)
{
	struct fix_traits tr;
	int result;

	memcpy(&tr, tr_, sizeof(struct fix_traits));
	if( n < 0 ) tr.sign_flag = true;
	n = abs(n);

	result = __nc_conv_digits(s, size, (unsigned int)n, x, tr_);
	if( result == -1 ) return -1;
	__nc_fix_format_string(s, size, tr_);
	return strlen(s);
}

int __nc_getStringLength(unsigned int n, int x, const struct fix_traits *tr_)
{
	int length;
	length = __nc_count_digits(n, x);
	if( tr_->prec < length && tr_->prec != 0 ) length = tr_->prec;
	if( tr_->another && n >= 0 )
	{ if( x == 8 ) length++; else if( x == 16 ) length += 2; }
	if( tr_->sign_flag || tr_->add_sign_flag ) length++;
	if( tr_->field_width < length && tr_->field_width != 0 )
		length = tr_->field_width;
	return length;
}
