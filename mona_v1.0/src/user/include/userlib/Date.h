#ifndef _MONA_USERLIB_DATE_
#define _MONA_USERLIB_DATE_

#include <types.h>

extern "C" int syscall_get_date(KDate*);
/*----------------------------------------------------------------------
    Date
----------------------------------------------------------------------*/
class Date {

  public:
    Date() {
        syscall_get_date(&date);
    }

    ~Date() {}

  public:
    inline int year() const {
        return date.year;
    }

    inline int month() const {
        return date.month;
    }

    inline int day() const {
        return date.day;
    }

    inline int hour() const {
        return date.hour;
    }

    inline int min() const {
        return date.min;
    }

    inline int sec() const {
        return date.sec;
    }

    inline int dayofweek() const {
        return date.dayofweek;
    }

    inline void refresh() {
        syscall_get_date(&date);
    }

  private:
    KDate date;

};

#endif
