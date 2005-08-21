/*!
    @file  Type.h
    @brief 基本型・typedef

    基本型・typedef

	License=Mona License
    \version $Revision$
    \date   update:$Date$
*/
#ifndef _MONAPI2_TYPE_H
#define _MONAPI2_TYPE_H

namespace monapi2
{

//author junjunn

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
なぜboolなのにintなのか？と言えばboolを使うよりも本当は全部をintで統一する方が
int→boolまたはbool→intへの暗黙的なキャスト演算がなくなり速くなるので。
もっともプログラム全体からすると微々たる時間だろうし
Y/Nの二択は意味的にboolで書くのが個人的に好きなので私はあまり使ってませんが・・・
*/
typedef int 					booli;


//可変幅
typedef	unsigned char			vchar;			///<標準文字。後からtypedef uint16 vcharとかユニコードに合わせ幅が変わるかも。
typedef vchar*					pvchar;			///<標準文字列へのポインタ
typedef const vchar				pcvchar;		///<const標準文字列へのポインタ

//固定幅
typedef	unsigned char			char1;			///<1バイト文字
typedef	unsigned short			char2;			///<2バイト文字

typedef char1*					pchar1;			///<標準文字列へのポインタ
typedef const char1*			pcchar1;		///<const標準文字列へのポインタ
typedef char2*					pchar2;			///<ユニコード文字列へのポインタ
typedef const char2*			pcchar2;		///<constユニコード文字列へのポインタ


}	//namespace monapi2

#endif
