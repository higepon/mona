#ifndef _MONAPI_DATE_
#define _MONAPI_DATE_

#include <sys/types.h>
#include <monapi/syscall.h>

namespace MonAPI {

/*
    class: Date

*/
class Date {

  public:
    /*
       function: Date

       Creates and returns current date.

       Returns:

         current date.
    */
    Date()
    {
        syscall_get_date(&date);
    }

    Date(KDate date) : date(date)
    {
    }

    ~Date() {}

  public:
    /*
       function: toUnixTime

       Returns current time in unixtime

       Returns:

         current time in unixtime.
    */
    uint64_t toUnixTime() const
    {
        const uint64_t EPOC_TIME = 719161;
        return ((365 * year() + (year() / 4) - (year() / 100) + (year() / 400) + (306 * (month() + 1) / 10) - 428 + day()) -  EPOC_TIME) * 86400 + (hour() * 3600) + (min() * 60) + sec();

    }

    /*
       function: nowInMsec

       Returns current time in msec since 1970/1/1 0:00

       Returns:

         current time in msec..
    */
    static uint64_t nowInMsec()
    {
        return syscall_now_in_nanosec() / 1000000;
    }

    /*
       function: year

       current year

       Returns:

         current year.
    */
    inline int year() const {
        return date.year;
    }

    /*
       function: month

       current month

       Returns:

         current month.
    */
    inline int month() const {
        return date.month;
    }

    /*
       function: day

       current day

       Returns:

         current day.
    */
    inline int day() const {
        return date.day;
    }

    /*
       function: hour

       current hour

       Returns:

         current hour.
    */
    inline int hour() const {
        return date.hour;
    }

    /*
       function: minutes

       current minutes

       Returns:

         current minutes.
    */
    inline int min() const {
        return date.min;
    }

    /*
       function: seconds

       current seconds

       Returns:

         current seconds.
    */
    inline int sec() const {
        return date.sec;
    }

    /*
       function: day of week

       current day of week

       Returns:

         current day of week.
    */
    inline int dayofweek() const {
        return date.dayofweek;
    }

    KDate getKDate() const
    {
        return date;
    }
  private:
    KDate date;
};

}

#endif
