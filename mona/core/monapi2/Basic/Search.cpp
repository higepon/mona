/**
	@file	Search.cpp
	@brief	ソート・探索

	ソート・探索

	License=Mona License
	@version $Revision$
	@date	$Date$
*/
//バグ修正をする時は関数本体説明の@date履歴に日付と名前と作業内容を付け足しておいてください。
//また.hファイルにあるクラス説明などの@date履歴部分にも同様の事をしておいてください。
#include "Search.h"
#include "CollectionList.h"

namespace monapi2
{

//QuickSortのオプション。パフォーマンス実験のためで普通はdefineしておくほうが絶対速くなる。
#define QSORTOPTION_SMARTPIVOT
#define QSORTOPTION_PRESORTCHECK


/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
void quicksort(void* pavArray,uint iArrayMemberCount,uint nArrayMemberWidth,FnQSortCompare pfnCompare)
{
	CQuickSort QuickSort;
	QuickSort.Sort(pavArray,0,iArrayMemberCount,nArrayMemberWidth,pfnCompare);
}


//CQuickSort/////////////
/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
void CQuickSort::Sort(void* pavArray,int iStart,int iCount,uint nArrayMemberWidth,FnQSortCompare pfnCompare)
{
//チューン用に時間を計る
#ifdef _PERFORMANCETEST
	DWORD dwTickStart = GetTickCount();
#endif

	m_iStart	= iStart;
	m_iCount	= iCount;
	m_pavArray	= pavArray;
	m_nArrayMemberWidth = nArrayMemberWidth;
	m_pfnCompare= pfnCompare;

//すでにソートされているかチェック
#ifdef QSORTOPTION_PRESORTCHECK
	if (isSorted())
	{
#ifdef _PERFORMANCETEST
		m_nTimeTaken = GetTickCount() - dwTickStart;
#endif
		return;
	}
#endif

	m_pSwapBuffer = new char[m_nArrayMemberWidth];

	int iLeft = iStart;
	int iRight = iStart+iCount-1;


	Stack<int> stack;

//QuickSortには再帰を取り除きスタックで実装したバージョンを使う。
//再帰も結局はOS的に実行するスタックなので
//再帰的に呼び出す手法よりかはこうしてプログラム内で自分でスタックを実装して1関数に納める方が
//スピード的にもメモリ的にも有利で効率よくチューンアップできる。
	for (;;)
	{
		while (iRight>iLeft)
		{
//これを実行し終えるとiPivotより左は全てpavArray[iPivot]より<=で、右は全て>=な要素が並ぶようソートされている。
			int iPivot = Partition(iLeft,iRight);

//右と左のどちらが要素数多いか比較。
			if (iPivot-iLeft > iRight-iPivot)
			{
				stack.push(iLeft);
				stack.push(iPivot-1);
				iLeft=iPivot+1;
			}
			else
			{
				stack.push(iPivot+1);
				stack.push(iRight);
				iRight=iPivot-1;
			}
		}

		if (stack.isEmpty()) break;
		iRight=stack.pop();
		iLeft=stack.pop();
	}

	delete m_pSwapBuffer;

#ifdef _PERFORMANCETEST
	m_nTimeTaken = GetTickCount() - dwTickStart;
#endif


//ソートが正しく行われたか検算
#ifdef _DEBUG
//	assert(IsSorted());
#endif

}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
int CQuickSort::Partition(int iLeft,int iRight)
{
//スマートピボット
#ifdef QSORTOPTION_SMARTPIVOT
/*
変な話だがQuickSortはあらかじめソートされているデータより完全にランダムなデータのソートの方が
遙かに速い。
（QuickSortはピボットでデータがうまく2分割された時が互いの比較回数が減って最高のスピードが出る。
　あらかじめソートされているデータはちょうどその逆で分割が全く働かなく最悪に遅くなる。）
その最悪ケースを避ける為にちょっとした並び替えを行っておく。

具体的にはまず最初、最後、中間の3つの値から中央値を探し出しそれをピボットに利用する。
（端の数字がピボットになるのが最悪なので）。
*/
	int iMiddle = (iLeft+iRight)/2;
//ピボットを探して、ついでに並び替えも行っておく。
	sortTriplet(iLeft,iMiddle,iRight);

//真ん中の値をiRight-1に持ってきてピボットとして利用する。
	swap(iMiddle,iRight-1);

//すでにSortTripletで左右両端は中央値に対しソートされてる事がわかっているので範囲を狭めていい
	iRight--;
	iLeft++;
#endif

//ここからPartitionのメイン
	void* pvPivot= getElementAddress(iRight);
	int iSwapLeft=iLeft-1;
	int iSwapRight=iRight;

#ifdef QSORTOPTION_SMARTPIVOT
	if (iSwapLeft>=iSwapRight)	return iSwapLeft;
	if (iRight==0)				return iSwapLeft;
#endif

	for(;;)
	{
		while (compare(getElementAddress(++iSwapLeft)	,pvPivot)<0);	//最悪pvPivotで止まるのでwhileループは問題ない。
		while (compare(getElementAddress(--iSwapRight)	,pvPivot)>0);	//同じく最悪pvPivotで止まるので問題ない。
		if (iSwapLeft>=iSwapRight)	break;
		swap(iSwapLeft,iSwapRight);
	}

	swap(iSwapLeft,iRight);

	return iSwapLeft;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
void CQuickSort::swap(int i1,int i2)
{
	set(m_pSwapBuffer,getElementAddress(i1));
	set(getElementAddress(i1),getElementAddress(i2));
	set(getElementAddress(i2),m_pSwapBuffer);
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
void CQuickSort::sortTriplet(int iLeft,int iMiddle,int iRight)
{
	if (isSwapRequired(getElementAddress(iLeft),getElementAddress(iMiddle)))	swap(iLeft,iMiddle);
	if (isSwapRequired(getElementAddress(iLeft),getElementAddress(iRight)))		swap(iLeft,iRight);
	if (isSwapRequired(getElementAddress(iMiddle),getElementAddress(iRight)))	swap(iMiddle,iRight);
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
bool CQuickSort::isSorted()
{
	for (int i=m_iStart;i<m_iStart+m_iCount-1;i++)
	{
		if (isSwapRequired(getElementAddress(i),getElementAddress(i+1)))
			return false;
	}
	return true;
}

/*
void CQuickSort::SmallSort(int iStart,int iCount)
{
	InsertionSort(iStart,iCount);
}

void CQuickSort::InsertionSort(int iStart,int iCount)
{
	for (int i=iStart;i<iStart+iCount;i++)
	{
		void* pvPivot=getElementAddress(i);
		int j=i;
		while (isSwapRequired(getElementAddress(j-1),pvPivot) && j>=iStart)	//@todo 番兵を使うなどでここは効率化できるかも。
		{
			set(getElementAddress(j),getElementAddress(j-1));
			j--;
		}

		set(getElementAddress(j),pvPivot);
	}
}
*/

///その他/////////////
#ifdef _PERFORMANCETEST
/**
	@brief	速度を測る。
	@date	2005/08/20	junjunn 作成
*/
void TestQSort()
{
//intテスト
	{
		int iTestCount = 100000;	//ソートする要素数

		UINT nTimeRandom;			//完全ランダムをソートするのにかかる時間
		UINT nTimeSorted;			//すでに完全にソートされているのにもう一度ソートかけたらかかる時間
		UINT nTimeSortedReverse;	//すでに完全に逆順ソートされているのをソートするにかかる時間
		UINT nTimeSortedAlmost;	//ほぼ完全にソートだが、一部に非ソートが混じってるデータをソートするのにかかる時間

		CQuickSort QuickSort;
		srand(time(NULL));

		int* paiArray = new int[iTestCount];

		for (int i=0;i<iTestCount;i++)		paiArray[i] = SCMath::GetRandom(iTestCount);
		QuickSort.Sort(paiArray,0,iTestCount,sizeof(int),CompareInt);
		nTimeRandom = QuickSort.m_nTimeTaken;

		for (i=0;i<iTestCount;i++)		paiArray[i] = i;
		QuickSort.Sort(paiArray,0,iTestCount,sizeof(int),CompareInt);
		nTimeSorted = QuickSort.m_nTimeTaken;

		for (i=0;i<iTestCount;i++)		paiArray[i] = iTestCount-i;
		QuickSort.Sort(paiArray,0,iTestCount,sizeof(int),CompareInt);
		nTimeSortedReverse = QuickSort.m_nTimeTaken;

		for (i=0;i<iTestCount;i++)		paiArray[i] = i;
		for (i=1;i<=4;i++)	paiArray[SCMath::GetRandom(iTestCount)] = iTestCount / 5 * (i);
		QuickSort.Sort(paiArray,0,iTestCount,sizeof(int),CompareInt);
		nTimeSortedAlmost = QuickSort.m_nTimeTaken;

		delete paiArray;

		::CString strReport;
		strReport.Format("Random-	%d\nSorted-	%d\nSortedReverse-	%d\nSortedAlmost-	%d",nTimeRandom ,nTimeSorted ,nTimeSortedReverse ,nTimeSortedAlmost);
		SCWinFile::Dump((PCSTR1)(PCSSTR1)strReport);


		int aiArray[]={1,5,3,9,7,2,6,10,4,8};
		QuickSort.Sort(aiArray,0,10,sizeof(int),CompareInt);

/*
実験結果。

iTestCount = 100000のソートにかかったTick時間(CPU：Athlon64 3000+)。
それぞれの実験を3回行い平均を取っている。
WindowsのTick性能の悪さで各数値に最大20ぐらいの誤差が出てるっぽいが
優劣を比べる程度ははっきり出てるので(ﾟεﾟ)ｷﾆｼﾅｲ

○スタック・×スマートピボット・×ソートチェック・×SmallSort
	Random-	141
	Sorted-	48625
	SortedReverse-	48472
	SortedAlmost-	31563
＊あらかじめソートされてるデータに対してはアホみたいに性能が落ちる・・・


○スタック・○スマートピボット・×ソートチェック・×SmallSort
	Random-	125
	Sorted-	83
	SortedReverse-	78
	SortedAlmost-	67
＊ソートデータ系に圧倒的な改善。


○スタック・○スマートピボット・○ソートチェック・×SmallSort
	Random-	125
	Sorted-	0
	SortedReverse-	99
	SortedAlmost-	67
＊SortedReverseが少し落ちた代わりにSortedが0に。
SortedReverseを使う機会は少ないだろうからSortedのメリットに対しこのリスクは採算あると思う。
Randomの時は一瞬でソートチェックも終わるのでオーバーヘッドも少ないと思う。


なお条件をintに限ってpFnCompareの代わりに直接< >比較式を埋め込んでソートすると
速さは以下の通りになった。
○スタック・×スマートピボット・×ソートチェック・×SmallSort
	Random-	62
○スタック・○スマートピボット・×ソートチェック・×SmallSort
	Random-	63
○スタック・○スマートピボット・○ソートチェック・×SmallSort
  	Random-	62

全体的に倍ぐらい速くなれる。
当然と言えば当然だがintなどの単純データ型にはqsortではなく
それぞれに最適化したソートを用意しておくと高速になれる。


@Memo QuickSort改善案
まだSmallSortは未インプリメント。
最適化は凝り出すとキリがないので他の作業もあるし今はここらで切り上げる。
*/
	}

//文字列テスト
	{
		srand(time(NULL));
		int iTestCharCount = 10;
		char** paszArray = new char*[iTestCharCount];

		for (int i=0;i<iTestCharCount;i++)
		{
			paszArray[i] = new char[11];
			for (int j=0;j<10;j++)
			{
				paszArray[i][j] = (char)('a' + SCMath::GetRandom(26));
			}
			paszArray[i][10] = '\0';
		}

		CQuickSort QuickSort;
		QuickSort.Sort(paszArray,0,iTestCharCount,sizeof(char*),CompareString);

		for (i=0;i<iTestCharCount;i++)	{delete paszArray[i];}
		delete paszArray;


		char* aszArray[]={"Zonu","Mona","Nida","Sii","Morara","Giko","Siraneeyo",};
		QuickSort.Sort(aszArray,0,7,sizeof(char*),CompareString);
	}
}
#endif


}	//namespace monapi2
