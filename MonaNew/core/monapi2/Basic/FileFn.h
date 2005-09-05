/**
	@file	FileFn.h
	@brief	ファイル一発操作

	ファイル一発操作

	License=Mona License
	@version $Revision$
	@date	$Date$
*/
//バグ修正をする時は関数本体説明の@date履歴に日付と名前と作業内容を付け足しておいてください。
//また.hファイルにあるクラス説明などの@date履歴部分にも同様の事をしておいてください。
#ifndef _MONAPI2_BASIC_FILEFN_H
#define _MONAPI2_BASIC_FILEFN_H

#include "switch.h"
#include "Type.h"

namespace monapi2	{

const int g_iMAX_PATH = 256;		///<Windows上で使える最大パス文字数

///
/**
	@brief	ファイル関数を集めたstaticクラス。Monapi2リファレンスも参照。
	@date	2005/08/20	junjunn 作成
*/
class FileFn
{
public:		//まだ未実装なので
	static bool isExist(pcchar1 cszPath);
	static bool isFile(pcchar1 cszPath);
	static int getSize(pcchar1 cszPath);
	static uint getLastModifiedTime(pcchar1 cszPath);
	static bool read(pcchar1 cszPath,byte* pBufferOut,int iBufferSize);
	static bool read(pcchar1 cszPath,class Buffer* pbufOut);
	static bool read(pcchar1 cszPath,class String* pstrOut);

	static bool write(pcchar1 cszPath,const byte* cpIn,int iCount);
	static bool write(pcchar1 cszPath,const Buffer* cpbufIn);
	static bool write(pcchar1 cszPath,const String* cpstrIn);
	static bool createDirectory(pcchar1 cszPath,pcchar1 cszDirName);
	static bool deleteFile(pcchar1 cszPath);
	static bool deleteDirectory(pcchar1 cszPath);
	static bool move(pcchar1 cszPathFrom,pcchar1 cszPathTo);
	static bool copy(pcchar1 cszPathFrom,pcchar1 cszPathTo);
};

}	//namespace monapi2

#endif
