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

#include <sys/types.h>
#include "time_util.h"

int isLeapYear(int year)
{
	if( year % 4 == 0 )
	{
		if( year % 100 == 0 )
		{
			if( year % 400 == 0 ) return 1;
			else return 0;
		}
		return 1;
	}
	return 0;
}

int countLeapYears(int epoch, int year)
{
	int i, result = 0;
	for( i = epoch ; i < year ; i++ ) if( isLeapYear(i) ) result++;
	return result;
}

int getYday(KDate *tm)
{
	int year = tm->year;
	int mon = tm->month;
	int result;
	switch(mon)
	{
		case 1: result = 0; break;
		case 2: result = 31; break; /* 1: 31 */
		case 3: result = 59; break; /* 2: 31+28 */
		case 4: result = 90; break; /* 3: 59+31 */
		case 5: result = 120;break; /* 4: 90+30 */
		case 6: result = 151;break; /* 5: 120+31 */
		case 7: result = 181;break; /* 6: 151+30 */
		case 8: result = 212;break; /* 7: 181+31 */
		case 9: result = 243;break; /* 8: 212+31 */
		case 10:result = 273;break; /* 9: 243+30 */
		case 11:result = 304;break; /* 10:273+31 */
		case 12:result = 334;break; /* 11:304+30 */
		default: break;
	}
	if( isLeapYear(year) && mon > 2 ) result++;
	result += tm->day-1;
	return result;
}

time_t KDate2time_t(KDate *tm)
{
	time_t result = 0;

	result = tm->sec;
	result += tm->min*60;
	result += tm->hour*3600;
	result += getYday(tm)*86400;
	result += (tm->year-1970)*31536000;
	result += countLeapYears(1970, tm->year)*86400;

	return result;
}
