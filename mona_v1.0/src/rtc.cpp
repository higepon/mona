#include<rtc.h>
#include<monaKernel.h>

#define RTC_ADRS  0x70
#define RTC_DATA  0x71
#define RTC_SEC   0x00
#define RTC_MIN   0x02
#define RTC_HOUR  0x04
#define RTC_DOW   0x06
#define RTC_DAY   0x07
#define RTC_MONTH 0x08
#define RTC_YEAR  0x09

byte rtc_read(byte reg) {

    byte result;

    pushf();
    disableInterrupt();

    outportb(RTC_ADRS, (byte)(reg & 0xff));
    result = inportb(RTC_DATA);

    popf();
    return result;
}

byte rtc_read_date(Date* date) {

    date->year      = rtc_read(RTC_YEAR);
    date->month     = rtc_read(RTC_MONTH);
    date->day       = rtc_read(RTC_DAY);
    date->dayofweek = rtc_read(RTC_DOW);
    date->hour      = rtc_read(RTC_HOUR);
    date->min       = rtc_read(RTC_MIN);
    date->sec       = rtc_read(RTC_SEC);

    //    console->printf("%d, %d/%d, %d:%d:%d (%d)", (dword)(date->year), (dword)(date->month), (dword)(date->day), (dword)(date->hour), (dword)(date->min), (dword)(date->sec), (dword)(date->dayofweek));

    byte min = date->min;
    return min;
}

void rtc_get_date(Date* date) {

    byte min = rtc_read_date(date);
    if (min != rtc_read_date(date)) rtc_read_date(date);

    return;
}
