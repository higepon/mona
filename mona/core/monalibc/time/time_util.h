#ifndef __TIME_UTIL_H__
#define __TIME_UTIL_H__

#include <sys/types.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

int isLeapYear(int year);
int countLeapYears(int epoch, int year);
int getYdayfromKDate(KDate *kd);
int getYdayfromTM(struct tm *tm);
time_t KDate2time_t(KDate *tm);
int getWday(int year, int mon, int mday);
int getMonth(int year, int yday);

#ifdef __cplusplus
}
#endif

#endif
