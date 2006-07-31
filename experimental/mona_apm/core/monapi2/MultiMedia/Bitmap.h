/**
	@file	Bitmap.h
	@brief	ビットマップの取り扱い・操作。
	
	ビットマップの取り扱い・操作。

	License=Mona License
	@version $Revision$
	@date	$Date$
*/
//バグ修正をする時は関数本体説明の@date履歴に日付と名前と作業内容を付け足しておいてください。
//また.hファイルにあるクラス説明などの@date履歴部分にも同様の事をしておいてください。
#ifndef _MONAPI2_MULTIMEDIA_BITMAP_H
#define _MONAPI2_MULTIMEDIA_BITMAP_H

#include "../Basic/Switch.h"
#include "../Basic/Type.h"
#include "../Basic/Buffer.h"
#include "../Basic/String.h"
#include "Size.h"
#include "Color.h"

namespace monapi2	{

/**
ビットマップを扱うためのクラス。24ビット限定。.
ファイルなどからの読み書きとピクセル単位での色操作が可能。

	@date	2005/08/20	junjunn 作成

@bug Y反転の場合のgetPixel(x,y)処理などが未対応・・・
*/
class Bitmap
{
//メソッド
public:
///@name 生成
//@{
	Bitmap()				{init();}				///<空の内容で作成
	Bitmap(cpchar1 cszPath,bool bReverseY=false)	{init();read(cszPath,bReverseY);}	///<cszPathのファイルで作成
	Bitmap(int iWidth,int iHeight)	{createCanvas(iWidth,iHeight);}	///<大きさを指定して作成。
//@}

///@name 取得
//@{
	int getWidth()	const {return m_oSize.getWidth();}				///<幅
	int getHeight()	const {return m_oSize.getHeight();}				///<高さ
	int getLineDataSize() const {return m_iLineSize;}				///<一行のデータ幅
	const Size* getSize() const {return &m_oSize;}
//@}

///@name ビットマップ操作
//@{
	byte* createCanvas(int iWidth,int iHeight);		///<バッファにサイズが収まるだけの容量を確保。
	byte* createCanvas(const class Size* cpSize);	///<バッファにサイズが収まるだけの容量を確保。
	void copy(const Bitmap* cpBitmap);				///<cpBitmapをコピー
	void clear();									///<現在の内容を消去する
//@}

///@name ピクセル操作
//@{
	byte* getPixelBuffer();										///<データバッファの始点の位置を返す。
	byte* getPixelBuffer(int x,int y);							///<(x,y)のピクセルのデータバッファの位置を返す。
	const byte* getPixelBufferConst() const;					///<データバッファの始点の位置を返す。const版。
	const byte* getPixelBufferConst(int x,int y) const;			///<(x,y)のピクセルのデータ位置を返す。const版。
	colort getPixel(int x,int y) const;							///<(x,y)のピクセルの色位置を返す
//	class Color* getPixel(int x,int y,Color* pColorMan) const;	///<(x,y)の位置にある色データを取得
//	void setPixel(int x,int y,const Color* pColorMan);			///<(x,y)の位置に色データをセット。24ビット用。
	void setPixel(int x,int y,colort rgb);						///<(x,y)の位置に色データをセット。colort引数。
//@}

///@name ペイント操作
//@{
	void fill(colort color);
//@}

///@name ファイル操作
//@{
	bool read(cpchar1 szPath,bool bReverseY=false);		///<szPathのファイルで作成
	void write(cpchar1 cszPath)	const;				///<.bmpをディスクに書き出す
//@}




protected:
	void init();									///<初期化
	byte* getNewBuffer(int iWidth,int iHeight);		///<引数のサイズのバッファを確保
	void setLineBits();								///<m_iLineSizeをセット
	bool isValid()const	{return	(m_oSize.getWidth()!=0);}		///<正しいバッファが存在しているか


//メンバ
public:
	Size	m_oSize;
	int		m_iLineSize;	///<1行のデータサイズ。BMPの幅は4の倍数でなければならない条件のためm_iWidthが4の倍数じゃなかった時に実際のバイト幅とは食い違う。
	Buffer	m_bufferData;	///<データ本体

	bool	m_bYReverse;	///<Y反転をしているか。

//junjunn 2005/09/22。Bitmapを共有メモリ上に作るとここで一緒に作られるStringの
//参照カウントシステムの関係でメモリが破綻する事に気づいたので一端停止・・・
//	String	m_strPath;		///<自分のパス。
};




//関数////////

/**
BMPの一行のバイト数を返す。BMPの4バイト問題により必要。

	@date	2005/08/20	junjunn 作成
*/
int getBMPLineSize(int iWidth);

}		// namespace monapi2

#endif
