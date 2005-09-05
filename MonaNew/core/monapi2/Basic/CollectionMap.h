/**
	@file	CollectionMap.h
	@brief	コレクション マップ

	コレクション マップ

	License=Mona License
	@version $Revision$
	@date	$Date$
*/
//バグ修正をする時は関数本体説明の@date履歴に日付と名前と作業内容を付け足しておいてください。
//また.hファイルにあるクラス説明などの@date履歴部分にも同様の事をしておいてください。
#ifndef _MONAPI2_BASIC_COLLECTIONMAP_H
#define _MONAPI2_BASIC_COLLECTIONMAP_H

#include "switch.h"
#include "Type.h"
#include "String.h"

#include "CollectionList.h"

namespace monapi2	{

//マップ系///////////////
/**
ハッシュ関数

もしかしたらこれよりも性能いいハッシュ構成関数を誰かが作るかも知れないので
ここでは標準を制定するような事はしないでgetHash1とバリエーションを作れそうな名前をつける事にした。

	@date	2005/08/20	junjunn 作成
*/
uint getHash1(pcchar1 cszKey,uint nHashTableSize);
uint getHash1(int iKey,uint nHashTableSize);

/**
マップ系に使用するハッシュテーブルの大きさ。
	@date	2005/08/20	junjunn 作成
*/
enum EPrimeNumber
{
	PRIME_ORDER_10			= 11,			///<約10
	PRIME_ORDER_12			= 13,
	PRIME_ORDER_14			= 17,
	PRIME_ORDER_17			= 17,
	PRIME_ORDER_20			= 23,
	PRIME_ORDER_24			= 29,
	PRIME_ORDER_29			= 31,
	PRIME_ORDER_35			= 37,
	PRIME_ORDER_42			= 43,
	PRIME_ORDER_51			= 53,
	PRIME_ORDER_61			= 67,
	PRIME_ORDER_74			= 79,
	PRIME_ORDER_89			= 97,

	PRIME_ORDER_100			= 101,			///<約100
	PRIME_ORDER_120			= 127,
	PRIME_ORDER_144			= 149,
	PRIME_ORDER_172			= 173,
	PRIME_ORDER_207			= 211,
	PRIME_ORDER_248			= 251,
	PRIME_ORDER_298			= 307,
	PRIME_ORDER_358			= 359,
	PRIME_ORDER_429			= 431,
	PRIME_ORDER_515			= 521,
	PRIME_ORDER_619			= 631,
	PRIME_ORDER_743			= 751,
	PRIME_ORDER_891			= 907,

	PRIME_ORDER_1000		= 1009,			///<約1K
	PRIME_ORDER_1200		= 1201,
	PRIME_ORDER_1440		= 1447,
	PRIME_ORDER_1728		= 1733,
	PRIME_ORDER_2073		= 2081,
	PRIME_ORDER_2488		= 2503,
	PRIME_ORDER_2985		= 2999,
	PRIME_ORDER_3583		= 3593,
	PRIME_ORDER_4299		= 4327,
	PRIME_ORDER_5159		= 5167,
	PRIME_ORDER_6191		= 6197,
	PRIME_ORDER_7430		= 7433,
	PRIME_ORDER_8916		= 8923,

	PRIME_ORDER_10000		= 10007,			///<約10K
	PRIME_ORDER_12000		= 12007,
	PRIME_ORDER_14400		= 14447,
	PRIME_ORDER_17280		= 17291,
	PRIME_ORDER_20736		= 20743,
	PRIME_ORDER_24883		= 24889,
	PRIME_ORDER_29859		= 29863,
	PRIME_ORDER_35831		= 35837,
	PRIME_ORDER_42998		= 43003,
	PRIME_ORDER_51597		= 51599,
	PRIME_ORDER_61917		= 61927,
	PRIME_ORDER_74300		= 74311,
	PRIME_ORDER_89161		= 89189,

