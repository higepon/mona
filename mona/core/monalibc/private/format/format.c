#include "format.h"
#include "vargs.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>

const char flag_char[] = "#0- +";
const char conv_char[] = "diouxXfFeEgGaAcspn%";

int __isflag_char(char c)
{
	char *p = (char*)flag_char;

	while(*p)
	{
		if( c == *p++ ) return 1;
	}
	return 0;
}

int __isconv_char(char c)
{
	char *p = (char*)conv_char;

	while(*p)
	{
		if( c == *p++ ) return 1;
	}
	return 0;
}

/*
成功の場合は0を返す。
解析に失敗したときはi+1を返す模様。
その他は-1
*/
int convspec_parse(ConvSpec *cs, char *str)
{
	int i, j;
	int length;
	char digits_buf[32];

	if( cs == NULL ) return 1;

	length = strlen(str);

	if( str[0] == '%' ) i = 1;
	else return 0+1;

	cs->field_width_arg_index = DEFAULT_INDEX;
	cs->precision_arg_index = DEFAULT_INDEX;

	// Check flag.
	for( ; i < length ; i++ )
	{
		if( !__isflag_char(str[i]) ) break;
		switch( str[i] )
		{
			case '-': cs->flag |= FLAG_LEFT; break;
			case '+': cs->flag |= FLAG_SIGNED; break;
			case ' ': cs->flag |= FLAG_SPACE; break;
			case '#': cs->flag |= FLAG_SUB; break;
			case '0': cs->flag |= FLAG_ZERO; break;
		}
	}

	// Check field width.
	if( isdigit(str[i]) && str[i] != '0' )
	{
		memset(digits_buf, 0, 32);
		j = 0;
		for( ; i < length ; i++ )
		{
			if( !isdigit(str[i]) ) break;
			digits_buf[j++] = str[i];
			if( j == 32 ) return i+1;
		}
		cs->field_width = atoi(digits_buf);
	}

	if( str[i] == '*' )
	{
		i++;
		if( !isdigit(str[i]) )
		{
			cs->field_width_arg_index = NEXT_ARG_INDEX;
		}
		else
		{
			memset(digits_buf, 0, 32);
			j = 0;
			for( ; i < length ; i++ )
			{
				if( str[i] == '$' )
				{
					i++;
					break;
				}
				if( !isdigit(str[i]) )
				{
					return i+1;
				}
				digits_buf[j++] = str[i];
				if( j == 32 ) return i+1;
			}
			cs->field_width_arg_index = abs(atoi(digits_buf));
		}
	}

	// Check precision.
	if( str[i] == '.' )
	{
		i++;
		if( isdigit(str[i]) && str[i] != '0' )
		{
			memset(digits_buf, 0, 32);
			j = 0;
			for( ; i < length ; i++ )
			{
				if( !isdigit(str[i]) ) break;
				digits_buf[j++] = str[i];
				if( j == 32 ) return i+1;
			}
			cs->precision = atoi(digits_buf);
		}

		if( str[i] == '*' )
		{
			i++;
			if( !isdigit(str[i]) )
			{
				cs->precision_arg_index = NEXT_ARG_INDEX;
			}
			else
			{
				memset(digits_buf, 0, 32);
				j = 0;
				for( ; i < length ; i++ )
				{
					if( str[i] == '$' )
					{
						i++;
						break;
					}
					if( !isdigit(str[i]) )
					{
						return i+1;
					}
					digits_buf[j++] = str[i];
					if( j == 32 ) return i+1;
				}
				cs->precision_arg_index = atoi(digits_buf);
			}
			if( cs->precision_arg_index < 0 )
			{
				cs->precision_arg_index = 0;
			}
		}
	}

	// Check length modifier.
	switch( str[i] )
	{
		case 'l':
		{
			if( length > i+1 )
			{
				if( str[i+1] == 'l' )
				{
					cs->length_modifier = LMOD_LONGLONG;
					i++;
				}
			}
			else
			{
				cs->length_modifier = LMOD_LONG;
			}
			i++;
			break;
		}
		case 'h':
		{
			if( length > i+1 )
			{
				if( str[i+1] == 'h' )
				{
					cs->length_modifier = LMOD_CHAR;
					i++;
				}
			}
			else
			{
				cs->length_modifier = LMOD_SHORT;
			}
			i++;
			break;
		}
		case 'j': cs->length_modifier = LMOD_INTMAX; i++; break;
		case 'z': cs->length_modifier = LMOD_SIZE_T; i++; break;
		case 't': cs->length_modifier = LMOD_PTRDIFF_T; i++; break;
		case 'L': cs->length_modifier = LMOD_LONGDOUBLE; i++; break;
		default: break;
	}

	// Check conversion cpecifier.
	if( __isconv_char(str[i]) )
	{
		switch(str[i])
		{
			case 'd': cs->conversion = CS_i; break;
			case 'i': cs->conversion = CS_i; break;
			case 'o': cs->conversion = CS_o; break;
			case 'u': cs->conversion = CS_u; break;
			case 'x': cs->conversion = CS_x; break;
			case 'X': cs->conversion = CS_X; break;
			case 'f': cs->conversion = CS_f; break;
			case 'F': cs->conversion = CS_F; break;
			case 'e': cs->conversion = CS_e; break;
			case 'E': cs->conversion = CS_E; break;
			case 'g': cs->conversion = CS_g; break;
			case 'G': cs->conversion = CS_G; break;
			case 'a': cs->conversion = CS_a; break;
			case 'A': cs->conversion = CS_A; break;
			case 'c': cs->conversion = CS_c; break;
			case 's': cs->conversion = CS_s; break;
			case 'p': cs->conversion = CS_p; break;
			case 'n': cs->conversion = CS_n; break;
			case '%': cs->conversion = CS_ESCAPE; break;
			default: break;
		}
		i++;
	}
	else
	{
		return i+1;
	}

	return 0;
}

