/**
	@file	File.h
	@brief	ファイル詳細操作

	ファイル詳細操作

	License=Mona License
	@version $Revision$
	@date	$Date$
*/
//バグ修正をする時は関数本体説明の@date履歴に日付と名前と作業内容を付け足しておいてください。
//また.hファイルにあるクラス説明などの@date履歴部分にも同様の事をしておいてください。
#ifndef _MONAPI2_BASIC_FILE_H
#define _MONAPI2_BASIC_FILE_H

#include "switch.h"
#include "Type.h"

#ifndef MONA
	#include <stdio.h>
#endif

namespace monapi2	{

/**
	@brief	詳細なファイル操作。Monapi2リファレンスも参照。
	@date	2005/08/20	junjunn 作成
*/
class File
{
public:
//コンストラクタ
	File()	{init();}
	File(const char* cszPath,bool bAllowWrite=false)	{init();open(cszPath,bAllowWrite);}
	~File()	{close();}

//取得
	const char* getName() const;
	const char* getPath() const;
	int getSize() const;
	int getLastModifiedTime() const;

//操作
	bool open(const char* cszPath,bool bAllowWrite=false);
	uint read(byte* pOut,uint nIndex,uint nCount) const;
	uint write(const byte* cpIn,uint nIndex,uint nCount) const;
	void close();

	void setPath(const char cszPath);


protected:
	void init() {
#ifndef MONA
		m_pFile=NULL;
#endif
	}


//応急処置
#ifndef MONA
	FILE* m_pFile;
#endif
};

}	//namespace monapi2

#endif
