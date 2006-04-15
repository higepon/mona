/**
	@file	FileOther.h
	@brief	ファイルユーティリティ

	ファイル操作・ファイル名操作・ファイルユーティリティ

	License=Mona License
	@version $Revision$
	@date	$Date$
*/
//バグ修正をする時は関数本体説明の@date履歴に日付と名前と作業内容を付け足しておいてください。
//また.hファイルにあるクラス説明などの@date履歴部分にも同様の事をしておいてください。
#ifdef MONA
	#include <monapi.h>
	#include <monapi/messages.h>
#endif

#include "Math.h"
#include "FileOther.h"
#include "StringFn.h"

namespace monapi2	{

//SplitPath//////////////
/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
void SplitPath::init()
{
	StringFn::empty(m_szDirectory);
	StringFn::empty(m_szFilename);
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
void SplitPath::set(cpchar1 cszPath)
{
	init();

//一番右の区切りを検索
	cpchar1 pDivYen = StringFn::findReverse(cszPath,'\\');
//こっちでも検索
	cpchar1 pDivSlash = StringFn::findReverse(cszPath,'/');

	cpchar1 pDiv = getMax(pDivYen,pDivSlash);

//普通に見つかったら
	if (pDiv)
	{
		StringFn::copy(m_szDirectory,cszPath,pDiv - cszPath);
		StringFn::copy(m_szFilename	,pDiv+1);
	}
	else
	{
//見つからなかったらcszがファイル名かディレクトリ名かどちらかを推測する。
		pchar1 pDestination = StringFn::find(cszPath,'.')?m_szFilename:m_szDirectory;
		StringFn::copy(pDestination,cszPath);
	}
}

//SplitFilename//////////
/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
void SplitFilename::init()
{
	StringFn::empty(m_szName);
	StringFn::empty(m_szExtension);
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
void SplitFilename::set(cpchar1 cszFilename)
{
//名前と拡張子に分ける
	cpchar1 pDiv = StringFn::findReverse(cszFilename,'.');

	if (pDiv)
	{
		int iLengthLeft = pDiv - cszFilename;
		StringFn::copy(m_szName,cszFilename,iLengthLeft);
		StringFn::copy(m_szExtension,pDiv + 1);

#ifdef _WIN32
//リンクショートカットの場合はaaa.txt.lnkのような形式なのでaaa.txtに分解できるようもう一段階調べる
		if (StringFn::isEqual(m_szExtension,"lnk"))
		{
			char1 szFilenameCp[g_iMAX_PATH];
			StringFn::copy(szFilenameCp,cszFilename);		//コピーを作り
			*(szFilenameCp + iLengthLeft)='\0';				//.の場所に\0をセットし

			SplitFilename SF2(szFilenameCp);				//もう一回挑戦
			StringFn::copy(m_szName		,SF2.getName());
			StringFn::copy(m_szExtension,SF2.getExtension());
		}
#endif
	}
//丸ごと名前にする。
	else
	{
		StringFn::copy(m_szName,cszFilename);
	}
}

#ifdef MONA
//ScanDirectory///////
/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
ScanDirectory::ScanDirectory(cpchar1 cszPath,bool includeFile,bool includeDirectory,ESortBy eSortBy)
{
	scan(cszPath,includeFile,includeDirectory,eSortBy);
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
void ScanDirectory::init()
{
	m_iCount = 0;
	m_oArrayPFileProperty.removeAll();
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/10/12	junjunn 作成
*/
void ScanDirectory::scan(cpchar1 cszPath,bool bIncludeFile,bool bIncludeDirectory,ESortBy /*eSortBy*/)
{
	init();
	m_strPathBase = cszPath;

	monapi_cmemoryinfo* pMemoryInfo = monapi_call_file_read_directory(cszPath,MONAPI_FALSE);

	if (! pMemoryInfo)		return;
	int iCount = *(int*)pMemoryInfo->Data;
	if (iCount == 0)		return;

	monapi_directoryinfo* pDirectoryInfo = (monapi_directoryinfo*)(pMemoryInfo->Data + sizeof(int));
	for (int i=0;i<iCount;i++) 
	{
		cpchar1 cszName = pDirectoryInfo->name;
		if (!StringFn::isEqual(cszName,".") &&	!StringFn::isEqual(cszName,".."))
		{
			bool bDirectory = (pDirectoryInfo->attr & ATTRIBUTE_DIRECTORY);

			if (
				(bIncludeFile && !bDirectory) ||
				(bIncludeDirectory && bDirectory)
				)
			{
				FileProperty* pFileProperty = new FileProperty;
				pFileProperty->m_strName	= cszName;
				pFileProperty->m_bDirectory = bDirectory;
				pFileProperty->m_iSize		= pDirectoryInfo->size;

				if (m_strPathBase.isEqual("/"))
					pFileProperty->m_strPath.format("/%s",cszName);
				else
					pFileProperty->m_strPath.format("%s/%s",m_strPathBase.getString(),cszName);

				m_oArrayPFileProperty.add(pFileProperty);
				m_iCount++;
			}
		}
		pDirectoryInfo++;
	}
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/10/12	junjunn 作成
*/
cpchar1 ScanDirectory::getNameAt(int iIndex) const
{
	if (! assertIndex(iIndex)) return NULL;

	return m_oArrayPFileProperty.getAt(iIndex)->getName();
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
cpchar1 ScanDirectory::getPathAt(int iIndex) const
{
	if (! assertIndex(iIndex)) return NULL;

	return m_oArrayPFileProperty.getAt(iIndex)->getPath();
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/10/12	junjunn 作成
*/
FileProperty* ScanDirectory::getFilePropertyAt(int iIndex)
{
	if (! assertIndex(iIndex)) return NULL;

	return m_oArrayPFileProperty.getAt(iIndex);
}

#endif	//ifdef MONA
}		//namespace monapi2