	PRIME_ORDER_100000		= 100003,			///<約100K
	PRIME_ORDER_120000		= 120011,
	PRIME_ORDER_144000		= 144013,
	PRIME_ORDER_172800		= 172801,
	PRIME_ORDER_207360		= 207367,
	PRIME_ORDER_248832		= 248839,
	PRIME_ORDER_298598		= 298601,
	PRIME_ORDER_358318		= 358327,
	PRIME_ORDER_429981		= 429991,
	PRIME_ORDER_515978		= 515993,
	PRIME_ORDER_619173		= 619181,
	PRIME_ORDER_743008		= 743027,
	PRIME_ORDER_891610		= 891617,

	PRIME_ORDER_1000000		= 1000003,			///<約1M
	PRIME_ORDER_1200000		= 1200007,
	PRIME_ORDER_1440000		= 1440011,
	PRIME_ORDER_1728000		= 1728017,
	PRIME_ORDER_2073600		= 2073601,
	PRIME_ORDER_2488320		= 2488327,
	PRIME_ORDER_2985984		= 2985991,
	PRIME_ORDER_3583180		= 3583187,
	PRIME_ORDER_4299816		= 4299821,
	PRIME_ORDER_5159780		= 5159813,
	PRIME_ORDER_6191736		= 6191737,
	PRIME_ORDER_7430083		= 7430107,
	PRIME_ORDER_8916100		= 8916107,

