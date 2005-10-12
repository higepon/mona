// This file was created by `gcjh -stubs'. -*- c++ -*-
//
// This file is intended to give you a head start on implementing native
// methods using CNI.
// Be aware: running `gcjh -stubs ' once more for this class may
// overwrite any edits you have made to this file.

#include <java/util/Date.h>
#include <gcj/cni.h>
#ifdef MONA
#include <monapi.h>
#include <sys/types.h>
#else
#include <stdio.h>
#include <time.h>
#endif

void
java::util::Date::create (::java::util::Date *date)
{
#ifdef MONA
	KDate kdate;
	syscall_get_date(&kdate);
	date->year = kdate.year;
	date->month = kdate.month;
	date->date = kdate.day;
	date->hour = kdate.hour;
	date->minutes = kdate.min;
	date->second = kdate.sec;
#else
	time_t timer;
	struct tm *kdate;
	time(&timer);
	kdate = localtime(&timer);
	date->year = kdate.tm_year + 1900;
	date->month = kdate.tm_mon + 1;
	date->date = kdate.tm_mday;
	date->hour = kdate.tm_hour;
	date->minutes = kdate.tm_min;
	date->second = kdate.tm_sec;
#endif
}


