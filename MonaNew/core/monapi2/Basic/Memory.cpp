/**
	@file	Memory.cpp
	@brief	メモリ読み込み・メモリ書き込み・メモリ操作

	メモリ読み込み・メモリ書き込み・メモリ操作

	License=Mona License
	@version $Revision$
	@date	$Date$
*/
//バグ修正をする時は関数本体説明の@date履歴に日付と名前と作業内容を付け足しておいてください。
//また.hファイルにあるクラス説明などの@date履歴部分にも同様の事をしておいてください。
#include "Memory.h"
#include "Math.h"

namespace monapi2	{

/*
・高速化メモ1

	forの処理を
		for(uint n=0;n<nCount;n++)
	と書くときと
		for(uint n=nCount;n>0;n--)
	と書くときは後者の方がループごとにnCountの値を呼び出す必要がないので速そうだが
	比較テストした結果は違いはなかった。

	コンパイラの最適化処理で実行時にはどっちも最速な形に直されるのか。
	書く方は気にしないでよさそうだ。


・高速化メモ2

	ここではnCountの連続した処理を
		divide(nCount,4,&i4ByteCount,&i1ByteCount);
		for(uint n=0;n<(uint)i4ByteCount;n++)		{*pdwTo++ = iValue4;}
	な感じでやっているが別の書き方として
		for (;nCount>=4;nCount-=4)					{*pdwTo++ = iValue4;}
	とかにしたらどうなるか。

	後者の方がdivideを呼ぶ手間が省けコード量も減ってスマートに見えるが実際は遅くなる。
	前者の書き方だとループの中が++の可能な限りのシンプルなループになるのに比べ
	後者はループカウンタの他に-=4の計算が入ってこれがペナルティになるようだ。

	
その他にも実験した事はMemoryFn::setにちょこちょこ書いてます。
*/


/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
void MemoryFn::copy(void* pTo,const void* cpFrom, uint nCount)
{
//@memo もしかしたら4バイト境界対策が必要かも。

//スピードを上げるために4バイト転送を使う。
//まず転送バイト数を4で割って商と剰余を計算し、4バイト転送と1バイト転送にわける。
	int i4ByteCount,i1ByteCount;
	divide(nCount,4,&i4ByteCount,&i1ByteCount);

#ifdef _COMPILER_VISUALC
//4バイト転送
	__asm
	{
		mov edi,pTo			//レジスタセット
		mov esi,cpFrom
		mov ecx,i4ByteCount
		cld					//順方向転送指示
		rep movsd			//4バイト転送命令
	}

//1バイト転送
	__asm
	{
		mov ecx,i1ByteCount	//他のレジスタ値は4バイト転送の時にセットした時から流用
		rep movsb			//1バイト転送
	}
#else
//4バイト転送部分
    asm volatile("movl %0, %%edi \n"
                 "movl %1, %%esi \n"
                 "movl %2, %%ecx \n"
                 "cld            \n"
                 "rep movsd      \n"
                 :
                 : "m"(pTo), "m"(cpFrom), "m"(i4ByteCount)
                 : "edi", "esi", "ecx");

//1バイト転送部分
    asm volatile("movl %0, %%ecx \n"
                 "rep movsb      \n"
                 :
                 : "m"(i1ByteCount)
                 : "ecx");
#endif
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
void MemoryFn::swap(void* pBuffer1,void* pBuffer2,uint nCount)
{
//スピードを上げるために4バイト転送を使う。
	int i4ByteCount,i1ByteCount;
	divide(nCount,4,&i4ByteCount,&i1ByteCount);

//4バイト転送部分
	uint32* pdwBuffer1=(uint32*)pBuffer1;
	uint32* pdwBuffer2=(uint32*)pBuffer2;
	uint n;
	for(n=0;n<(uint)i4ByteCount;n++)
	{
		int iTemp = *pdwBuffer1;
		*pdwBuffer1++ = *pdwBuffer2;
		*pdwBuffer2++ = iTemp;
	}

//1バイト転送部分
	uint8* pcBuffer1=(uint8*)pdwBuffer1;
	uint8* pcBuffer2=(uint8*)pdwBuffer2;
	for(n=0;n<(uint)i1ByteCount;n++)
	{
		char cTemp = *pcBuffer1;
		*pcBuffer1++ = *pcBuffer2;
		*pcBuffer2++ = cTemp;
	}
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
void MemoryFn::copySafe(void* pTo, const void* cpFrom,uint nCount)
{
//普通のcopyでは困る条件
	if (pTo>cpFrom && pTo<(uint8*)cpFrom+nCount)
	{
//スピードを上げるために4バイト転送を使う。
		int i4ByteCount,i1ByteCount;
		divide(nCount,4,&i4ByteCount,&i1ByteCount);

//反対方向からコピー
//1バイト転送部分
		uint8* pcTo		=(uint8*)pTo	+ nCount;
		uint8* cpcFrom	=(uint8*)cpFrom	+ nCount;
		uint n;
		for(n=0;n<(uint)i1ByteCount;n++)
		{
			*--pcTo = *--cpcFrom;
		}

//4バイト転送部分
		uint32* pdwTo		=(uint32*)pcTo;
		uint32* cpdwFrom	=(uint32*)cpcFrom;
		for(n=0;n<(uint)i4ByteCount;n++)
		{
			*--pdwTo = *--cpdwFrom;
		}
	}
	else
	{
		copy(pTo,cpFrom,nCount);
	}
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
int MemoryFn::findDiff(const void* cpBuffer1,const void* cpBuffer2,uint nCount)
{
//スピードを上げるために4バイト比較を使う。
	int i4ByteCount,i1ByteCount;
	divide(nCount,4,&i4ByteCount,&i1ByteCount);

	uint32* cpdwBuffer1=(uint32*)cpBuffer1;
	uint32* cpdwBuffer2=(uint32*)cpBuffer2;
	uint n;
	for (n=0;n<(uint)i4ByteCount;n++)
	{
//違いを見つけたら1バイト比較に切り替え。
		if (*cpdwBuffer1!=*cpdwBuffer2)
		{
			uint8* cpcBuffer1=(uint8*)cpdwBuffer1;
			uint8* cpcBuffer2=(uint8*)cpdwBuffer2;
			for (;;)	//必ず次の4バイト中に止まるので必要なし。
			{
				if (*cpcBuffer1!=*cpcBuffer2)	return (cpcBuffer1-(uint8*)cpBuffer1);

				cpcBuffer1++;
				cpcBuffer2++;
			}
		}

		cpdwBuffer1++;
		cpdwBuffer2++;
	}

	uint8* cpcBuffer1=(uint8*)cpdwBuffer1;
	uint8* cpcBuffer2=(uint8*)cpdwBuffer2;
	for(n=0;n<(uint)i1ByteCount;n++)
	{
		if (*cpcBuffer1!=*cpcBuffer2)	return (cpcBuffer1-(uint8*)cpBuffer1);

		cpcBuffer1++;
		cpcBuffer2++;
	}

	return -1;
}

/**
	@brief	説明、引数、戻り値はMonapi2リファレンス参照。
	@date	2005/08/20	junjunn 作成
*/
void MemoryFn::set(void* pTo,int iValue,uint nCount)
{
//スピードを上げるために4バイト転送を使う。
	uint32* pdwTo=(uint32*)pTo;
	int iValue4 = iValue + (iValue<<8) + (iValue<<16) + (iValue<<24);


//スピード実験のために二つの違う書き方で実装を試みてみる。

//ケース1。divideは使わないで-=4ループを使う。遅い。10000000回のループに35.0秒かかった。
#if 0
	for (;nCount>=4;nCount-=4)
	{
		*pdwTo++ = iValue4;
	}

/*
上記のforの中のVCによる最適化後アセンブラ

00401971 	mov	ebx,[ebp+00000010h]	//ebx = n2
00401974 	cmp	ebx,04h				//if (n2>4)
00401977 	jc	00401992h			//if (n2>4) goto 401992

00401979 	push	esi
0040197A 	mov	esi,ebx
0040197C 	shr	esi,02h
0040197F 	mov	ecx,esi
00401981 	mov	eax,edx
00401983 	repz
00401984 	stosd
00401985 	mov	eax,[ebp+00000008h]
00401988 	lea	edi,[eax+esi*4]

0040198B 	sub	ebx,04h
0040198E 	dec	esi
0040198F 	jnz	0040198Bh
00401991 	pop	esi

00401997 	test	ebx,ebx
00401999 	jbe	004019BAh
0040199B 	mov	al,[ebp+0000000Ch]
0040199E 	mov	ecx,ebx
004019A0 	mov	bl,al
004019A2 	mov	edx,ecx
004019A4 	mov	bh,bl
004019A6 	mov	eax,ebx
004019A8 	shl	eax,10h
004019AB 	mov	ax,bx
004019AE 	shr	ecx,02h
004019B1 	repz
004019B2 	stosd
004019B3 	mov	ecx,edx
004019B5 	and	ecx,03h
004019B8 	repz
004019B9 	stosb
～return
*/
	uint n;
	int i1ByteCount =nCount;

//ケース2。divideで分けて++ループを使う。速い。10000000回のループに7.5秒かかった。
#else
	int i4ByteCount,i1ByteCount;
	divide(nCount,4,&i4ByteCount,&i1ByteCount);

	uint n;
	for(n=0;n<(uint)i4ByteCount;n++)
	{
		*pdwTo++ = iValue4;
	}

/*
上記のforの中のVCによる最適化後アセンブラ

00401988 	mov	edx,[ebp+0000000Ch]		:.
0040198B 	test	edx,edx				:.
0040198D 	jbe	0040199Bh				:if (n2!=0)	goto 40199B

0040198F 	mov	ecx,edx					:ecx=n2;
00401991 	mov	eax,esi					:eax=
00401993 	repz
00401994 	stosd
00401995 	mov	eax,[ebp+00000008h]
00401998 	lea	edi,[eax+edx*4]

004019A0 	mov	ecx,[ebp+00000010h]
004019A3 	test	ecx,ecx
004019A5 	jbe	004019C1h
004019A7 	mov	al,bl
004019A9 	mov	bh,bl
004019AB 	mov	eax,ebx
004019AD 	mov	edx,ecx
004019AF 	shl	eax,10h
004019B2 	mov	ax,bx
004019B5 	shr	ecx,02h
004019B8 	repz
004019B9 	stosd
004019BA 	mov	ecx,edx
004019BC 	and	ecx,03h
004019BF 	repz
004019C0 	stosb
～return
*/
#endif


//1バイト転送部分
	uint8* pcTo=(uint8*)pdwTo;
	for(n=0;n<(uint)i1ByteCount;n++)
	{
		*pcTo++ = (uint8)iValue;
	}
/*
ここは
	uint anMask1[4] ={0x00000000,0x000000FF,0x0000FFFF,0x00FFFFFF};
	uint anMask2[4] ={0xFFFFFFFF,0xFFFFFF00,0xFFFF0000,0xFF000000};
	*pdwTo &= anMask2[i1ByteCount];
	*pdwTo |= 0x12345678 & anMask1[i1ByteCount];
のように書けば少なくて済むと思うが
いかんせんこれだと一時的にとは言えdwordでバッファの垣根を越えて
read、writeしていて保護違反くらいそうなので止める。
*/
}


}		//namespace monapi2
