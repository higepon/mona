#include "global.h"
#include "RTC.h"
#include "io.h"

/*!

    \brief  RTC

    Copyright (c) 2002-2005 Higepon and the individuals listed on the ChangeLog entries.
    All rights reserved.
    License=MIT/X License

    \author  HigePon
    \version $Revision$
    \date   create:2005/07/21 update:$Date$
*/

/*----------------------------------------------------------------------
    RTC
----------------------------------------------------------------------*/
void RTC::init() {

    /* 24h */
    write(0x0B, 0x02);
}

void RTC::write(byte reg, byte value) {

    dword eflags = get_eflags();
    disableInterrupt();

    outp8(RTC_ADRS, reg);
    outp8(RTC_DATA, value);
    set_eflags(eflags);
}

byte RTC::read(byte reg) {

    byte result;
    dword eflags = get_eflags();
    disableInterrupt();

    outp8(RTC_ADRS, (byte)(reg & 0xff));
    result = inp8(RTC_DATA);

    set_eflags(eflags);
    return result;
}

int RTC::readDateOnce(KDate* date) {

    date->year      = convert(read(RTC_YEAR)) + 2000;
    date->month     = convert(read(RTC_MONTH));
    date->day       = convert(read(RTC_DAY));
    date->dayofweek = convert(read(RTC_DOW));
    date->hour      = convert(read(RTC_HOUR));
    date->min       = convert(read(RTC_MIN));
    date->sec       = convert(read(RTC_SEC));
    return date->min;
}

void RTC::getDate(KDate* date) {

    for (;;) {
        int once  = readDateOnce(date);
        int twice = readDateOnce(date);
        if (once == twice) break;
    }
    return;
}

void rdtsc(dword* timeL, dword* timeH) {

    dword l,h;
    asm volatile("rdtsc           \n"
                 "mov   %%eax, %0 \n"
                 "mov   %%edx, %1 \n"
                 : "=m"(l), "=m"(h)
                 : /* no */
                 : "eax", "edx");
    *timeL = l;
    *timeH = h;
}

void rdtscsub(dword* timeL, dword* timeH) {

    dword l = *timeL;
    dword h = *timeH;

    asm volatile("rdtsc           \n"
                 "sub   %2, %%eax \n"
                 "sub   %3, %%edx \n"
                 "mov   %%eax, %0 \n"
                 "mov   %%edx, %1 \n"
                 : "=m"(l), "=m"(h)
                 : "m"(l), "m"(h)
                 : "eax", "edx");

    *timeL = l;
    *timeH = h;
}
