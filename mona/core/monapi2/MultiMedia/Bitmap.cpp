/**
	@file	Bitmap.cpp
	@brief		ビットマップの取り扱い・操作。

	ビットマップの取り扱い・操作。

	License=Mona License
	@version $Revision$
	@date	$Date$
*/
//バグ修正をする時は関数本体説明の@date履歴に日付と名前と作業内容を付け足しておいてください。
//また.hファイルにあるクラス説明などの@date履歴部分にも同様の事をしておいてください。
#ifdef MONA
	#include <monapi.h>
#endif

#include "Bitmap.h"
#include "../Basic/File.h"
#include "../Basic/Debug.h"
#include "../Basic/Memory.h"
#include "Size.h"

namespace monapi2	{

#define ASSERT_PIXEL(x,y)	ASSERT(x>=0 && x<m_oSize.getWidth() && y>=0 && y<m_oSize.getHeight());

	
	

//.bmpファイルの規格。古い規格のため今じゃ使われてないメンバや全く役に立たないメンバも多く
//ここではあえて.bmp形式に適合はしない。使わないメンバは全く無視する。
//過去互換の為のファイル入出力に使うだけ。

/**
ビットマップについての情報。

	@date	2005/08/20	junjunn 作成
*/
class BitmapInfoHeader
{
public:
	void set(const uint8_t* p)
	{
		m_iWidth	=*(uint32_t*)(p+0x04);
		m_iHeight	=*(uint32_t*)(p+0x08);
		m_wBitFormat=* (uint16_t*)(p+0x0E);
	}


public:
//	uint32_t	m_dwSizeStructure;		///<この構造体のサイズ。ある意味バージョン情報に使われる。
	int		m_iWidth;				///<幅。uintじゃなくてintなのはマイナス値だと上下反転のBMPを生成する意味とか考えられる。
	int		m_iHeight;				///<高さ。
//	uint16_t	m_wPlanes;				///<不使用。常に1。
	uint16_t	m_wBitFormat;			///<何ビットフォーマットか。1,4,8,16,24,32などがある。
//	uint32_t	m_dwCompression;		///<圧縮方法。普通は不使用。
//	uint32_t	m_dwSize;				///<イメージのサイズ。圧縮をしているときの解凍後の大きさに使うが普通は不使用。
//	long	m_lXPelsPerMeter;		///<水平解像度。普通は不使用。
//	long	m_lYPelsPerMeter;		///<垂直解像度。普通は不使用。
//	uint32_t	m_dwClrUsed;			///<実際に使われている色数。普通は不使用。
//	uint32_t	m_dwClrImportant;		///<重要な色数。普通は不使用。
}; 

//色指定。いわゆるパレットに使う。
/**
	@date	2005/08/20	junjunn 作成
*/
struct RGBQuad
{
	uint8_t	r; 
	uint8_t	g; 
	uint8_t	b;
	uint8_t	m_byReserved; 
}; 

//ビットマップ情報+パレットデータ。24ビットならパレットがないのでパレットデータ部は無効。
/**
	@date	2005/08/20	junjunn 作成
*/
struct BitmapInfo
{
	BitmapInfoHeader	m_BitmapInfoHeader;
	RGBQuad*			m_RGBQuad;	//ビットマップの色ビット数に応じて必要な分だけ動的に確保する。
};

//ビットマップファイルヘッダー。
/**
	@date	2005/08/20	junjunn 作成
*/
struct BitmapFileHeader
{
public:
	void set(const uint8_t* p)	
	{
		m_dwSize		=*(uint32_t*)(p+0x02);
		m_dwOffsetToData=*(uint32_t*)(p+0x0A);
	}
	int getDataSize()	{return m_dwSize-m_dwOffsetToData;};		//データ本体の大きさ。

public:
//	uint16_t	m_wType;			///<ファイルタイプ識別。常に"BM"。
	uint32_t	m_dwSize;			///<ファイルの大きさ。
//	uint16_t	m_wReserved1;		///<不使用。
//	uint16_t	m_wReserved2;		///<不使用。
	uint32_t	m_dwOffsetToData;	///<ファイル先頭からデータ本体までのビット数。
}; 


//Bitmap/////////////////
/**
	@brief	Monapi2リファレンスを参照。
	@date	2005/08/20	junjunn 作成
*/
void Bitmap::init()
{
	m_oSize.set(0,0);

//	m_strPath.empty();
	m_bufferData.clear();

	m_bYReverse = false;

//	m_strPath = "Bitmap.bmp";
}

/**
	@brief	Monapi2リファレンスを参照。
	@date	2005/08/20	junjunn 作成
*/
void Bitmap::clear()
{
	init();
}

/**
	@brief	Monapi2リファレンスを参照。
	@date	2005/09/20	junjunn 作成
*/
uint8_t* Bitmap::createCanvas(const Size* cpSize)
{
	return createCanvas(cpSize->getWidth(),cpSize->getHeight());
}

/**
	@brief	Monapi2リファレンスを参照。
	@date	2005/08/20	junjunn 作成
	@date	2005/09/20	junjunn createCanvasから名前を変更。
*/
uint8_t* Bitmap::createCanvas(int iWidth,int iHeight)
{
	return getNewBuffer(iWidth,iHeight);
}

/**
	@brief	Monapi2リファレンスを参照。
	@date	2005/08/20	junjunn 作成
*/
uint8_t* Bitmap::getNewBuffer(int iWidth,int iHeight)
{
	clear();

	m_oSize.set(iWidth,iHeight);
	setLineBits();

	return m_bufferData.extendBuffer(m_iLineSize * iHeight);
}

/**
	@brief	Monapi2リファレンスを参照。
	@date	2005/08/20	junjunn 作成
*/
void Bitmap::setLineBits()
{
	m_iLineSize = getBMPLineSize(m_oSize.getWidth());
}

/**
	@brief	Monapi2リファレンスを参照。
	@date	2005/08/20	junjunn 作成
*/
void Bitmap::copy(const Bitmap* cpBitmap)
{
	if (! cpBitmap->isValid())	return;

	clear();

	m_oSize = cpBitmap->m_oSize;
	m_bufferData.copy(&cpBitmap->m_bufferData);
}

uint8_t* Bitmap::getPixelBuffer()
{
	return m_bufferData.getBuffer();
}

/**
	@brief	Monapi2リファレンスを参照。
	@date	2005/08/20	junjunn 作成
*/
uint8_t* Bitmap::getPixelBuffer(int x,int y)
{
	int iOffset = (m_oSize.getHeight() - y - 1)*m_iLineSize + x*3;
	return m_bufferData.getBuffer() + iOffset;
}

/**
	@brief	Monapi2リファレンスを参照。
	@date	2005/08/20	junjunn 作成
*/
const uint8_t* Bitmap::getPixelBufferConst(int x,int y) const
{
	int iOffset = (m_oSize.getHeight() - y - 1) * m_iLineSize + x*3;
	return m_bufferData.getData() + iOffset;
}

/**
	@brief	Monapi2リファレンスを参照。
	@date	2005/10/03	junjunn 作成
*/
const uint8_t* Bitmap::getPixelBufferConst() const
{
	return m_bufferData.getData();
}

/**
	@brief	Monapi2リファレンスを参照。
	@date	2005/08/20	junjunn 作成
*/
colort Bitmap::getPixel(int x,int y) const
{
	const uint8_t* cpBaseByte = getPixelBufferConst(x,y);
//BGRに並んでいる。
	return makeColor(cpBaseByte[2],cpBaseByte[1],cpBaseByte[0]);
}


/**
	@brief	Monapi2リファレンスを参照。
	@date	2005/08/20	junjunn 作成
*/
void Bitmap::setPixel(int x,int y,colort color)
{
//	ASSERT_PIXEL(x,y);

	uint8_t* pBaseByte = getPixelBuffer(x,y);

	pBaseByte[0] = getBValue(color);
	pBaseByte[1] = getGValue(color);
	pBaseByte[2] = getRValue(color);
}

/**
	@brief	Monapi2リファレンスを参照。
	@date	2005/08/20	junjunn 作成
*/
void Bitmap::fill(colort color)
{
	uint8_t* p = getPixelBuffer();
//uint32_t転送では最後の一つだけはバッファの外に出てしまうので。
	for (int i=0;i<(m_oSize.getWidth()*m_oSize.getHeight())-1;i++)
	{
		*(uint32_t*)p = color;
		p+=3;
	}

	setPixel(m_oSize.getWidth()-1,m_oSize.getHeight()-1,color);
}

/**
	@brief	Monapi2リファレンスを参照。
	@date	2005/08/20	junjunn 作成
	@date	2005/10/03	junjunn Y反転機能を付け加える。
*/
bool Bitmap::read(cpchar1 cszPath,bool bReverseY)
{
	File file;
	if (! file.open(cszPath))		return false;

//BMPファイルヘッダの読み込み
	int iRead=0;
	BitmapFileHeader bitmapFileHeader;
	BitmapInfoHeader bitmapInfoHeader;

	uint8_t abyBuffer[100];
	iRead+=file.read(abyBuffer,iRead,14);
	bitmapFileHeader.set(abyBuffer);

	iRead+=file.read(abyBuffer,iRead,40);
	bitmapInfoHeader.set(abyBuffer);

//現時点では24ビット以外は対応してない。
	if (bitmapInfoHeader.m_wBitFormat != 24)		return false;
//	ASSERT(m_oSize.getWidth()>0 && m_oSize.getHeight()>0);

//初期化。設定。
//	m_strPath = cszPath;
	uint8_t* pBuffer = createCanvas(bitmapInfoHeader.m_iWidth,bitmapInfoHeader.m_iHeight);


	m_bYReverse = bReverseY;

//読み込み。
	int iDataStart = bitmapFileHeader.m_dwOffsetToData;
	for (int y=0;y<getHeight();y++)
	{
//Y反転機能。VRAMなどは.bmpと逆に上から下にデータが流れるので反転しておくと便利な場合もある。
		int y2 = (! bReverseY)?y:(getHeight()-y-1);

		int iOffsetYSource = y * getLineDataSize();
		int iOffsetYTarget = y2 * getLineDataSize();
		file.read(pBuffer + iOffsetYTarget,iDataStart + iOffsetYSource,getLineDataSize());
	}

	return true;
}

/**
	@brief	Monapi2リファレンスを参照。
	@date	2005/08/20	junjunn 作成
*/
void Bitmap::write(cpchar1 cszPath) const
{
//	ASSERT(m_iBitCount==8 || m_iBitCount==24);

const int iBMP_HEADER_SIZE = 0x36;

//54バイト
	uint8_t abyHeader[] = {
0x42,0x4D,				//00:ファイルの識別ID
0x36,0x10,0x0E,0x00,	//02:サイズ。例。640*480*24 = E10000。それにヘッダーを加える 
0x00,0x00,				//06:固定。0で
0x00,0x00,				//08:固定。0で
0x36,0x00,0x00,0x00,	//0A:画像データ本体へのオフセット

0x28,0x00,0x00,0x00,	//0E:固定。ヘッダーサイズ
0x80,0x02,0x00,0x00,	//12:m_nWidth.			幅
0xE0,0x01,0x00,0x00,	//16:biHeight.			高さ
0x01,0x00,				//1A:biPlanes.			固定。カラープレーン
0x18,0x00,				//1C:m_wBitFormat.		カラービット
0x00,0x00,0x00,0x00,	//1E:biCompression.		固定。圧縮スキーム(0=なし）
0x00,0x10,0x0E,0x00,	//22:biSizeImage.		画像のサイズ（圧縮時のみ）
0x13,0x0B,0x00,0x00,	//26:biXPelsPerMeter.	固定。水平解像度
0x13,0x0B,0x00,0x00,	//2A:biYPelsPerMeter.	固定。垂直解像度
0x00,0x00,0x00,0x00,	//2E:biClrUsed.			固定。色数
0x00,0x00,0x00,0x00};	//32:biClrImportant.	固定。必要な色数（現在は使用されてない)


	int* piSize		= (int*) (abyHeader + 2);
	int* piMainDataOffset = (int*) (abyHeader + 10);
	int* piWidth	= (int*) (abyHeader + 18);
	int* piHeight	= (int*) (abyHeader + 22);
	int* piBitCount	= (int*) (abyHeader + 28);

	int iSize=0;
	int iMainDataOffset=0;

	iSize = m_iLineSize * m_oSize.getHeight() + iBMP_HEADER_SIZE;
	iMainDataOffset = iBMP_HEADER_SIZE;

	*piSize = iSize;
	*piMainDataOffset = iMainDataOffset;
	*piWidth			= m_oSize.getWidth();
	*piHeight			= m_oSize.getHeight();
	*piBitCount			= 24;

	File file(cszPath,true);
	int iWrite=0;
	iWrite += file.write(abyHeader,iWrite,sizeof(abyHeader));
	iWrite += file.write(m_bufferData.getData(),iWrite,m_bufferData.getSize());
}


//その他関数////////////////
/**
	@date	2005/08/20	junjunn 作成
*/
int	getBMPLineSize(int iWidth)
{
//各行の大きさは4バイト単位でなくてはならないので必要なら空バイトを追加する。
	int iLineSize = 3 * iWidth;

	int iExcess = iLineSize % 4;
	if (iExcess != 0)
	{
		iLineSize += (4-iExcess);
	}

	return iLineSize;
}


}	// namespace monapi2
