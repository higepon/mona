#ifndef _MONAPI_DATE_
#define _MONAPI_DATE_

#include <sys/types.h>

extern "C" int syscall_get_date(KDate*);

namespace MonAPI {

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

}

#endif
