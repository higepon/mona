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
#include "Debug.h"
#include "Time.h"
#include "Math.h"

namespace monapi2
{

//それぞれの月に至るまでの総日数
//31.28.31.30.31.30.31.31,30.31.30.31
int g_aiCumulativeMonthDayCount[] = {0,31,59,90,120,151,181,212,243,273,304,334,365};


/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
void Time::init()
{
	m_iYear		= 0;
	m_iMonth	= 0;
	m_iDay		= 0;
	m_iDayOfWeek= 0;
	m_iHour		= 0;
	m_iMinute	= 0;
	m_iSecond	= 0;
	m_iDayCountInYear =0;
	m_iSecondCount	= 0;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
bool Time::isEqual(const Time* cpTime)
{
	return (
	m_iYear		== cpTime->getYear()				&&
	m_iMonth	== cpTime->getMonth()				&&
	m_iDay		== cpTime->getDay()					&&
	m_iDayOfWeek== cpTime->getDayOfWeek()			&&
	m_iHour		== cpTime->getHour()				&&
	m_iMinute	== cpTime->getMinute()				&&
	m_iSecond	== cpTime->getSecond()				&&
	m_iDayCountInYear ==cpTime->m_iDayCountInYear	&&
	m_iSecondCount	== cpTime->getSecondCount());
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
int Time::getCurrentSecondCount()
{
	return 0;
}

/**
	@brief	1970年1月1日0時0分からの総秒数を年月日時分秒に直す。Monapi2リファレンスも参照。
	@date	2005/08/20	junjunn 作成
*/
void Time::set(int iSecondCount)
{
//未対応
	if (iSecondCount<0)
	{
		init();
		return;
	}

//秒数カウントセット。
	m_iSecondCount = iSecondCount;

//それぞれの単位の秒数変換などを作っておく。
	const uint cnMinuteSecond	= 60;
	const uint cnHourSecond		= cnMinuteSecond * 60;
	const uint cnDaySecond		= cnHourSecond * 24;
	const uint cn4YearDay		= 365 * 4+ 1;

//月は毎月の日数にばらつきがあるし年は閏年がある。よって最初はまず確実な総日数を求める。
	int iDayCount;
	divide(iSecondCount,cnDaySecond	,&iDayCount,&iSecondCount);

//総日数からは曜日を求めれる。
	m_iDayOfWeek = iDayCount%7 + 4;
	if (m_iDayOfWeek>=7)	m_iDayOfWeek-=7;

//閏年問題のため4年単位の大きさを求める。
	int i4YearCount;	//4年の回数。
	int iDayIn4Year;	//i4YearCountでmodを取ったときに残る日数。
	divide(iDayCount,cn4YearDay		,&i4YearCount,&iDayIn4Year);

//4年の中で何年かを求める。
	int iYearIn4Year=3;
	m_iDayCountInYear = iDayIn4Year;
	int aiYearDiv[] = {0,365,730,1096,1461};	//それぞれの年に達するまでの総日数。1970年からのカウントなので3年目に閏年が来てる。
	for (int i=0;i<4;i++)
	{
		if (iDayIn4Year<aiYearDiv[i+1])			//日数がそれぞれの年の総日数に落ちたら抜ける。
		{
			iYearIn4Year = i;
			m_iDayCountInYear = iDayIn4Year - aiYearDiv[i];
			break;
		}
	}

//年を求める。
	m_iYear = 1970 + i4YearCount*4 + iYearIn4Year;

//閏年。4年に一回。しかし100で割り切れる年はなくなる。しかしさらに400で割り切れる年は有効になる。
	bool bLeapYear=false;
	if (m_iYear%4==0)
	{
		bLeapYear = true;
		if (m_iYear%100==0)
		{
			bLeapYear = false;
			if (m_iYear%400==0)
			{
				bLeapYear = true;
			}
		}
	}

//1年の中の総日数から月と日数を求める。
	const int cnLeapDay = 59;	//2月29日の日数カウント。
//閏日を直撃する条件。
	if (bLeapYear && m_iDayCountInYear==cnLeapDay)
	{
		m_iMonth=1;
		m_iDay=28;
	}
	else
	{
//閏年問題を取り除いた総日数。
		int iDayCountInYearNoLeap = m_iDayCountInYear;
		if (bLeapYear && m_iDayCountInYear>cnLeapDay)	iDayCountInYearNoLeap--;

		getMonthAndDay(iDayCountInYearNoLeap,&m_iMonth,&m_iDay);
	}

//残りの秒を時と分と秒にわける。
	divide(iSecondCount,cnHourSecond	,&m_iHour,&iSecondCount);
	divide(iSecondCount,cnMinuteSecond	,&m_iMinute,&m_iSecond);
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
void Time::set(const Time* cpTime)
{
	m_iYear		= cpTime->getYear();
	m_iMonth	= cpTime->getMonth();
	m_iDay		= cpTime->getDay();
	m_iDayOfWeek= cpTime->getDayOfWeek();
	m_iHour		= cpTime->getHour();
	m_iMinute	= cpTime->getMinute();
	m_iSecond	= cpTime->getSecond();
	m_iDayCountInYear =cpTime->m_iDayCountInYear;
	m_iSecondCount	= cpTime->getSecondCount();
}

/**
	@brief 説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
void Time::set(int iYear,int iMonth,int iDay,int iHour,int iMinute,int iSecond)
{
//1970年から閏年が起きた回数を求める。
	int iLeapYearCount = getLeapYearCountSince1970(iYear,iMonth);

//年の中の日数＋月の中の日数＋日数＋閏年補正
	int iDayCount = (iYear-1970)*365 + g_aiCumulativeMonthDayCount[iMonth] + iDay + iLeapYearCount;

//日数の中の秒＋時の中の秒＋分の中の秒＋秒。
	int iSecondCount = iSecond + 60*(iMinute + 60*(iHour + 24*iDayCount));

//秒からセット。
//@memo 現時点ではもう一度年月日時分秒を求める無駄な作業が入る・・
	set(iSecondCount);
}


//protected///////
/**
	@date	2005/08/20	junjunn 作成
*/
void Time::getMonthAndDay(int m_iDayCountInYear,int* piMonth,int* piDay)
{
	for (int i=0;i<12;i++)
	{
		if (m_iDayCountInYear < g_aiCumulativeMonthDayCount[i+1])
		{
			*piMonth = i;
			*piDay = m_iDayCountInYear - g_aiCumulativeMonthDayCount[i];
			return;
		}
	}

	ASSERT(0);		//ここには来ないはず。
}

/**
	@date	2005/08/20	junjunn 作成
*/
int Time::getLeapYearCountSince1970(int iYear,int iMonth)
{
//0～iYear年までの閏年イベント。
	int i4YearCount		= iYear/4;		//4で割り切れる年は閏年。
	int i100YearCount	= iYear/100;	//しかし100で割り切れる年は例外で無効。
	int i400YearCount	= iYear/400;	//しかし400で割り切れる年はさらに例外でその上から有効になる。

//0～1970年までの閏年カウント。
	const int ciLeapYearCountUntil1970 = 477;

//1970年以降の閏年の回数。
	int iLeapYearCount = (i4YearCount-i100YearCount+i400YearCount) - ciLeapYearCountUntil1970;

//今年の分。まだ閏年が起きる前の日なのに含めてしまったケースを修正する。
	if (iYear%4==0 && iMonth<2)		iLeapYearCount--;

	return iLeapYearCount;
}

}		//namespace monapi2
