/**
	@file	Time.h
	@brief	時刻・時間関連

	時刻・時間関連

	License=Mona License
	@version $Revision$
	@date	$Date$
*/
//バグ修正をする時は関数本体説明の@date履歴に日付と名前と作業内容を付け足しておいてください。
//また.hファイルにあるクラス説明などの@date履歴部分にも同様の事をしておいてください。
#ifndef _MONAPI2_BASIC_TIME_H
#define _MONAPI2_BASIC_TIME_H

#include "Switch.h"
#include "Type.h"

namespace monapi2	{

/**
	@brief	時間・時刻を取り扱う。Monapi2リファレンスも参照。
	@date	2005/08/20	junjunn 作成
*/
class Time
{
public:
//構築
	Time()						{init();}
	Time(const Time* cpTime)	{set(cpTime);}
	Time(int iSecondCount)		{set(iSecondCount);}
	Time(int iYear,int iMonth,int iDay,int iHour,int iMinute,int iSecond)	{set(iYear,iMonth,iDay,iHour,iMinute,iSecond);}

//取得
	static int getCurrentSecondCount();

	int getYear()		const{return m_iYear;}			///<年を得る
	int getMonth()		const{return m_iMonth;}			///<月を得る
	int getDay()		const{return m_iDay;}			///<日を得る
	int getHour()		const{return m_iHour;}			///<時を得る
	int getMinute()		const{return m_iMinute;}		///<分を得る
	int getSecond()		const{return m_iSecond;}		///<秒を得る
	int getDayOfWeek()	const{return m_iDayOfWeek;}		///<何曜日か。0=日曜日。
	int getSecondCount()const{return m_iSecondCount;}	///<総秒数を得る

	bool isEqual(const Time* cpTime);
	bool operator ==(const Time& crefTime)	{return isEqual(&crefTime);}
	bool operator !=(const Time& crefTime)	{return ! (isEqual(&crefTime));}
	bool isBefore(const Time* cpTime,bool bAllowEqual=false)	{return  (bAllowEqual)?((getSecondCount()) <= (cpTime->getSecondCount())):(getSecondCount() < cpTime->getSecondCount());}
	bool operator <(const Time& crefTime)	{return isBefore(&crefTime,false);}
	bool operator <=(const Time& crefTime)	{return isBefore(&crefTime,true);}
	bool isAfter(const Time* cpTime,bool bAllowEqual=false)		{return  (bAllowEqual)?(getSecondCount() >= cpTime->getSecondCount()):(getSecondCount() > cpTime->getSecondCount());}
	bool operator >(const Time& crefTime)	{return isAfter(&crefTime,false);}
	bool operator >=(const Time& crefTime)	{return isAfter(&crefTime,true);}
	int compare(const Time* cpTime)			{return getSecondCount() - cpTime->getSecondCount();}

	Time add(int iSecondCount)			{return Time(getSecondCount()+iSecondCount);}
	Time operator +(int iSecondCount)	{return add(iSecondCount);}
	Time subtract(int iSecondCount)		{return Time(getSecondCount()-iSecondCount);}
	Time operator -(int iSecondCount)	{return subtract(iSecondCount);}

//操作
	void set(int iSecondCount);						///<秒数カウントをセットしYMDhms時刻に変換。
	void operator =(int iSecondCount)	{set(iSecondCount);}
	void set(const Time* cpTime);
	void operator = (const Time& crefTime)	{set(&crefTime);}
	void set(int iYear,int iMonth,int iDay,int iHour,int iMinute,int iSecond);

	void addSelf(int iSecondCount)		{set(getSecondCount()+iSecondCount);}
	void operator +=(int iSecondCount)	{addSelf(iSecondCount);}
	void subtractSelf(int iSecondCount)	{set(getSecondCount()-iSecondCount);}
	void operator -=(int iSecondCount)	{subtractSelf(iSecondCount);}

protected:
///一年の中の日数カウントから月と日を求める。
	void getMonthAndDay(int iDayInYear,int* piMonth,int* piDay);

///1970からiYear,iMonthまでの閏年の回数を求める。
	int getLeapYearCountSince1970(int iYear,int iMonth);

///初期化
	void init();

protected:
	int m_iYear;
	int m_iMonth;
	int m_iDay;
	int m_iDayOfWeek;
	int m_iHour;
	int m_iMinute;
	int m_iSecond;
	int m_iDayCountInYear;		//一年の中のトータル日数
	int m_iSecondCount;			//1970年1月1日からの総秒数。
};

/**
	@brief	二点間の時刻を計測。Monapi2リファレンスも参照。
	@date	2005/08/20	junjunn 作成
*/
class TimeMeasure
{
public:
	void start();
	uint finish();

protected:
	uint m_nTick;
};

}	//namespace monapi2

#endif