	PRIME_ORDER_10000000	= 10000019,			///<約10M
	PRIME_ORDER_12000000	= 12000017,
	PRIME_ORDER_14400000	= 14400011,
	PRIME_ORDER_17280000	= 17280013,
	PRIME_ORDER_20736000	= 20736017,
	PRIME_ORDER_24883200	= 24883207,
	PRIME_ORDER_29859840	= 29859877,
	PRIME_ORDER_35831808	= 35831833,
	PRIME_ORDER_42998169	= 42998173,
	PRIME_ORDER_51597803	= 51597817,
	PRIME_ORDER_61917364	= 61917367,
	PRIME_ORDER_74300837	= 74300839,
	PRIME_ORDER_89161004	= 89161013,

//	PRIME_ORDER_100000000	= 100000007,	///<約100M	（こんな大量にメモリを食うアプリケーションは設計がそもそも間違ってないか・・・）
//	PRIME_ORDER_1000000000	= 1000000007,	///<約1G　

//これ以上はUINT_MAXの4294967296を超えるので使う機会がないかと。
};

/**
	@brief	マップの中で使う。キーと値のペア情報。これ自体は使わない。
	@date	2005/08/20	junjunn 作成
*/
template<class KEYTYPE,class VALUETYPE>
struct SKVPairBase
{
	KEYTYPE		m_tKey;
	VALUETYPE	m_tValue;
};

/**
Map系の巡回に使う位置情報。
list系のpositionとは全然違いインデックスベースだが利便のため同じような文法で
使えるように少し小細工を入れる。

	@date	2005/08/20	junjunn 作成
*/
class mapposition
{
public:
	mapposition();	///<コンストラクタ

/**
while (mapposition)で呼び出される有効判定。
ポインタベースのpositionとインデックスベースのmappositionを同じ形にしてしまうなんて
裏でコソコソして失礼かもしれないが・・・統一性と見るかインチキと見るか。
*/
	operator int()	{return m_iListArrayIndex>=0;}

///無効な値としてセットする。
	void setInvalid()	{m_iListArrayIndex=-1;}


public:
	int m_iListArrayIndex;		///<リスト配列のインデックス
	int m_iListIndex;			///リストの中のインデックス。
};


/*
マップのクラス図

StringMap<int>　　　　　　StringMapCollidable<int>						//値型の実体化

StringMap<VALUETYPE>　　　StringMapCollidable<VALUETYPE>				//キー型とハッシュ関数・呼び出し・書き込みなど実装。Uncollibableマップはデフォのマップ形式として名無しになる。
　　|　　　　　　　　　　　　　　　　　　|
　　|　　　　　　　　　　　　　　　　　　|　　　キー型の実体化
　　|　　　　　　　　　　　　　　　　　　|
MapUncollidable<KEYTYPE,VALUETYPE>　MapCollidable<KEYTYPE,VALUETYPE>	//ハッシュテーブルの実装
 　　 ＼　　　　　　　　　　　　　　　　／
  　　　＼　　　　　　　　　　　　　　／　　　　	継承
  　　　　＼　　　　　　　　　　　　／
　　　　　　　　　　MapBase												//ハッシュテーブルの要素数だけ
*/

/**
マップの基礎クラス。ハッシュの何もまだ持ってないのでまるで
何もできないがマップの系図的に全マップ系の一番下にこれが存在する。

	@date	2005/08/20	junjunn 作成
*/
template<class KEYTYPE,class VALUETYPE>
class MapBase
{
	typedef SKVPairBase<KEYTYPE,VALUETYPE> SKVPair;

public:
	uint getCount()	{return m_nCount;}
	virtual bool lookup(KEYTYPE tKey,VALUETYPE* ptOut) const = 0;
	virtual mapposition getStartPosition() const = 0;
	virtual void getNext(mapposition* ppos,KEYTYPE* ptKey,VALUETYPE* ptValue) const = 0;
	virtual uint getHash(KEYTYPE tKey)	const =0;
	virtual void findNextPosition(mapposition* ppos) const = 0;
	virtual void initHashTable(uint nSize) = 0;
	virtual void setAt(KEYTYPE tKey,VALUETYPE tValue)=0;
	virtual bool removeAt(KEYTYPE tKey) = 0;
	virtual void removeAll() = 0;

protected:
	MapBase()		{m_nCount=0;}		///<作れませんよ・・・。protectedにしてアクセス禁止。


protected:
	uint m_nHashTableSize;	///<ハッシュテーブルの大きさ
	uint m_nCount;			///<現在の要素数。
};



/**
マップ。キーは衝突可能。
同じ所にマップされた衝突キーはリストで管理されている。

	@date	2005/08/20	junjunn 作成
*/
template<class KEYTYPE,class VALUETYPE>
class MapUncollidable : public MapBase<KEYTYPE,VALUETYPE>
{
	typedef SKVPairBase<KEYTYPE,VALUETYPE> SKVPair;
	typedef ListAD<SKVPair*> ListADPSKVPair;

public:
///コンストラクタ。
	MapUncollidable()						{init();}
	MapUncollidable(uint nHashTableSize)	{init();initHashTable(nHashTableSize);}
///初期化
	void init();

///デストラクタ。
	virtual ~MapUncollidable();

///ハッシュテーブルの初期化。使う前に絶対に実行する必要がある。
///@param nSize ハッシュテーブルのサイズ。最大要素数より20％以上大きい素数を選ぶのが最大に効率よくなる。
	void initHashTable(uint nPrimeNumber);

///キーで探す。
///ヒットした場合は戻り値にtrueが返りptOutに値が入っている。
///ヒットしなかった場合はfalseが返りptOutには何もされない。
	bool lookup(KEYTYPE tKey,VALUETYPE* ptOut) const;

///キーをセットする。
///二回以上同じキーでsetAt()を実行した場合は該当する値がsetAt()した新しいtで置き換えられる。
	void setAt(KEYTYPE tKey,VALUETYPE tValue);

///キーで削除。
	bool removeAt(KEYTYPE tKey);
///全部削除。
	void removeAll();

///探索。スタートを得る。
	mapposition getStartPosition() const;
///探索。pposの次の位置と現在の値を得る。
	void getNext(mapposition* ppos,KEYTYPE* ptKey,VALUETYPE* ptValue) const;

protected:
///uint nHashとKEYTYPE tKeyを持ってその要素がいるリストとその位置を探す。
	bool lookup_Internal(uint nHash,KEYTYPE tKey,ListADPSKVPair** ppListOut,monapi2::position* pPosOut) const;
///uint nHashとKEYTYPE tKeyを持ってその要素のアドレスを探す。
	SKVPair* lookup_Internal(uint nHash,KEYTYPE tKey) const;

///ハッシュ関数
	uint getHash(KEYTYPE tKey)	const {return monapi2::getHash1(tKey,MapBase<KEYTYPE,VALUETYPE>::m_nHashTableSize);}

///探索。pposの次の位置を探す。
	void findNextPosition(mapposition* ppos) const;


protected:
//メンバ
///内部には分離連鎖法を使用している。
///（まずハッシュで配列に大きく振りわけて、同じ配列に飛ばされた重複文字列はリストで総当たり探索)
	ListADPSKVPair** m_apListPKVP;	//リストのアドレスの配列へのアドレス。
};

/**
	@brief	文字列限定のMapUncollidable。Monapi2リファレンスも参照。
	@date	2005/08/20	junjunn 作成
*/
template<class VALUETYPE>
class StringMap : public MapUncollidable<String,VALUETYPE>
{
public:
	StringMap()	{}
	StringMap(uint nHashTableSize):MapUncollidable<String,VALUETYPE>(nHashTableSize)	{}
	virtual ~StringMap(){}
};

/**
	@brief	int限定のMapUncollidable
	@date	2005/08/20	junjunn 作成
*/
template<class VALUETYPE>
class IntMap : public MapUncollidable<int,VALUETYPE>
{
public:
	IntMap()	{}
	IntMap(uint nHashTableSize):MapUncollidable<int,VALUETYPE>(nHashTableSize)	{}
	virtual ~IntMap()	{}
};


/*
マップ。キーは衝突不可。
同じ所にマップされたキーは元のエンコード文字列が違っていても後から来た値で上書きされる。
Monapi2リファレンスも参照。

	@date	2005/08/20	junjunn 作成
*/
template<class KEYTYPE,class VALUETYPE>
class MapCollidable : public MapBase<KEYTYPE,VALUETYPE>
{
	typedef SKVPairBase<KEYTYPE,VALUETYPE> SKVPair;

public:
	MapCollidable()						{init();};		///<コンストラクタ
	MapCollidable(uint nHashTableSize)	{init();initHashTable(nHashTableSize);}		///<コンストラクタ
	virtual ~MapCollidable();						///<デストラクタ
///初期化
	void init();

///ハッシュテーブルの初期化。使う前に絶対に実行する必要がある。
///@param nSize ハッシュテーブルのサイズ。最大要素数より20％以上大きい素数を選ぶのが最大に効率よくなる。
	void initHashTable(uint nPrimeNumber);

