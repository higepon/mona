/*!
    \file  rtc.h
    \brief real time clock

    Copyright (c) 2003 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2003/03/09 update:$Date$
*/

#ifndef _MONA_RTC_
#define _MONA_RTC_

#include <monaTypes.h>
#include <io.h>

typedef struct Date {
    byte year;
    byte month;
    byte day;
    byte dayofweek;
    byte hour;
    byte min;
    byte sec;
};

byte rtc_read(byte reg);
byte rtc_read_date(Date* date);
void rtc_get_date(Date* date);

#endif
