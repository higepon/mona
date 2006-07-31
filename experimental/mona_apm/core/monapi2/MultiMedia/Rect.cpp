/**
	@file	Rect.cpp
	@brief	二次上での四角形

	二次上での四角形

	License=Mona License
	@version $Revision$
	@date	$Date$
*/
//バグ修正をする時は関数本体説明の@date履歴に日付と名前と作業内容を付け足しておいてください。
//また.hファイルにあるクラス説明などの@date履歴部分にも同様の事をしておいてください。
#include "../Basic/Math.h"
#include "Rect.h"
#include "Size.h"
#include "Point.h"

namespace monapi2	{

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
Size Rect::getSize() const
{
	Size size(getWidth(),getHeight());
	return size;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
Point Rect::getLeftTopPoint() const
{
	Point point(getLeft(),getTop());
	return point;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
Point Rect::getRightBottomPoint() const
{
	Point point(getRight(),getBottom());
	return point;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
Point Rect::getCenterPoint() const
{
	Point point((getLeft()+getRight())/2,(getTop()+getBottom())/2);
	return point;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
bool Rect::isEqual(const class Point* cpPointLeftTop,const class Size* cpSize) const
{
	return isEqual(cpPointLeftTop->getX(),cpPointLeftTop->getY(),cpPointLeftTop->getX()+cpSize->getWidth(),cpPointLeftTop->getY()+cpSize->getHeight());
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
bool Rect::isEqual(const Point* cpPointLeftTop,const Point* cpPointRightBottom) const
{
	return isEqual(cpPointLeftTop->getX(),cpPointLeftTop->getY(),cpPointRightBottom->getX(),cpPointRightBottom->getY());
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
bool Rect::isPointInside(int x,int y) const
{
	return (getLeft()<=x && x<getRight() && getTop()<=y && y<getBottom());
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
bool Rect::isPointInside(const Point* cppoint) const
{
	return isPointInside(cppoint->getX(),cppoint->getY());
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
void Rect::moveBy(const Size* cpSize)
{
	moveBy(cpSize->getWidth(),cpSize->getHeight());
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/09/20	junjunn 作成
*/
void Rect::moveBy(const Point* cpPoint)
{
	moveBy(cpPoint->getX(),cpPoint->getY());
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/09/20	junjunn 作成
*/
void Rect::moveTo(const Point* cpPoint)
{
	moveTo(cpPoint->getX(),cpPoint->getY());
}


/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
void Rect::normalize()
{
	if (m_iLeft>m_iRight) swap(&m_iLeft,&m_iRight);
	if (m_iTop>m_iBottom) swap(&m_iTop,&m_iBottom);
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
void Rect::set(const Point* cpPointLeftTop,const Size* cpSize)
{
	set(cpPointLeftTop->getX(),cpPointLeftTop->getY(),cpSize->getWidth(),cpSize->getHeight());
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
void Rect::set(const Point* cpPointLeftTop,const Point* cpPointRightBottom)
{
	set(cpPointLeftTop->getX(),cpPointLeftTop->getY(),
		cpPointRightBottom->getX()-cpPointLeftTop->getX(),cpPointRightBottom->getY()-cpPointLeftTop->getY());
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
void Rect::inflate(const Size* cpsize)
{
	inflate(cpsize->getWidth(),cpsize->getHeight());
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
Rect Rect::getIntersect(const Rect* cprect1,const Rect* cprect2)
{
	Rect rectReturn;
	const Rect* cprectS,*cprectL;	//smallとlarge

//X軸
	if (cprect1->getLeft() <= cprect2->getLeft())
	{
		cprectS	= cprect1;
		cprectL	= cprect2;
	}
	else
	{
		cprectS	= cprect2;
		cprectL	= cprect1;
	}

//交わらないので0を返す。
	if (cprectL->getLeft() >= cprectS->getRight())	return Rect(0,0,0,0);

	rectReturn.m_iLeft = cprectL->getLeft();
	rectReturn.m_iRight = getMin(cprectS->getRight(),cprectL->getRight());

//Y軸
	if (cprect1->getTop() <= cprect2->getTop())
	{
		cprectS	= cprect1;
		cprectL	= cprect2;
	}
	else
	{
		cprectS	= cprect2;
		cprectL	= cprect1;
	}

//交わらないので0を返す。
	if (cprectL->getTop() >= cprectS->getBottom())	return Rect(0,0,0,0);

	rectReturn.m_iTop = cprectL->getTop();
	rectReturn.m_iBottom = getMin(cprectS->getBottom(),cprectL->getBottom());

	return rectReturn;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
Rect Rect::getMinimumInclude(const Rect* cprect1,const Rect* cprect2)
{
	Rect rectReturn;

	rectReturn.m_iLeft	= getMin(cprect1->getLeft(),cprect2->getLeft());
	rectReturn.m_iRight = getMax(cprect1->getRight(),cprect2->getRight());
	rectReturn.m_iTop	= getMin(cprect1->getTop(),cprect2->getTop());
	rectReturn.m_iBottom= getMax(cprect1->getBottom(),cprect2->getBottom());

	return rectReturn;
}


}		//namespace monapi2
