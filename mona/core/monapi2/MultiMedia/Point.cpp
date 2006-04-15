/**
	@file	Point.cpp
	@brief	(x,y)の二次元座標

	(x,y)の二次元座標

	License=Mona License
	@version $Revision$
	@date	$Date$
*/
//バグ修正をする時は関数本体説明の@date履歴に日付と名前と作業内容を付け足しておいてください。
//また.hファイルにあるクラス説明などの@date履歴部分にも同様の事をしておいてください。
#include "Point.h"
#include "Size.h"

namespace monapi2	{

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
Point Point::add(const Size* cpSize) const
{
	Point point(x+cpSize->m_iWidth,y+cpSize->m_iHeight);
	return point;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
Point Point::add(const Point* cpPoint) const
{
	Point point(x+cpPoint->getX(),y+cpPoint->getY());
	return point;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
Point Point::operator +(const Size& crefSize) const
{
	Point point(x+crefSize.m_iWidth,y+crefSize.m_iHeight);
	return point;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
Point Point::subtract(const Size* cpSize) const
{
	Point point(x-cpSize->m_iWidth,y-cpSize->m_iHeight);
	return point;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/09/20	junjunn 作成
*/
Size Point::subtract(const Point* cpPoint) const
{
	Size size(x-cpPoint->x,y-cpPoint->y);
	return size;
}

Size Point::operator -(const Point& crefPoint)const
{
	return subtract(&crefPoint);
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
void Point::move(const Size* cpSize)
{
	move(cpSize->getWidth(),cpSize->getHeight());
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
void Point::move(const Point* cpPoint)
{
	move(cpPoint->getX(),cpPoint->getY());
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
void Point::operator +=(const Size& crefSize)
{
	move(&crefSize);
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
void Point::moveMinus(const Size* cpSize)
{
	move(-cpSize->getWidth(),-cpSize->getHeight());
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
void Point::operator -=(const Size& crefSize)
{
	moveMinus(&crefSize);
}


}		//namespace monapi2
