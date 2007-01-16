/**
	@file	Color.h
	@brief	色を扱う

	色を扱う

	License=Mona License
	@version $Revision$
	@date	$Date$
*/
//バグ修正をする時は関数本体説明の@date履歴に日付と名前と作業内容を付け足しておいてください。
//また.hファイルにあるクラス説明などの@date履歴部分にも同様の事をしておいてください。
#ifndef _MONAPI2_MULTIMEDIA_COLOR_H
#define _MONAPI2_MULTIMEDIA_COLOR_H

#include "../Basic/Switch.h"
#include "../Basic/Type.h"

namespace monapi2	{

typedef uint32_t colort;
typedef uint16_t color16t;

const colort COLOR_OPAQUE = 0xFF000000;			///<真っ黒

inline uint8_t getRValue(colort color)	{return (uint8_t)((color & 0x00FF0000) >> 16);}
inline uint8_t getGValue(colort color)	{return (uint8_t)((color & 0x0000FF00) >> 8);}
inline uint8_t getBValue(colort color)	{return (uint8_t)((color & 0x000000FF));}
inline uint8_t getRValue(color16t color)	{return (uint8_t)(color>>8);}		//(color>>11)*8と同じ
inline uint8_t getGValue(color16t color)	{return (uint8_t)((color >> 3) & 0x000000F8);}
inline uint8_t getBValue(color16t color)	{return (uint8_t)((color << 3) & 0x000000F8);}
inline colort makeColor(uint8_t r,uint8_t g,uint8_t b)	{return ((r<<16) + (g<<8) + (b));}

colort scaleColor(colort color,float fScale,bool bNormalize);

/**
24ビットの色値を操作するクラス。

	@date	2005/08/20	junjunn 作成
*/
class Color
{
//メソッド
public:
///@name コンストラクタ
//@{
	Color()						{clear();}			///<コンストラクタ
	Color(uint8_t r,uint8_t g,uint8_t b)	{set(r,g,b);}		///<コンストラクタ
	Color(colort color)			{set(color);}		///<コンストラクタ
	Color(color16t color16)		{set(color16);}		///<コンストラクタ
//@}

///@name 取得
//@{
	uint8_t getR() const {return (uint8_t)r;}
	uint8_t getG() const {return (uint8_t)g;}
	uint8_t getB() const {return (uint8_t)b;}
	colort getColort() const;		///<return RGB(r,g,b);
//@}

///@name 演算
//@{
	void set(uint8_t r,uint8_t g,uint8_t b);			///<セット
	void set(colort color);					///<セット
	void set(color16t color);				///<セット
	void scale(float f);					///<スケーリング
	void scale(float f1,float f2,float f3);	///<スケーリング
	void add(int r,int g,int b);			///<増減
	void add(colort color);					///<増減
	void subtract(int r,int g,int b);		///<増減
	void subtract(colort color);			///<増減
	void clear()	{set(0,0,0);}			///<クリア
	void normalize();						///<ScaleやAddの後に成分が0～0xFFの範囲にトリミングする
//@}


///メンバ
public:
	int r;		///<赤。本来はuint8_t幅だが演算している途中にオーバーフローが出ないようにintにする。
	int g;		///<緑
	int b;		///<青
};

/**
16ビットの色値クラス。

	@date	2005/09/20	junjunn 作成
*/
class ColorConvertFn
{
public:

	inline static uint16_t convert24to16(colort color)
	{
        return  (uint16_t)(((color >> 8) & 0xF800) | ((color >> 5) & 0x07E0) | ((color >> 3) & 0x001F));
	}

//	inline static uint16_t convert24to16(uint8_t r, uint8_t g, uint8_t b) {
//		return (uint16_t)(((r << 8) & 0xF800) | ((g << 3) & 0x07E0) | (b >> 3));}

//	inline static uint16_t convert24to16(uint8_t* rgb) {
//		return (uint16_t)(((rgb[2] << 8) & 0xF800) | ((rgb[1] << 3) & 0x07E0) | (rgb[0] >> 3));}
};



}		// namespace monapi2

#endif