	void removeAll();		///<全要素を取り除く


///キーをセットする。
///二回以上同じキーでsetAt()を実行した場合は該当する値がsetAt()した新しいtで置き換えられる。
	void setAt(KEYTYPE tKey,VALUETYPE t);

///キーを見る。
///ヒットした場合は戻り値にtrueが返りptOutに値が入っている。
///ヒットしなかった場合はfalseが帰る。ptOutには何もされない。
	bool lookup(KEYTYPE tKey,VALUETYPE* ptOut) const;

///キーで削除。
	bool removeAt(KEYTYPE tKey);

	mapposition getStartPosition() const;
	void getNext(mapposition* ppos,KEYTYPE* ptKey,VALUETYPE* ptValue) const;

protected:
	uint getHash(KEYTYPE tKey)const 	{return getHash1(tKey,MapBase<KEYTYPE,VALUETYPE>::m_nHashTableSize);}

	SKVPair* lookup_Internal(uint nHash) const	{return m_apKVP[nHash];}

//pposの次の位置を探す。
	void findNextPosition(mapposition* ppos) const;

protected:
	SKVPair** m_apKVP;
};


/**
	@brief	文字列限定のMapCollidable。Monapi2リファレンスも参照。
	@date	2005/08/20	junjunn 作成
*/
template<class VALUETYPE>
class StringMapCollidable : public MapCollidable<String,VALUETYPE>
{
public:
	StringMapCollidable()	{}
	StringMapCollidable(uint nHashTableSize):MapCollidable<String,VALUETYPE>(nHashTableSize)	{}
	virtual ~StringMapCollidable()	{}
};

/**
	@brief	int限定のMapCollidable。Monapi2リファレンスも参照。
	@date	2005/08/20	junjunn 作成
*/
template<class VALUETYPE>
class IntMapCollidable : public MapCollidable<int,VALUETYPE>
{
public:
	IntMapCollidable()	{}
	IntMapCollidable(uint nHashTableSize):MapCollidable<int,VALUETYPE>(nHashTableSize)	{}
	virtual ~IntMapCollidable()	{}
};

}	//namespace monapi2

//////////////////////////////////////////////////////////////////////
//テンプレートは.cppに書いては認識されない事があるので.hに入れてる
#include "CollectionMapInline.h"

#endif
