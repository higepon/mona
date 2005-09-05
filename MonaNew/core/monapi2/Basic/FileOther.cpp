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
void SplitPath::set(pcchar1 cszPath)
{
	init();

//一番右の区切りを検索
	pcchar1 pDivYen = StringFn::findReverse(cszPath,'\\');
//こっちでも検索
	pcchar1 pDivSlash = StringFn::findReverse(cszPath,'/');

	pcchar1 pDiv = getMax(pDivYen,pDivSlash);

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
void SplitFilename::set(pcchar1 cszFilename)
{
//名前と拡張子に分ける
	pcchar1 pDiv = StringFn::findReverse(cszFilename,'.');

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

//ScanDirectory///////
/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
ScanDirectory::ScanDirectory(pcchar1 cszPath,bool includeFile,bool includeDirectory,ESortBy eSortBy)
{
	scan(cszPath,includeFile,includeDirectory,eSortBy);
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
ScanDirectory::ScanDirectory()
{
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
pcchar1 ScanDirectory::getNameAt(int iIndex)
{
	iIndex=0;
	return NULL;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
pcchar1 ScanDirectory::getFullPathAt(int iIndex)
{
	iIndex=0;
	return NULL;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
pcchar1 ScanDirectory::getFilePropertyAt(int iIndex)
{
	iIndex=0;
	return NULL;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
int ScanDirectory::getCount()
{
	return 0;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
void ScanDirectory::scan(pcchar1 cszPath,bool bIncludeFile,bool bIncludeDirectory,ESortBy eSortBy)
{
	cszPath=NULL;
	bIncludeFile=false;
	bIncludeDirectory=false;
	eSortBy=SORTBY_NAME;
}


}		//namespace monapi2
