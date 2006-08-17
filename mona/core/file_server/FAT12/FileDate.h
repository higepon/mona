/*!
    \file  FileDate.h
    \brief FileDate

    Copyright (c) 2005 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision: 3123 $
    \date   create:2005/01/23 update:$Date: 2006-04-15 13:55:21 +0900 (土, 15  4月 2006) $
*/

#ifndef _FILE_DATE_
#define _FILE_DATE_

/*----------------------------------------------------------------------
    FileDate
----------------------------------------------------------------------*/
class FileDate
{
public:
    FileDate() {}
    virtual ~FileDate() {}

public:
    int getYear() const {return this->year;}
    void setYear(int year) {this->year = year;}
    int getMonth() const {return this->month;}
    void setMonth(int month) {this->month = month;}
    int getDay() const {return this->day;}
    void setDay(int day) {this->day = day;}
    int getDayOfWeek() const {return this->dayOfWeek;}
    void setDayOfWeek(int dayOfWeek) {this->dayOfWeek = dayOfWeek;}
    int getHour() const {return this->hour;}
    void setHour(int hour) {this->hour = hour;}
    int getMinute() const {return this->minute;}
    void setMinute(int minute) {this->minute = minute;}
    int getSecond() const {return this->second;}
    void setSecond(int second) {this->second = second;}

private:
    int year;
    int month;
    int day;
    int dayOfWeek;
    int hour;
    int minute;
    int second;
};

#endif
