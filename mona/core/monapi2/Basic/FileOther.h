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
#ifndef _MONAPI2_BASIC_FILEOTHER_H
#define _MONAPI2_BASIC_FILEOTHER_H

#include "switch.h"
#include "Type.h"
#include "FileFn.h"
#include "String.h"
#include "CollectionArray.h"

namespace monapi2	{

/**
"C:/aaa/bbb/ccc/ddd.txt"などを"C:/aaa/bbb/ccc"と"ddd.txt"に分ける。.

@par 使い方：
@code
基本的な使い方：

//構築
	CSplitPath SplitPath("C:/aaa/bbb/ccc/ddd.txt");


//使用
	cpchar1 cszDir		= SplitPath.GetDir();		//=="C:/aaa/bbb/ccc"
	cpchar1 cszFilename	= SplitPath.GetFilename();	//=="ddd.txt"
@endcode

	@date	2005/08/20	junjunn 作成
*/

class SplitPath
{
//メソッド
public:
///@name 生成
//@{
	SplitPath()					{init();}					///<コンストラクタ
	SplitPath(cpchar1 cszPath)	{set(cszPath);}				///<コンストラクタ
	void init();											///<初期化
//@}


///@name セット
//@{
	void set(cpchar1 cszPath);								///<セット
//@}

///@name 取得
//@{
	cpchar1 getDirectory()	const {return m_szDirectory;}	///<ディレクトリ部分を返す
	cpchar1 getFilename()	const {return m_szFilename;}	///<ファイル名部分を返す
//@}


//メンバ
protected:
	char1 m_szDirectory[g_iMAX_PATH];								///<ディレクトリ部分
	char1 m_szFilename[g_iMAX_PATH];							///<ファイル名部分
};


/**
"test.txt"などを"test"と"txt"に分ける。.

@par 使い方：
@code
基本的な使い方：

//構築
	CSplitFilename SplitFilename("test.txt");


//使用
	cpchar1 cszDir		= SplitFilename.GetName();		//=="test"
	cpchar1 cszFilename	= SplitFilename.GetExtension();	//=="txt"
@endcode

	@date	2005/08/20	junjunn 作成
*/
class SplitFilename
{
//メソッド
public:
///@name 生成
//@{
	SplitFilename()						{init();}				///<コンストラクタ
	SplitFilename(cpchar1 cszFilename)	{set(cszFilename);}		///<コンストラクタ
	void init();												///<初期化。
//@}


///@name 取得
//@{
	cpchar1 getName()		const {return m_szName;}			///<名前部分を返す
	cpchar1 getExtension()	const {return m_szExtension;}		///<拡張子部分を返す
//@}

//操作
	void set(cpchar1 cszFilename);								///<セット

//メンバ
protected:
	char1 m_szName[g_iMAX_PATH];								///<名前部分
	char1 m_szExtension[g_iMAX_PATH];							///<拡張子部分
};



/**
	@brief	ディレクトリをスキャンして中身を列挙。Monapi2リファレンスも参照。
	@date	2005/08/20	junjunn 作成
*/

//単一のファイルの
class FileProperty
{
public:
	cpchar1 getName()	const {return m_strName;}
	cpchar1 getPath()	const {return m_strPath;}
	int getSize()		const {return m_iSize;}
	bool isDirectory()	const {return m_bDirectory;}


public:
	String m_strName;			///<名前
	String m_strPath;			///<パス
	bool m_bDirectory;			///<trueならディレクトリ、falseならファイル。
	int m_iSize;				///<サイズ
};

class ScanDirectory
{
//ソートは未インプリメント
	enum ESortBy
	{
		SORTBY_NONE,
		SORTBY_NAME,
		SORTBY_EXTENSION,
		SORTBY_LASTMODIFIEDTIME,
	};

public:
	ScanDirectory(cpchar1 cszPath,bool bIncludeFile,bool bIncludeDirectory,enum ESortBy eSortBy=SORTBY_NONE);
	ScanDirectory()	{init();}

	cpchar1 getNameAt(int iIndex) const;
	cpchar1 getPathAt(int iIndex) const;
	FileProperty* getFilePropertyAt(int iIndex);
	int getCount()						{return m_iCount;}

	void scan(cpchar1 cszPath,bool bIncludeFile,bool bIncludeDirectory,enum ESortBy eSortBy=SORTBY_NONE);


protected:
	void init();
	bool assertIndex(int iIndex) const	{return (iIndex>=0 && iIndex<m_iCount);}


protected:
	int m_iCount;
	String m_strPathBase;			///<現在探索しているディレクトリのパス。

	ArrayAD<FileProperty*> m_oArrayPFileProperty;
};

}	//namespace monapi2

#endif
