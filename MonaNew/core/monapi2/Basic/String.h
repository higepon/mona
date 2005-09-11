/**
	@file	String.h
	@brief	汎用文字列クラス

汎用文字列クラス。.

データは内部に直接持つのではなくStringData* m_pStringDataと言う形で別の所への参照で保持する。
これは無駄なコピーを防ぐため。
例えば
	String a="ABC";
	String b=a;
としたとき参照なしの生データだとaにもbにも"ABC"が二重に存在しメモリ効率が悪く
またコピーも行われる際にCPUが食われる。
さらに値による受け渡しが行われれば全ての中途経過にコピーが作成されては捨てられ恐ろしく不効率。
(関数でポインタ参照を受け渡しするようにすれば普通は値の受け渡しはしないで済むのだが
StringListの中で使われるStringなどでは値による受け渡しを使って実装しないと不可能。)

参照有り方式だとこの場合bはaのデータを参照するだけなのでコピーはしていなく負荷がほとんどない。
aのデータを変更する際にはその時に初めてaのデータのコピーを作りbから独立されるように作っているので
aの変更はbに影響を及ばさなくなる。

データはこの方法で別の所への参照にして参照カウントで管理しガベージコレクションみたいに扱うと
値受け渡しの際にも無駄なコピーがなくなり一気に効率よくなる。

	License=Mona License
	@version $Revision$
	@date	$Date$
*/
//バグ修正をする時は関数本体説明の@date履歴に日付と名前と作業内容を付け足しておいてください。
//また.hファイルにあるクラス説明などの@date履歴部分にも同様の事をしておいてください。
#ifndef _MONAPI2_BASIC_STRING_H
#define _MONAPI2_BASIC_STRING_H

#include "Switch.h"
#include "Type.h"
#include "VariableArgument.h"

namespace monapi2
{

/**
	@brief	class Stringが参照する文字列実データ。
	@date	2005/08/20	junjunn 作成
*/
class StringData
{
public:
///コンストラクタ
	StringData();
///参照カウントアップ。この変数を参照していると教える。
	void registerRef();
///参照カウントダウン。この変数を参照し終わったと教える。参照カウントが0になったら消滅。
	void unregisterRef();

///どこかのStringが自分以外にこれを参照しているか。
///参照されていなければ(! Locked())自由に変更可能だが
///参照されていれば(Locked())変更には新しいバッファを作る必要がある。
	bool isLocked()		{return getRefCount()>1;}


protected:
	int getRefCount()	{return m_iRefCount;}

public:
	pchar1	m_szData;		///<データ本体
	uint	m_nLength;		///<文字列が使用している大きさ。'\0'は含めていない。よって本当はこれ+1確保してある。
	uint	m_nSize;		///<バッファが確保している大きさ。
	int		m_iRefCount;	///<参照カウント

/*
m_nSizeとm_nLengthの何で二つの長さがあるかややこしいっぽいが
m_nLengthは'\0'までの文字列の長さでm_nSizeは確保しているメモリの大きさ。

要求された文字列の長さ分だけきっちりnewして確保すると後で1文字でも+=が来るたびに
deleteしてnewしての繰り返しで効率が悪い。
よってバッファ領域は要求された分よりは多少大きめにとっておいた方がいい。
だから文字列長と領域長の二つの値が別々にある。

具体的にはここでは1,2,4,8,16,32・・・と倍々で増やして
要求されたサイズを超える一番近い数を使っている。

例えばCStirng str = "ABCDEFGHI"と来たときは必要な長さは9+1バイト(+1は'\0'の分)。
この時strの内部では16バイトのバッファを確保しその中に"ABCDEFGHI"を格納する。
よってm_nLength=9、m_nSize=16になってる。

余った7バイトは文字列が後から足される時に使われる。
*/
};


/**
	@brief	汎用文字列クラス。Monapi2リファレンスも参照。
	@date	2005/08/20	junjunn 作成
*/
class String
{
public:
///コンストラクタとデストラクタ
	String();									///<コンストラクタ
	String(int iInitialBufferLength);			///<コンストラクタ
	String(cpchar1 cszInitialString);			///<コンストラクタ
	String(const String& refInitialString);		///<コンストラクタ
	~String();									///<デストラクタ

///取得
	cpchar1	getString()	const{return m_pStringData->m_szData;}			///<文字列を得る。
			operator cpchar1()	const{return getString();}				///<キャスト演算子。
	uint	getLength()	const{return m_pStringData->m_nLength;}			///<文字列の長さを得る('\0'は含まない)。
	uint	getSize()	const{return m_pStringData->m_nSize;}			///<文字列の領域サイズを得る。
	bool	isEmpty()	const{return getLength()==0;}		///<文字列が空かどうか
	bool	isEqual(cpchar1 csz) const;						///<同一かどうか比較。
	bool	operator==(cpchar1 csz) const;					///<同一かどうか比較。
	bool	operator!=(cpchar1 csz) const;					///<非同一かどうか比較。
	bool	isEqualNoCase(cpchar1 csz) const;				///<同一かどうか比較。大小文字無視。
	char1	getAt(uint n) const;							///<指定のインデックスの文字を返す。
	int		find(char1 cFind,uint nStart=0) const;			///<検索。
	int		find(cpchar1 cszFind,uint nStart=0) const;		///<検索。
	int		findReverse(char1 cFind) const;					///<逆検索。
	int		findReverse(cpchar1 cszFind) const;				///<逆検索。
	void	getLeft(String* pstrOut,uint nCount) const;		///<左の数文字から新しいStringを構築。
	void	getRight(String* pstrOut,uint nCount) const;	///<右の数文字から新しいStringを構築。
	void	getMiddle(String* pstrOut,uint nStart,uint nCount) const;	///<中央の数文字から新しいStringを構築。

