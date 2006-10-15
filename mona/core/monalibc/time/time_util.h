#ifndef __TIME_UTIL_H__
#define __TIME_UTIL_H__

#include <sys/types.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

int isLeapYear(int year);
int countLeapYears(int epoch, int year);
int getYday(KDate *tm);
time_t KDate2time_t(KDate *tm);

#ifdef __cplusplus
}
#endif

#endif
