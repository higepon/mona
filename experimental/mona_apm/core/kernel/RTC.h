/*!
    \file  rtc.h
    \brief definition for rtc

    definition for tester
    Copyright (c) 2002 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2002/11/11 update:$Date$
*/

#ifndef _MONA_RTC_
#define _MONA_RTC_

#include <global.h>

/*----------------------------------------------------------------------
    RTC
----------------------------------------------------------------------*/
class RTC {

  public:
    static void init();
    static void getDate(KDate* date);

  private:
    static byte read(byte reg);
    static void write(byte reg, byte value);
    static int readDateOnce(KDate* date);
    inline static int convert(byte value) {
        return (value & 0x0f) + ( (value >> 4) * 10);
    }

  public:
    static const byte RTC_ADRS  = 0x70;
    static const byte RTC_DATA  = 0x71;
    static const byte RTC_SEC   = 0x00;
    static const byte RTC_MIN   = 0x02;
    static const byte RTC_HOUR  = 0x04;
    static const byte RTC_DOW   = 0x06;
    static const byte RTC_DAY   = 0x07;
    static const byte RTC_MONTH = 0x08;
    static const byte RTC_YEAR  = 0x09;
};

#endif