size_t get_next_convspec_index(const char *str, size_t index)
{
	int i = index;
	int length = strlen(str);
	for( ; i < length ; i++ )
	{
		if( str[i] == '%' ) return (size_t)i;
	}
	return (size_t)-1;
}

size_t get_next_convspec_end_index(const char *str, size_t index)
{
	int i = index;
	int length = strlen(str);
	for( ; i < length ; i++ )
	{
		if( __isconv_char(str[i]) ) return (size_t)i;
	}
	return (size_t)-1;
}

void dump_ConvSpec(ConvSpec *cs)
{
	char f = '?';
	char lm[3];
	if( cs->flag & FLAG_LEFT ) f = '-';
	if( cs->flag & FLAG_SIGNED ) f = '+';
	if( cs->flag & FLAG_SPACE ) f = ' ';
	if( cs->flag & FLAG_SUB ) f = '#';
	if( cs->flag & FLAG_ZERO ) f = '0';
	memset(lm, 0, 3);
	if( cs->length_modifier == LMOD_CHAR ) strcpy(lm, "hh");
	if( cs->length_modifier == LMOD_SHORT ) strcpy(lm, "h");
	if( cs->length_modifier == LMOD_LONG ) strcpy(lm, "ll");
	if( cs->length_modifier == LMOD_LONGLONG ) strcpy(lm, "l");
	if( cs->length_modifier == LMOD_INTMAX ) strcpy(lm, "j");
	if( cs->length_modifier == LMOD_SIZE_T ) strcpy(lm, "z");
	if( cs->length_modifier == LMOD_PTRDIFF_T ) strcpy(lm, "t");
	if( cs->length_modifier == LMOD_LONGDOUBLE ) strcpy(lm, "L");
	printf("%c, %2s, %06d, %04d, %06d, %04d, %c\n",
					f, lm, cs->field_width,
					cs->field_width_arg_index,
					cs->precision,
					cs->precision_arg_index,
					conv_char[cs->conversion-1]);
}

size_t count_convspecs(const char *str)
{
	size_t result = 0;
	int index = 0;
	int next = 0;
	int length = strlen(str);

	while(1)
	{
		index = get_next_convspec_index(str, index);
		if( index == -1 ) return result;
		next = get_next_convspec_end_index(str, index+1);
		if( next == -1 ) return result;
		index = next+1;
		result++;
	}

	return result;
}

