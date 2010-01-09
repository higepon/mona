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
uint64_t RTC::epochNanosec_;
void RTC::init() {

    /* 24h */
    write(0x0B, 0x02);
}

void RTC::write(uint8_t reg, uint8_t value) {

    uint32_t eflags = get_eflags();
    disableInterrupt();

    outp8(RTC_ADRS, reg);
    outp8(RTC_DATA, value);
    set_eflags(eflags);
}

uint8_t RTC::read(uint8_t reg) {

    uint8_t result;
    uint32_t eflags = get_eflags();
    disableInterrupt();

    outp8(RTC_ADRS, (uint8_t)(reg & 0xff));
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

uint64_t RTC::readEpochNanoSeconds()
{
    static int DAYS_IN_MONTH[]= {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31};
    KDate date;
    readDateOnce(&date);
    ASSERT(date.year >= 1970);

    uint64_t days = 0;
    for (int i = 1970; i < date.year; i++) {
        days += 365;
        if (isLeapYear(i)) {
            days++;
        }
    }

    for (int i = 0; i < date.month - 1; i++) {
        int daysInMonth= DAYS_IN_MONTH[i];
        if (i == 1 && isLeapYear(i)) {
            daysInMonth++;
        }
        days += daysInMonth;
    }
    days += date.day;

    uint64_t ret = days * 24 * 60 * 60 * 1000000000;
    ret += date.hour * 60 * 60 * 1000000000;
    ret += date.min * 60 * 1000000000;
    ret += date.sec * 1000000000;
    return ret;
}

void RTC::getDate(KDate* date) {

    for (;;) {
        int once  = readDateOnce(date);
        int twice = readDateOnce(date);
        if (once == twice) break;
    }
    return;
}

void rdtsc(uint32_t* timeL, uint32_t* timeH) {

    uint32_t l,h;
    asm volatile("rdtsc           \n"
                 "mov   %%eax, %0 \n"
                 "mov   %%edx, %1 \n"
                 : "=m"(l), "=m"(h)
                 : /* no */
                 : "eax", "edx");
    *timeL = l;
    *timeH = h;
}

void rdtscsub(uint32_t* timeL, uint32_t* timeH) {

    uint32_t l = *timeL;
    uint32_t h = *timeH;

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
