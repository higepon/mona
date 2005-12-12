/**
    @file  Type.h
    @brief 基本型・typedef

    基本型・typedef

	License=Mona License
	@version $Revision$
	@date   update:$Date$
*/
//バグ修正をする時は関数本体説明の@date履歴に日付と名前と作業内容を付け足しておいてください。
//また.hファイルにあるクラス説明などの@date履歴部分にも同様の事をしておいてください。
#ifndef _MONAPI2_BASIC_TYPE_H
#define _MONAPI2_BASIC_TYPE_H

#include "switch.h"

namespace monapi2
{

/**
	@brief	型。Monapi2リファレンスも参照。
	@date	2005/08/20	junjunn 作成
*/

/**
型の考えには固定幅と可変幅とがある。
二つの型の考えの詳細についてはmonapi2リファレンスで。
*/

//可変幅。
//	typedef	signed int				int;		///<OS依存singned int
typedef	unsigned int			uint;		///<OS依存unsigned int

//固定幅。
typedef	signed char				int8;		///<1バイト幅固定のsigned
typedef	signed short			int16;		///<2バイト幅固定のsigned
typedef	signed int				int32;		///<4バイト幅固定のsigned
typedef	unsigned char			uint8;		///<1バイト幅固定のunsigned
typedef	unsigned short			uint16;		///<2バイト幅固定のunsigned
typedef	unsigned int			uint32;		///<4バイト幅固定のunsigned

//固定幅の一般的に使われる別名。
typedef	uint8					byte;		///<uint8の別名
typedef	uint16					word;		///<uint16の別名
typedef	uint32					dword;		///<uint32の別名。オフセット(アドレス)を記述する意味でのときなどによく使われる。


/**
boolなint。
なぜboolなのにintなのか？と言えばプログラムのほとんどの変数はint型なので
boolを使うよりも本当は全部をintで統一する方が
int→boolまたはbool→intへの暗黙的なキャスト演算がなくなり速くなるので。
もっともプログラム全体からすると微々たる時間だろうし
Y/Nの二択は意味的にboolで書くのが個人的に好きなので私はあまり使ってませんが・・・
*/
typedef int 					booli;


//固定幅
typedef	char					char1;			///<1バイト文字。本当はunsignedなんだけどsignedが普及しすぎててコンパイラでも混乱しやすいのでsignedにしとく。
typedef	unsigned short			char2;			///<2バイト文字
typedef	unsigned int			char4;			///<4バイト文字

typedef char1*					pchar1;			///<標準文字列へのポインタ
typedef const char1*			cpchar1;		///<const標準文字列へのポインタ
typedef char2*					pchar2;			///<2バイト文字列へのポインタ
typedef const char2*			cpchar2;		///<const2バイト文字列へのポインタ
typedef char4*					pchar4;			///<4バイト文字列へのポインタ
typedef const char4*			cpchar4;		///<const4バイト文字列へのポインタ

//可変幅
typedef	char4					charv;			///<標準文字。
typedef charv*					pcharv;			///<標準文字列へのポインタ
typedef const charv*			cpcharv;		///<const標準文字列へのポインタ

#ifndef NULL
	#define NULL 0
#endif


/**
	64ビット変数をエミュレートする。コードを書く労力の問題で必要になるまでは機能限定。

	@date	2005/09/06	junjunn 作成
*/

template<class TYPE>
class int64Base
{
public:
	word operator =(const word w)	{set(0,w);return w;}
	int operator =(const int i)		{set(0,i);return i;}
	uint operator =(const uint n)	{set(0,n);return n;}
	operator uint()	{return m_t[0];}

protected:
	void set(TYPE w1,TYPE w2)	{m_t[1] = w1;m_t[0]=w2;};
	TYPE m_t[2];
};

typedef int64Base<int> int64;
typedef int64Base<uint> uint64;

}	//namespace monapi2

#endif
