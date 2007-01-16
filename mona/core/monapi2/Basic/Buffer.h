/**
	@file	Buffer.h
	@brief	汎用バッファ

	汎用バッファ

	License=Mona License
	@version $Revision$
	@date	$Date$
*/
//バグ修正をする時は関数本体説明の@date履歴に日付と名前と作業内容を付け足しておいてください。
//また.hファイルにあるクラス説明などの@date履歴部分にも同様の事をしておいてください。
#ifndef _MONAPI2_BASIC_BUFFER_H
#define _MONAPI2_BASIC_BUFFER_H

#include "switch.h"
#include "Type.h"

namespace monapi2	{

/**
	汎用便利ヒープバッファ。基本的にはnewして領域を得て、デストラクタで自動deleteする機能をつけくわえた物。.
	必要な分だけ動的にメモリを確保し
	解放もデストラクタで自動的に行われるので非常に便利。

	@date	2005/08/20	junjunn 作成
*/
class Buffer
{
//メソッド
public:
///@name 生成
//@{
	Buffer();								///<コンストラクタ
	Buffer(int iInitialSize);				///<コンストラクタ。Set(cp,iLength);
	~Buffer();								///<デストラクタ
//@}


///@name 操作
//@{
	void copy(const uint8_t* cpSource,int iLength);	///<領域を確保しコピー。
	void copy(const Buffer* cpbuf);					///<十分な領域を確保しcpbufをコピー

	uint8_t* extendBuffer(int iNewSize,bool bKeepOriginalData=false);					///<iLengthバイトの領域を確保
	void clear();									///<内部バッファのクリア
//@}


///@name 取得
//@{
//	int getLength()			const {return m_iLength;}
	int getSize()			const {return m_iSize;}
	const	uint8_t* getData()	const {return m_pBuffer;}		//内部データにアクセス。const
			uint8_t* getBuffer()	{return m_pBuffer;}			//内部データにアクセス。
//@}

//メンバ
protected:
	uint8_t*	m_pBuffer;					///<内部バッファ
//	int		m_iLength;					///<長さ。確保している領域のサイズとは別。
	int		m_iSize;					///<確保している領域のサイズ
};

}	//namespace monapi2

#endif