int parse_format(Format *format, const char *str)
{
	int strlength;
	int i;
	int next, index, end, next_origin;
	int bufsize;
	ConvSpec *cs;
	char *s;

	if( format == NULL )
	{
		return 1;
	}

	format->length = count_convspecs(str)*2+1;
	format->ins = malloc(format->length*sizeof(struct _informat));
	strlength = strlen(str);

	if( strlength == 0 ) return 0;
	if( format->ins == NULL ) return 1;

	memset(format->ins, 0, format->length*sizeof(struct _informat));
	next = index = end = 0;
	i = 0;

	while(1)
	{
		next = get_next_convspec_index(str, index);
		next_origin = next;
		if( next == -1 )
		{
			end = -1;
			next = strlength;
		}
		else end = get_next_convspec_end_index(str, next+1);

		//printf("next = %d, end = %d, index = %d\n", next, end, index);

		bufsize = next-index;
		if( bufsize > 0 )
		{
			//printf("length = %d\n", bufsize);
			//printf("%.*s\n", bufsize, str+index);

			s = (char*)malloc((bufsize+1)*sizeof(char));
			if( s == NULL ) return 1;
			strlcpy(s, str+index, bufsize+1);
			format->ins[i].flag = 1;
			format->ins[i].u.s = s;
			i++;
		}
		bufsize = end-next+1;
		if( bufsize > 0 )
		{
			//printf("length = %d\n", bufsize);
			//printf("%.*s\n", bufsize, str+next);

			s = (char*)malloc((bufsize+1)*sizeof(char));
			if( s == NULL )
			{
				return 1;
			}
			strlcpy(s, str+next, bufsize+1);
			cs = (ConvSpec*)malloc(sizeof(ConvSpec));
			if( cs == NULL )
			{
				free(s);
				return 1;
			}
			memset(cs, 0, sizeof(ConvSpec));
			if( convspec_parse(cs, s) != 0 )
			{
				free(cs);
				free(s);
				return 1;
			}
			format->ins[i].flag = 2;
			format->ins[i].u.cs = cs;
			free(s);
			i++;
		}

		if( next_origin == -1 ) break;

		index = end+1;
	}

	return 0;
}

int init_format(Format **fmtp)
{
	*fmtp = (Format*)malloc(sizeof(Format));
	if( *fmtp == NULL )
	{
		return 1;
	}
	memset(*fmtp, 0, sizeof(Format));
	return 0;
}

int free_format(Format *fmt)
{
	int i;

	if( fmt == NULL ) return 1;
	for( i = 0 ; i < fmt->length ; i++ )
	{
		free(fmt->ins[i].u.p);
	}
	free(fmt->ins);
	free(fmt);
	return 0;
}

void dump_format(Format *format)
{
	int i;
	int length;

	length = format->length;

	for( i = 0 ; i < length ; i++ )
	{
		if( format->ins[i].flag == 1 )
		{
			printf("%d: String  : %s\n", i, format->ins[i].u.s);
		}
		else if( format->ins[i].flag == 2 )
		{
			printf("%d: ConvSpec: ", i);
			dump_ConvSpec(format->ins[i].u.cs);
		}
	}
}

size_t count_format(Format *f, int flag)
{
	int i;
	int r;
	r = 0;
	for( i = 0 ; i < f->length ; i++ )
	{
		if( f->ins[i].flag == flag ) r++;
	}
	return (size_t)r;
}

size_t max_length_format(Format *f)
{
	int i;
	int r;
	int l;
	r = 0;
	l = count_format(f, 2);
}

size_t trans_format(char *dest, size_t size, const char *format, va_list ap)
{
	size_t result;
	Format *fmt;
	ConvSpec *cs;
	Vargs *vargs;
	int vi = 0;
	int funcresult;
	int i;
	int slen;
	char *dp;

	result = 0;
	dp = dest;

	if( init_format(&fmt) != 0 )
	{
		return -1;
	}

	if( parse_format(fmt, format) != 0 )
	{
		free_format(fmt);
		return -2;
	}

	if( init_vargs(&vargs) != 0 )
	{
		return -3;
	}
	if( __nc_vargs_1(vargs, fmt, ap) != 0 )
	{
		free_format(fmt);
		free_vargs(vargs);
		return -4;
	}

	for( i = 0 ; i < fmt->length ; i++ )
	{
		if( fmt->ins[i].flag == 1 )
		{
			slen = strlen(fmt->ins[i].u.s);
			if( slen+result >= size )
			{
				slen = size-1;
			}
			if( slen != 0 )
			{
				memcpy(dp+result, fmt->ins[i].u.s, slen);
				result += slen;
			}
		}
		else if( fmt->ins[i].flag == 2 )
		{
			cs = fmt->ins[i].u.cs;
			funcresult = convFuncs[cs->conversion](dp,size,result,cs,&(vargs->p[vi]),0);
			result += funcresult;
			vi++;
		}
	}

	dp[result] = '\0';

	free_format(fmt);
	free_vargs(vargs);

	return result;
}
