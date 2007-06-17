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
#include <stdlib.h>
#include <string.h>
#include <time.h>

static size_t __nc_sft_day_of_weeko(char *s, size_t max, int wday)
{
	size_t result = 0;

	switch(wday)
	{
		case 0: result = strlcat(s, "Sun", max); break;
		case 1: result = strlcat(s, "Mon", max); break;
		case 2: result = strlcat(s, "Tue", max); break;
		case 3: result = strlcat(s, "Wed", max); break;
		case 4: result = strlcat(s, "Thu", max); break;
		case 5: result = strlcat(s, "Fri", max); break;
		case 6: result = strlcat(s, "Sat", max); break;
		default: break;
	}

	return result;
}

static size_t __nc_sft_day_of_week(char *s, size_t max, int wday)
{
	size_t result = 0;

	switch(wday)
	{
		case 0: result = strlcat(s, "Sunday", max); break;
		case 1: result = strlcat(s, "Monday", max); break;
		case 2: result = strlcat(s, "Tuesday", max); break;
		case 3: result = strlcat(s, "Wednesday", max); break;
		case 4: result = strlcat(s, "Thursday", max); break;
		case 5: result = strlcat(s, "Friday", max); break;
		case 6: result = strlcat(s, "Saturday", max); break;
		default: break;
	}

	return result;
}

static size_t __nc_sft_montho(char *s, size_t max, int x)
{
	size_t result = 0;

	switch(x)
	{
		case 0: result = strlcat(s, "Jan", max); break;
		case 1: result = strlcat(s, "Feb", max); break;
		case 2: result = strlcat(s, "Mar", max); break;
		case 3: result = strlcat(s, "Apr", max); break;
		case 4: result = strlcat(s, "May", max); break;
		case 5: result = strlcat(s, "Jun", max); break;
		case 6: result = strlcat(s, "Jul", max); break;
		case 7: result = strlcat(s, "Aug", max); break;
		case 8: result = strlcat(s, "Sep", max); break;
		case 9: result = strlcat(s, "Oct", max); break;
		case 10:result = strlcat(s, "Nov", max); break;
		case 11:result = strlcat(s, "Dec", max); break;
		default: break;
	}

	return result;
}

static size_t __nc_sft_month(char *s, size_t max, int x)
{
	size_t result = 0;

	switch(x)
	{
		case 0: result = strlcat(s, "January", max); break;
		case 1: result = strlcat(s, "Feburary", max); break;
		case 2: result = strlcat(s, "March", max); break;
		case 3: result = strlcat(s, "April", max); break;
		case 4: result = strlcat(s, "May", max); break;
		case 5: result = strlcat(s, "June", max); break;
		case 6: result = strlcat(s, "July", max); break;
		case 7: result = strlcat(s, "August", max); break;
		case 8: result = strlcat(s, "September", max); break;
		case 9: result = strlcat(s, "October", max); break;
		case 10:result = strlcat(s, "November", max); break;
		case 11:result = strlcat(s, "December", max); break;
		default: break;
	}

	return result;
}

static size_t __nc_sft_ampm(char *s, size_t max, int x)
{
	size_t result = 0;

	result = strlcat(s, x < 13 ? "AM" : "PM", max);

	return result;
}

static size_t __nc_sft_ampml(char *s, size_t max, int x)
{
	size_t result = 0;

	result = strlcat(s, x < 13 ? "am" : "pm", max);

	return result;
}

static size_t __nc_sft_day(char *s, size_t max, const struct tm *tm)
{
	size_t result = 0;
//	result = snprintf(s, max, "%.02d/%.02d/%.02d", tm->tm_mon+1, tm->tm_mday, tm->tm_year-100);
	result = sprintf(s, "%.02d/%.02d/%.02d", tm->tm_mon+1, tm->tm_mday, tm->tm_year-100);
	return result;
}

static size_t __nc_sft_hms(char *s, size_t max, const struct tm *tm)
{
	size_t result = 0;
//	result = snprintf(s, max, "%.02d:%.02d:%.02d", tm->tm_hour, tm->tm_min, tm->tm_sec);
	result = sprintf(s, "%.02d:%.02d:%.02d", tm->tm_hour, tm->tm_min, tm->tm_sec);
	return result;
}

static size_t __nc_sft_tz(char *s, size_t max)
{
	size_t result = 0;
//	result = snprintf(s, max, "JST");
	result = sprintf(s, "JST");
	return result;
}

