/*************************************************************
 * Copyright (c) 2006 Shotaro Tsuji
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is	 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
 * THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 *************************************************************/

/* Please send bug reports to
	Shotaro Tsuji
	4-1010,
	Sakasedai 1-chome,
	Takaraduka-si,
	Hyogo-ken,
	665-0024
	Japan
	negi4d41@yahoo.co.jp
*/

#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <math.h>

#define PRINTF_FLAG_ANOTHER 0x01
#define PRINTF_FLAG_ZERO    0x02
#define PRINTF_FLAG_LEFT    0x04
#define PRINTF_FLAG_SPACE   0x08
#define PRINTF_FLAG_FLAG    0x10
#define PRINTF_FLAG_LARGE   0x20
#define PRINTF_hh 1
#define PRINTF_h  2
#define PRINTF_l  3
#define PRINTF_ll 4
#define PRINTF_LL 5
#define PRINTF_q  6
#define PRINTF_j  7
#define PRINTF_z  8
#define PRINTF_t  9

char hexdigits_table[] = {"0123456789abcdef0123456789ABCDEF"};

static int __nc_get_digits(int x)
{
//	return (int)floor(log10((double)x))+1;
	int digits = 0;
	while( x != 0 )
	{
		x /= 10;
		digits++;
	}
	return digits;
}

static int __nc_get_digits_hex(int x)
{
//	return (int)floor(log2((double)x))/4.0+1;
//	return (int)floor((log((double)x))/log(16))+1;
	int digits = 0;
	while( x != 0 )
	{
		x /= 16;
		digits++;
	}
	return digits;
}

static int __nc_get_digits_oct(int x)
{
//	return (int)floor(log2((double)x))/3.0+1;
//	return (int)floor((log((double)x))/log(8))+1;
	int digits = 0;
	while( x != 0 )
	{
		x /= 8;
		digits++;
	}
	return digits;
}

static void __nc_put_spaces(FILE *s, int fwidth, int len, char c)
{
	int i;
	if( fwidth > len )
	{
		for( i = 0 ; i < fwidth - len ; i++ )
			putc(c, s);
	}
}

static int __nc_convReal(FILE *stream, double x, int prec, int width, int flags)
{
	int result = 0;
	int n;
	if( x < 0 )
	{
		x = -x;
		putc('-', stream); result++;
	}

	if( isinf(x) )
	{
		result += fwrite("inf", 1, 3, stream);
		return result;
	}
	if( isnan(x) )
	{
		result += fwrite("nan", 1, 3, stream);
		return result;
	}
	n = (int)floor(x);
	putc(n+0x30, stream);
	result++;
	if( prec == 0 ) return result;
	putc('.', stream); result++;
	x = x - floor(x);
	x *= 10.0;
	while(1)
	{
		if( prec == 0 ) break;
		if( prec == 1 )
			n = (int)round(x);
		else
			n = (int)floor(x);
		if( n == 0 ) break;
		putc(n+0x30, stream);
		result++;
		x = x - floor(x);
		x *= 10.0;
		prec--;
	}
	return result;
}

