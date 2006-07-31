/**
	@file	Limit.h
	@brief	intやfloatなどの最小・最大の大きさなど型の情報。

	intやfloatなどの最小・最大の大きさなど型の情報。

	License=Mona License
	@version $Revision$
	@date	$Date$
*/
//バグ修正をする時は関数本体説明の@date履歴に日付と名前と作業内容を付け足しておいてください。
//また.hファイルにあるクラス説明などの@date履歴部分にも同様の事をしておいてください。
#ifndef _MONAPI2_BASIC_LIMIT_H
#define _MONAPI2_BASIC_LIMIT_H

/*
議論：#defineを使うべきかenumを使うべきかconst int,floatを使うべきか。

オブジェクト指向的にはconstであるべきです。
#defineやenumは単純な文字列のコピーなのでソースの各地に数字のコピーをまき散らします。

例えば
>	#define PI 3.1415;
>	float f1=PI;
>	float f2=PI;
としたときは実際に展開したときは

>	float f1=3.1415;
>	float f2=3.1415;
となりソース中に"3.1415"は2回現れそれはお互いに関係のないただの数字のコピーです。
本来のi1=i2=Aと言う図式は失われたまたま数字的に一致しているだけです。

その点constを使い
>	const float PI = 3.1415;
>	float f1=PI;
>	float f2=PI;
とした場合は展開しても

>	float f1=PI;
>	float f2=PI;
const Aの数値をその場でコンパイラが問い合わせf1,f2に代入しています。
よってこちらはPIの存在はconst float PI一箇所で1つの物が変数PIを表している
オブジェクト的に正しい構図です。


意味がわかりにくかったらchar*に置き換えてみるとわかるかもしれません・・・

>	#define PI "3.1415"
>	const char* A=PI;
>	const char* B=PI;
とした場合A!=Bです。なぜなら展開後には
>	const char* A="3.1415";
>	const char* B="3.1415";
となりそれぞれの"3.1415"は別のアドレスに別々に作られた文字列データだからです。

しかしconstを使うと
>	const char* PI = "3.1415";
>	const char* A=PI;
>	const char* B=PI;
とした場合A==Bです。なぜならAもBも同じconst char* PIへのアドレスを指すからです。

よってオブジェクト指向的には#defineを使うよりもconstが正しい考え方です。


スピード的には数字埋め込みの#defineより数字問い合わせのconstの方が時間がかかるので不利ですが。
*/

namespace monapi2
{
/**
	@brief	定数。Monapi2リファレンスも参照。
	@date	2005/08/20	junjunn 作成
*/

//整数系
///charのビット幅
	const int _CHAR_BIT = 8;

///signed charの最大と最小値
	const signed char _SCHAR_MIN = -127;
	const signed char _SCHAR_MAX =	127;

///unsigned charの最大値
	const unsigned char _UCHAR_MAX = 255;

///デフォルトのcharの最小と最大値(コンパイラオプションでsingedかunsignedか切り替えられたときに）
	const char _CHAR_MIN = _SCHAR_MIN;
	const char _CHAR_MAX = _SCHAR_MAX;

///signed intの最大と最小値
	const int _INT_MIN =-2147483647;
	const int _INT_MAX = 2147483647;

///unsigned intの最大値
	const unsigned int _UINT_MAX= 4294967295U;

///signed longの最大と最小値
	const long _LONG_MIN	= -2147483647;
	const long _LONG_MAX	= 2147483647;
///unsigned longの最大値
	const unsigned long _ULONG_MAX= 4294967295U;


//浮動小数点型
//切り上げ・切り捨て・四捨五入の丸め単位。この数の倍数で一番近いintに丸められる。
	const int _FLOAT_ROUNDS		= 1;
//不動小数点型の内部表現に使用されている基数。float f=FLOAT_RADIX^nと言う形で内部では記憶されている。
	const int _FLOAT_RADIX		= 2;

	const int _FLOAT_MANT_DIG	= 24;
	const int _DOUBLE_MANT_DIG	= 53;
	const int _LDOUBLE_MANT_DIG = 53;
	
	const int _FLOAT_DIG	= 6;
	const int _DOUBLE_DIG	= 15;
	const int _LDOUBLE_DIG	= 15;
//}//

	const float _FLOAT_EPSILON	= 1.192092896e-07F;
	const float _DOUBLE_EPSILON	= 2.2204460492503131e-016;
	const float _LDOUBLE_EPSILON= 2.2204460492503131e-016;

	const int _FLOAT_MIN_EXP	= -125;
	const int _DOUBLE_MIN_EXP	= -1021;
	const int _LDOUBLE_MIN_EXP	= -1021;

	const int _FLOAT_MAX_EXP	= 128;
	const int _DOUBLE_MAX_EXP	= 1024;
	const int _LDOUBLE_MAX_EXP	= 1024;

	const int _FLOAT_MIN_10_EXP		= -37;
	const int _DOUBLE_MIN_10_EXP	= -307;
	const int _LDOUBLE_MIN_10_EXP	= -307;

	const int _FLOAT_MAX_10_EXP		= 38;
	const int _DOUBLE_MAX_10_EXP	= 308;
	const int _LDOUBLE_MAX_10_EXP	= 308;

	const float _FLOAT_MIN			= 1.175494351e-38F;
	const double _DOUBLE_MIN		= 2.2250738585072014e-308;
	const long double _LDOUBLE_MIN	= 2.2250738585072014e-308;

	const float _FLOAT_MAX			= 3.402823466e+38F;
	const double _DOUBLE_MAX		= 1.7976931348623158e+308;
	const long double _LDOUBLE_MAX	= 1.7976931348623158e+308;

}	//namespace monapi2

#endif
