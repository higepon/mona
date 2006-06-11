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
    int GetYear() const {return this->year;}
    void SetYear(int year) {this->year = year;}
    int GetMonth() const {return this->month;}
    void SetMonth(int month) {this->month = month;}
    int GetDay() const {return this->day;}
    void SetDay(int day) {this->day = day;}
    int GetDayOfWeek() const {return this->dayOfWeek;}
    void SetDayOfWeek(int dayOfWeek) {this->dayOfWeek = dayOfWeek;}
    int GetHour() const {return this->hour;}
    void SetHour(int hour) {this->hour = hour;}
    int GetMinute() const {return this->minute;}
    void SetMinute(int minute) {this->minute = minute;}
    int GetSecond() const {return this->second;}
    void SetSecond(int second) {this->second = second;}

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