size_t strftime(char *s, size_t max, const char *format, const struct tm *tm)
{
	size_t result = 0;
	char *fp;
	fp = (char*)format;

	do
	{
		if( result == max-1 )
		{
			*s = '\0'; break;
		}
		if( *fp == '%' )
		{
			fp++;
			switch(*fp)
			{
				int r;
				case '\0': *s++ = *fp; result++; break;
				case '%': *s++ = '%'; result++; break;
				case 'a':
				r= __nc_sft_day_of_weeko(s, max-result, tm->tm_wday);
				result += r; s += r; break;
				case 'A':
				r = __nc_sft_day_of_week(s, max-result, tm->tm_wday);
				result += r; s += r; break;
				case 'b':
				r = __nc_sft_montho(s, max-result, tm->tm_wday);
				result += r; s += r; break;
				case 'B':
				r = __nc_sft_month(s, max-result, tm->tm_wday);
				result += r; s += r; break;
				case 'c':
				{
				r= __nc_sft_day_of_weeko(s, max-result, tm->tm_wday);
				result += r; s += r;*s++ = ' '; result++;
				r = __nc_sft_montho(s, max-result, tm->tm_wday);
				result += r; s += r;*s++ = ' '; result++;
//				r = snprintf(s, max-result, "%.02d %.02d:%.02d:%.02d %.04d", tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec,tm->tm_year+1900);
				r = sprintf(s, "%.02d %.02d:%.02d:%.02d %.04d", tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec,tm->tm_year+1900);
				result += r; s += r;
				break;
				}
				case 'C':
//				r = snprintf(s, max-result, "%.02d", (tm->tm_year+1900)/100);
				r = sprintf(s, "%.02d", (tm->tm_year+1900)/100);
				result += r; s += r; break;
				case 'd':
//				r=snprintf(s, max-result, "%.02d", tm->tm_mday);
				r=sprintf(s, "%.02d", tm->tm_mday);
				result += r; s += r; break;
				case 'F':
//				r=snprintf(s, max-result, "%.04d-%.02d-%.02d", tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday);
				r=sprintf(s, "%.04d-%.02d-%.02d", tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday);
				result += r; s += r; break;
				case 'H':
//				r=snprintf(s, max-result, "%.02d", tm->tm_hour);
				r=sprintf(s, "%.02d", tm->tm_hour);
				result += r; s += r; break;
				case 'I':
				//r=snprintf(s, max-result, "%.02d", tm->tm_hour > 11 ? tm->tm_hour-12 : tm->tm_hour);
				r=sprintf(s, "%.02d", tm->tm_hour > 11 ? tm->tm_hour-12 : tm->tm_hour);
				result += r; s += r; break;
				case 'm':
//				r=snprintf(s, max-result, "%.02d",tm->tm_mon+1);
				r=sprintf(s, "%.02d",tm->tm_mon+1);
				result += r; s += r; break;
				case 'M':
//				r=snprintf(s, max-result, "%.02d", tm->tm_min);
				r=sprintf(s, "%.02d", tm->tm_min);
				result += r; s += r; break;
				case 'p':
				r = __nc_sft_ampm(s, max-result, tm->tm_hour);
				result += r; s += r; break;
				case 'P':
				r = __nc_sft_ampml(s, max-result, tm->tm_hour);
				result += r; s += r; break;
				case 'S':
//				r=snprintf(s, max-result, "%.02d", tm->tm_sec);
				r=sprintf(s, "%.02d", tm->tm_sec);
				result += r; s += r; break;
				case 'x':
				r = __nc_sft_day(s, max-result, tm);
				result += r; s += r; break;
				case 'X':
				r = __nc_sft_hms(s, max-result, tm);
				result += r; s += r; break;
				case 'y':
//				r=snprintf(s, max-result, "%.02d", tm->tm_year-100);
				r=sprintf(s, "%.02d", tm->tm_year-100);
				result += r; s += r; break;
				case 'Y':
//				r=snprintf(s, max-result, "%.02d", tm->tm_year+1900);
				r=sprintf(s, "%.02d", tm->tm_year+1900);
				result += r; s += r; break;
				case 'Z':
				r = __nc_sft_tz(s, max-result);
				result += r; s += r; break;
				default: break;
			}
		}
		else
		{
			*s++ = *fp;
			result++;
		}
	}
	while(*fp++);

	return result;
}
