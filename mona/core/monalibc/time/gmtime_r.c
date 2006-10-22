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

#include <monalibc/time.h>
#include "time_util.h"

struct tm *gmtime_r(const time_t *timep, struct tm *result)
{
	int x;
	int t;
	int i;
	int epoch;

	if( result == NULL ) return NULL;

	t = *timep;
	x = 31536000;
	i = 0;
	epoch = 1970;
	while(1)
	{
		if( isLeapYear(epoch+i) ) x += 86400;
		if( t < x ) break;
		x += 31536000; i++;
	}

	result->tm_year = i+70;

	t -= x-31536000;
	x = 86400;
	i = 0;
	while(1)
	{
		if( t < x ) break;
		x += 86400; i++;
	}

	result->tm_yday = i;

	t -= x-86400;
	x = 3600;
	i = 0;
	while(1)
	{
		if( t < x ) break;
		x += 3600; i++;
	}

	result->tm_hour = i;

	t -= x-3600;
	x = 60;
	i = 0;
	while(1)
	{
		if( t < x ) break;
		x += 60; i++;
	}

	result->tm_min = i;
	t -= x-60;
	result->tm_sec = t;

	result->tm_mon = getMonth(result->tm_year+1900, result->tm_yday);
	result->tm_mday = getMday(result->tm_year+1900, result->tm_mon+1, result->tm_yday);
	result->tm_wday = getWday(result->tm_year+1900, result->tm_mon+1, result->tm_mday);
	result->tm_isdst = -1;

	return result;
}