static int __nc_vfprintf_format(FILE *stream, char **formatp, va_list argp)
{
	int result = 0;
	int flags = 0;
	int fwidth = -1;
	int prec = -1;
	int lmod = 0;
	int large_char = 0;
	char *format;
	static va_list ap;
	static int apIsAssigned = 0;
	if( stream == NULL && formatp == NULL )
	{
		apIsAssigned = 0;
		return 0;
	}
	if( !apIsAssigned )
	{
		va_copy(ap, argp);
		apIsAssigned = 1;
	}
	format = *formatp;
	char *s;
	while(*++format) switch(*format)
	{
		case '#': flags |= PRINTF_FLAG_ANOTHER; break;
		case '-': flags |= PRINTF_FLAG_LEFT; break;
		case ' ': flags |= PRINTF_FLAG_SPACE; break;
		case '+': flags |= PRINTF_FLAG_FLAG; break;
		case '0':
			if( *(format-1) == '%' )
			{
				flags |= PRINTF_FLAG_ZERO; break;
			}
		case '.':
			prec = 0;
			while(*++format)
			{
				if( !isdigit(*format) ) break;
				prec *= 10;
				prec += (*format-0x30);
			}
			format--;
			break;
		case 's':
		{
			int len, i;
			s = va_arg(ap, char*);
			len = strlen(s);
			result += len;
			if( prec != -1 )
			{
				fwidth = prec;
				flags |= PRINTF_FLAG_LEFT;
			}
			if( flags & PRINTF_FLAG_LEFT )
			{
				fwrite(s, 1, len, stream);
				__nc_put_spaces(stream, fwidth, len, ' ');
			}
			else
			{
				__nc_put_spaces(stream, fwidth, len, ' ');
				fwrite(s, 1, len, stream);
			}
			*formatp = format;
			return result;
		}
		case 'i':
		case 'd':
		{
			int digits;
			int x;
			int a, b, c, i;
			x = //lmod == PRINTF_hh ? (int)va_arg(ap, signed char) :
			    //lmod == PRINTF_h  ? (int)va_arg(ap, signed short):
//			    lmod == PRINTF_l  ? (int)va_arg(ap, long int)    :
//			    lmod == PRINTF_z  ? (int)va_arg(ap, size_t)      :
//			    lmod == PRINTF_t  ? (int)va_arg(ap, ptrdiff_t)   :
			    (int)va_arg(ap, int);
//			x = va_arg(ap, int);
			if( flags & PRINTF_FLAG_FLAG )
			{
				putc(x>0?'+':'-', stream);
			}
			else if( x < 0 )
			{
				putc('-', stream);
				x = -x;
			}
			else if( x > 0 && flags & PRINTF_FLAG_SPACE )
			{
				putc(' ', stream);
			}
			digits = __nc_get_digits(x);
			b = pow((double)10, (double)(digits-1));
			a = x;
			if( prec == -1 ) prec = 1;
			for( i = 0 ; i < prec - digits ; i++ )
				putc('0', stream);
			for( i = 0 ; i < digits ; i++ )
			{
				c = a / b;
				putc(c+0x30, stream);
				a = a % b;
				b = b / 10;
			}
			result += prec>digits?prec:digits;
			*formatp = format;
			return result;
		}
		case 'X': large_char = 1; goto caseX;
		case 'p': flags |= PRINTF_FLAG_ANOTHER; prec = 8; goto caseX;
		case 'x':
		caseX:
		{
			int digits;
			unsigned int x;
			unsigned int a, b, c;
			int i;
			/*
			x = lmod == PRINTF_hh ? (int)va_arg(ap, signed char) :
			    lmod == PRINTF_h  ? (int)va_arg(ap, signed short):
			    lmod == PRINTF_l  ? (int)va_arg(ap, long int)    :
			    lmod == PRINTF_z  ? (int)va_arg(ap, size_t)      :
//			    lmod == PRINTF_t  ? (int)va_arg(ap, ptrdiff_t)   :
			    (int)va_arg(ap, int);
			    */
			x = va_arg(ap, unsigned int);
			if( flags & PRINTF_FLAG_ANOTHER )
			{
				fwrite(large_char==1?"0X":"0x",1,2,stream);
			}
			digits = __nc_get_digits_hex(x);
			b = pow((double)16, (double)(digits-1));
			a = x;
			if( prec == -1 ) prec = 1;
			for( i = 0 ; i < prec - digits ; i++ )
				putc('0', stream);
			for( i = 0 ; i < digits ; i++ )
			{
				c = a / b;
				if( large_char == 1 ) c += 16;
				putc(hexdigits_table[c], stream);
				a = a % b;
				b = b / 16;
			}
			result += prec>digits?prec:digits;
			*formatp = format;
			return result;
		}
		case 'o':
		{
			int digits;
			unsigned int x;
			unsigned int a, b, c;
			int i;
			/*
			x = lmod == PRINTF_hh ? (int)va_arg(ap, signed char) :
			    lmod == PRINTF_h  ? (int)va_arg(ap, signed short):
			    lmod == PRINTF_l  ? (int)va_arg(ap, long int)    :
			    lmod == PRINTF_z  ? (int)va_arg(ap, size_t)      :
//			    lmod == PRINTF_t  ? (int)va_arg(ap, ptrdiff_t)   :
			    (int)va_arg(ap, int);
			    */
			x = va_arg(ap, unsigned int);
			if( flags & PRINTF_FLAG_ANOTHER )
			{
				putc('0', stream);
			}
			digits = __nc_get_digits_oct(x);
			b = pow((double)8, (double)(digits-1));
			a = x;
			if( prec == -1 ) prec = 1;
			for( i = 0 ; i < prec - digits ; i++ )
				putc('0', stream);
			for( i = 0 ; i < digits ; i++ )
			{
				c = a / b;
				putc(c+0x30, stream);
				a = a % b;
				b = b / 8;
			}
			result += prec>digits?prec:digits;
			*formatp = format;
			return result;
		}
		case 'f':
		{
			double d;
			d = va_arg(ap, double);
			result += __nc_convReal(stream, d, prec==-1?6:prec, fwidth, flags);
			*formatp = format;
			return result;
		}
		case 'c':
		{
			int c;
			result++;
			c = va_arg(ap, int);
			putc(c, stream);
			*formatp = format;
			return result;
		}
		case '%':
			result++;
			putc('%', stream);
			*formatp = format;
			return result; 
		default:
			if( !isdigit(*format) && *format == '0' )
				break;
			fwidth = 0;
			while(isdigit(*format))
			{
				fwidth *= 10;
				fwidth += (*format-0x30);
				*format++;
			}
			format--;
			break;
	}
	return result;
}

int vfprintf(FILE *stream, const char *format, va_list ap)
{
	int result = 0;
	char *fp;

	fp = (char*)format;
	char **fpp = (char**)&fp;

	do
	{
		if( feof(stream) ) break;
		if( *fp == '%' )
		{
			result += __nc_vfprintf_format(stream, fpp, ap);
			//fp++;
		}
		else
		{
			putc(*fp, stream);
			result++;
		}
	}
	while(*fp++);

	__nc_vfprintf_format(NULL, NULL, ap);

	return result;
}
