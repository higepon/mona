/**
	@file	IniManager.h
	@brief	.ini形式のデータのファイルへの入出力

	.ini形式のデータのファイルへの入出力

	License=Mona License
	@version $Revision$
	@date	$Date$
*/
//バグ修正をする時は関数本体説明の@date履歴に日付と名前と作業内容を付け足しておいてください。
//また.hファイルにあるクラス説明などの@date履歴部分にも同様の事をしておいてください。
#ifndef _MONAPI2_OS_INIMANAGER_H
#define _MONAPI2_OS_INIMANAGER_H

#include "../Basic/switch.h"
#include "../Basic/Type.h"
#include "../Basic/CollectionMap.h"

namespace monapi2	{

typedef StringMap<String> StringStringMap;

/**
	@brief	.ini形式のスクリプト。 キー名と値のペアで入出力を行う。Monapi2リファレンスも参照。
	@date	2005/08/20	junjunn 作成
*/
class IniManager
{
public:
	IniManager(pcchar1 cszPath)	{init();read(cszPath);}		///<コンストラクタ
	IniManager()				{init();}					///<コンストラクタ
	void init()					{m_strstrmap.initHashTable(101);}

	void read(pcchar1 cszPath);						///<ファイルから読み込む
	void write(pcchar1 cszPath);					///<ファイルに書き込む
	void toString(String* pstrOut);					///<内部データを文字列形式にする。


//取得
///@name 値をゲット
//@{
	bool	lookup(pcchar1 cszDir,pcchar1 cszKey,bool bDefault);
	int		lookup(pcchar1 cszDir,pcchar1 cszKey,int iDefault);
	float	lookup(pcchar1 cszDir,pcchar1 cszKey,float fDefault);
	pcchar1	lookup(pcchar1 cszDir,pcchar1 cszKey,pcchar1 cszDefault);

//@}

///@name 値をセット
//@{
	void	setAt(pcchar1 cszDir,pcchar1 cszKey	,bool bValue);
	void	setAt(pcchar1 cszDir,pcchar1 cszKey	,int iValue);
	void	setAt(pcchar1 cszDir,pcchar1 cszKey	,float fValue);
	void	setAt(pcchar1 cszDir,pcchar1 cszKey	,pcchar1 cszValue);
//@}

	const	StringStringMap* getMap(){return &m_strstrmap;}		///<全ての要素を巡回する時のための直接呼び出し

protected:
	void	parse(pcchar1 cszContent);	///<szContentを解析し、含まれている"キー - 値"を取り出す
	bool	isLineComment(pcchar1 p);


//メンバ
protected:
	StringStringMap		m_strstrmap;			///<キー名とその値が結びつけられる
	String				m_strCurDir;				///<現在のディレクトリ

//キーと値をLookUpする時に使うバッファ。GetStrの値を返せるようにオブジェクトとして持っておく。
	String				m_strValueBuffer;
};

/**
CScriptIniで値の名前とキー名を一緒にするためのマクロ。入出力の相対性もサポート

	@date	2005/08/20	junjunn 作成
*/
#define INIMANAGER_LOOKUP2(iniManager,dir,value,defaultValue)	\
	value = (iniManager).lookup(dir,#value,defaultValue);

#define INIMANAGER_SETAT2(iniManager,dir,value)	\
	(iniManager).setAt(dir,#value,value);

#define INIMANAGER_LOOKUPSETAT2(iniManager,save,dir,value,defaultValue)	\
	if (save)	{INIMANAGER_SETAT2(iniManager,dir,value)}				\
	else		{INIMANAGER_LOOKUP2(iniManager,dir,value,defaultValue)}

}	//namespace monapi2

#endif