	String	join(cpchar1 csz) const;						///<連結して新しいStringを構築。
	String	operator+(cpchar1 csz) const;					///<連結して新しいStringを構築。

//操作
	void copy(cpchar1 csz,int iCount=-1);					///<代入。
	String& operator=(cpchar1 csz);							///<代入。
	void copy(const String* pstr);							///<代入。
	String& operator=(const String& rstr);					///<代入。
	void joinSelf(cpchar1 csz);								///<自分に直接連結。
	String& operator+=(cpchar1 csz);						///<自分に直接連結。
	void empty();											///<空にする。
	void format(cpchar1 cszFormat,...);						///<sprintf形式で文字列を作る。
	void formatJoinSelf(cpchar1 cszFormat,...);				///<sprintf形式で文字列を作って最後にくっつける。
	bool setAt(uint n,char1 c);								///<指定のインデックスに文字をセット。
	int replace(char1 cFrom,char1 cTo);						///<置き換え
	int replace(pchar1 cszFrom,pchar1 cszTo);				///<置き換え
	int remove(char1 cRemove);								///<消去
	int remove(pchar1 cszRemove);							///<消去
	void insert(pchar1 cszIn,uint nIndex);					///<挿入
	void cut(uint nIndex,uint nCount);						///<削除
	void toUpper();											///<大文字変換
	void toLower();											///<小文字変換
	pchar1 getBuffer();										///<内部バッファを直接得る。
	pchar1 extendBuffer(uint nLength);						///<内部バッファを拡張。ポインタも得る。
	void recalculateLength(int iLength=-1);					///<getBuffer,extendBufferでいじられた内部文字列の長さを再計算。


protected:
///初期化
	void init();
///formatなどの内部処理。
	void formatV(cpchar1 cszFormat,vapointer vl);
///nLengthは格納可能か。
	bool isContainable(uint nLength);
///長さをセットする
	void setLength(uint n)		{m_pStringData->m_nLength=n;}
///自分の参照していたm_pStringDataを解放。
	void releaseBuffer();
///getBufferはユーザーが呼び出す、バッファのコピーを作りながらバッファへのアクセスを得るメソッド。
///こっちは直接アドレスを呼び出す。名前が似てるだけで用途は違うと思った方がいい。
	pchar1 getBufferDirect()	{return m_pStringData->m_szData;}
///参照へのアクセス。
	StringData* getStringData()	const {return m_pStringData;}


//内部バッファ操作

/**
バッファの内容を今から書き換えるので書き換え可能な領域を要求する。
参照カウントが自分以外になく、かつサイズが十分あるなら自分のバッファをそのまま利用するし、
他からも参照されてロックされているなら新しい領域を作る。
*/
	pchar1 requestModifiableBuffer(uint nLength,bool bKeepOriginalData);

/**
最低限nMinLengthのバッファサイズを確保する。
バッファのサイズが足りないなら拡張するし、すでにnMinLengthを上回っているなら何もしない。
バッファの大きさを考慮して次にcreateNewBufferを呼び出す。
@memo 実装はまだ完璧ではない。先読みのため必要以上の領域を得る機能が未完成。

extendBufferと名前が似ているがあっちはユーザーが呼び出す、
バッファのコピーを作りながらバッファへのアクセスを得るメソッド。
こっちは直接操作する。名前が似てるだけで用途は違うと思った方がいい。
*/
	pchar1 extendBufferDirect(uint nMinLength,bool bKeepOriginalData);

/**
BufferLenの長さを確保する。iBufferLenは'\0'を含まない文字列の長さ。
createNewBuffer()で勝手に'\0'を足す処理してくれる。

これらの関数の流れとしては
	requestModifiableBuffer
	↓	(現在のバッファが他から参照されていなく直接使えるか、
	↓　 それとも参照されているので新しく作るべきか判断。)
	↓
	extendBufferDirect
	↓	(要求される長さより先読みのため多めに計算する。スキップ可。)
	↓
	createNewBuffer
		(実際に指定された大きさを取る)
*/
	pchar1 createNewBuffer(int iSizeWithoutNull,bool bKeepOriginalData=false);



//メンバ
protected:
/**
可変引数時の扱いについて。

普段はoperator cpchar1()が効くので
	cpchar1 p = string;
とか書いても自動で正しくStringからcpchar1へのキャストが効き正しい型に変換されますが
可変引数使用時には型の情報が消滅しますから
	StringFn::format(szBuffer,"%s",string->getString());
と明示的に指定してください。
	StringFn::format(szBuffer,"%s",string);
ではバグります。

MFCのCString型には可変引数時に
	sprintf(szBuffer,"%s",string);
のように書いても正しくchar*文字列として出力される機能が実装されています。

これは多分CString本体はconst char* m_pだけをメンバに持ち普通の文字列と見た目は全く同一に、
CStringのその他本体データは(m_pが指す普通のNUL文字列 + 本体メンバ情報)のように
文字列の終わりにメンバデータをくっつけ、参照するときは
（m_p + m_pの長さ）で調べるようなかなり変則的な置き方をしているからだと思われます。

ここでも実装できない事はないですが正直コード的にはかなりの反則で
可変引数時の自動キャスト成功の為だけにそんな反則する価値があるのか・・・って話で
とりあえず現時点ではあえて実装していません。

ちなみにこのクラスにもう一つメンバcpchar1 m_szDataPointerをつけてそれにm_pStringData->m_szData
を設定しておく方法にすればStirngの生データが文字列データを指すようになるのでキャストは成功しますが、
可変引数の方のVAP_ADVANCE(p,sizeof(char*))などが失敗するので結局無理です・・・
*/

///データ本体
	class StringData* m_pStringData;
};

}	//namespace monapi2

#endif
