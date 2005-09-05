/**
	@file	Point.h
	@brief	(x,y)の二次元座標

	(x,y)の二次元座標

	License=Mona License
	@version $Revision$
	@date	$Date$
*/
//バグ修正をする時は関数本体説明の@date履歴に日付と名前と作業内容を付け足しておいてください。
//また.hファイルにあるクラス説明などの@date履歴部分にも同様の事をしておいてください。
#ifndef _MONAPI2_MULTIMEDIA_POINT_H
#define _MONAPI2_MULTIMEDIA_POINT_H

#include "../Basic/Switch.h"
#include "../Basic/Type.h"

namespace monapi2	{

/**
	@brief	(x,y)の二次元座標。Monapi2リファレンスも参照。
	@date	2005/08/20	junjunn 作成
*/
class Point
{
public:
	Point()											{set(0,0);}
	Point(int _x,int _y)							{set(_x,_y);}
	Point(const Point* cpPoint)						{set(cpPoint);}

//取得
	int getX() const								{return x;}
	int getY() const								{return y;}
	bool isEqual(int _x,int _y) const				{return (x==_x && y==_y);}
	bool isEqual(const Point* cpPoint) const		{return isEqual(cpPoint->x,cpPoint->y);}
	bool operator ==(const Point& crefPoint) const	{return isEqual(&crefPoint);}
	bool operator !=(const Point& crefPoint) const 	{return ! isEqual(&crefPoint);}
	Point add(int _x,int _y) const					{Point point(x+_x,y+_y);return point;}
	Point add(const class Size* cpSize) const;
	Point operator +(const Size& crefSize) const;
	Point subtract(int _x,int _y) const				{Point point(x-_x,y-_y);return point;}
	Point subtract(const class Size* cpSize) const;
	Point operator -(const Size& crefSize) const;

//操作 
	void set(int _x,int _y)						{x=_x;y=_y;}
	void set(const Point* cpPoint)				{set(cpPoint->x,cpPoint->y);}
	void operator =(const Point& crefPoint)		{set(&crefPoint);}
	void move(int _x,int _y)					{x+=_x;y+=_y;}
	void move(const class Size* cpSize);
	void operator +=(const Size& crefSize);
	void moveMinus(int _x,int _y)				{move(-_x,-_y);}
	void moveMinus(const class Size* cpSize);
	void operator -=(const class Size& crefSize);

public:
	int x;
	int y;
};

	
}	//namespace monapi2

#endif
