/**
	@file	Color.cpp
	@brief	色を扱う

	色を扱う

	License=Mona License
	@version $Revision$
	@date	$Date$
*/
//バグ修正をする時は関数本体説明の@date履歴に日付と名前と作業内容を付け足しておいてください。
//また.hファイルにあるクラス説明などの@date履歴部分にも同様の事をしておいてください。
#include "Color.h"
#include "../Basic/Math.h"

namespace monapi2	{

//Color/////////////
/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
void Color::scale(float f)
{
	scale(f,f,f);
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
void Color::scale(float f1,float f2,float f3)
{
	r = (int)(f1 * r);
	g = (int)(f2 * g);
	b = (int)(f3 * b);
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
void Color::add(byte _r,byte _g,byte _b)
{
	r += _r;
	g += _g;
	b += _b;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
void Color::add(colort color)
{
	add(getRValue(color),getGValue(color),getBValue(color));
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
void Color::subtract(byte _r,byte _g,byte _b)
{
	r -= _r;
	g -= _g;
	b -= _b;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
void Color::subtract(colort color)
{
	subtract(getRValue(color),getGValue(color),getBValue(color));
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
void Color::set(byte _r,byte _g,byte _b)
{
	r = _r;
	g = _g;
	b = _b;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
void Color::set(colort color)
{
	r = getRValue(color);
	g = getGValue(color);
	b = getBValue(color);
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
void Color::normalize()
{
	putBetween(&r,0,0xFF);
	putBetween(&g,0,0xFF);
	putBetween(&b,0,0xFF);
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
colort Color::getColort() const
{
	return makeColor((byte)r,(byte)g,(byte)b);
}

}	//namespace monapi2
