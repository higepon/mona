/*************************************************************
 * Copyright (c) 2006 Shotaro Tsuji
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is     * furnished to do so, subject to the following conditions:
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
#include <monapi/syscall.h>
#include <monapi/Assert.h>
#include "time_util.h"

time_t time(time_t *t) {
    KDate date;
    time_t result;
    syscall_get_date(&date);
    _logprintf("%d %s %s:%d\n", date.year, __func__, __FILE__, __LINE__);
    _logprintf("%d %s %s:%d\n", date.month, __func__, __FILE__, __LINE__);
    _logprintf("%d %s %s:%d\n", date.day, __func__, __FILE__, __LINE__);
    _logprintf("%d %s %s:%d\n", date.hour, __func__, __FILE__, __LINE__);
    _logprintf("%d %s %s:%d\n", date.min, __func__, __FILE__, __LINE__);

    result = KDate2time_t(&date);
    if( t != NULL ) *t = result;
    return result;
}

#define SECONDS_DAY (24 * 60 * 60)
#define IS_LEAPYEAR(year) (!((year) % 4) && (((year) % 100) || !((year) % 400)))
#define DAYS_YEAR(year) (IS_LEAPYEAR(year) ? 366 : 365)

const int _num_days[2][12] = 
{
  {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
  {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
};

struct tm *localtime(const time_t *timep) {
  static struct tm ret;
  time_t now = *timep;

  int year = 1970;
  unsigned long dayclock = (unsigned long) now % SECONDS_DAY;
  unsigned long dayno = (unsigned long) now / SECONDS_DAY;

  ret.tm_sec = dayclock % 60;
  ret.tm_min = (dayclock % 3600) / 60;
  ret.tm_hour = dayclock / 3600;
  ret.tm_wday = (dayno + 4) % 7;
  while (dayno >= (unsigned long) DAYS_YEAR(year))  {
    dayno -= DAYS_YEAR(year);
    year++;
  }
  ret.tm_year = year - 1900;
  ret.tm_yday = dayno;
  ret.tm_mon = 0;
  while (dayno >= (unsigned long) _num_days[IS_LEAPYEAR(year)][ret.tm_mon]) {
    dayno -= _num_days[IS_LEAPYEAR(year)][ret.tm_mon];
    ret.tm_mon++;
  }
  ret.tm_mday = dayno + 1;
  ret.tm_isdst = 0;
  return &ret;
}

int gettimeofday(struct timeval *tv, struct timezone *tz)
{
    if (tv) {
        uint64_t nowMsec = syscall_now_in_nanosec() / 1000000;
        tv->tv_sec = nowMsec / 1000;
        tv->tv_usec = (nowMsec - (nowMsec/1000) * 1000) * 1000;
    }
}
