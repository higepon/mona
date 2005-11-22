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
#include "Bitmap.h"
#include "../Basic/File.h"
#include "../Basic/Debug.h"
#include "../Basic/Memory.h"

namespace monapi2	{

#define ASSERT_PIXEL(x,y)	ASSERT(x>=0 && x<m_iWidth && y>=0 && y<m_iHeight);

	
	

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
	void set(const byte* p)
	{
		m_iWidth	=*(dword*)(p+0x04);
		m_iHeight	=*(dword*)(p+0x08);
		m_wBitFormat=*(word*) (p+0x0E);
	}


public:
//	dword	m_dwSizeStructure;		///<この構造体のサイズ。ある意味バージョン情報に使われる。
	int		m_iWidth;				///<幅。uintじゃなくてintなのはマイナス値だと上下反転のBMPを生成する意味とか考えられる。
	int		m_iHeight;				///<高さ。
//	word	m_wPlanes;				///<不使用。常に1。
	word	m_wBitFormat;			///<何ビットフォーマットか。1,4,8,16,24,32などがある。
//	dword	m_dwCompression;		///<圧縮方法。普通は不使用。
//	dword	m_dwSize;				///<イメージのサイズ。圧縮をしているときの解凍後の大きさに使うが普通は不使用。
//	long	m_lXPelsPerMeter;		///<水平解像度。普通は不使用。
//	long	m_lYPelsPerMeter;		///<垂直解像度。普通は不使用。
//	dword	m_dwClrUsed;			///<実際に使われている色数。普通は不使用。
//	dword	m_dwClrImportant;		///<重要な色数。普通は不使用。
}; 

//色指定。いわゆるパレットに使う。
/**
	@date	2005/08/20	junjunn 作成
*/
struct RGBQuad
{
	byte	r; 
	byte	g; 
	byte	b;
	byte	m_byReserved; 
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
	void set(const byte* p)	
	{
		m_dwSize		=*(dword*)(p+0x02);
		m_dwOffsetToData=*(dword*)(p+0x0A);
	}
	int getDataSize()	{return m_dwSize-m_dwOffsetToData;};		//データ本体の大きさ。

public:
//	word	m_wType;			///<ファイルタイプ識別。常に"BM"。
	dword	m_dwSize;			///<ファイルの大きさ。
//	word	m_wReserved1;		///<不使用。
//	word	m_wReserved2;		///<不使用。
	dword	m_dwOffsetToData;	///<ファイル先頭からデータ本体までのビット数。
}; 


//Bitmap/////////////////
/**
	@brief	時間・時刻を取り扱う。Monapi2リファレンスも参照。
	@date	2005/08/20	junjunn 作成
*/
void Bitmap::init()
{
	m_iWidth	= 0;
	m_iHeight	= 0;

	m_strPath.empty();
	m_bufferData.clear();

	m_strPath = "Bitmap.bmp";
}

/**
	@brief	時間・時刻を取り扱う。Monapi2リファレンスも参照。
	@date	2005/08/20	junjunn 作成
*/
void Bitmap::clear()
{
	init();
}

/**
	@brief	時間・時刻を取り扱う。Monapi2リファレンスも参照。
	@date	2005/08/20	junjunn 作成
*/
byte* Bitmap::getCanvas(int iWidth,int iHeight)
{
	return getNewBuffer(iWidth,iHeight);
}

/**
	@brief	時間・時刻を取り扱う。Monapi2リファレンスも参照。
	@date	2005/08/20	junjunn 作成
*/
byte* Bitmap::getNewBuffer(int iWidth,int iHeight)
{
	clear();

	m_iWidth	= iWidth;
	m_iHeight	= iHeight;
	setLineBits();

	return m_bufferData.extendBuffer(m_iLineSize * iHeight);
}

/**
	@brief	時間・時刻を取り扱う。Monapi2リファレンスも参照。
	@date	2005/08/20	junjunn 作成
*/
void Bitmap::setLineBits()
{
	m_iLineSize = getBMPLineSize(m_iWidth);
}

/**
	@brief	時間・時刻を取り扱う。Monapi2リファレンスも参照。
	@date	2005/08/20	junjunn 作成
*/
void Bitmap::copy(const Bitmap* cpBitmap)
{
	if (! cpBitmap->isValid())	return;

	clear();

	m_iWidth	= cpBitmap->m_iWidth;
	m_iHeight	= cpBitmap->m_iHeight;
	m_bufferData.copy(&cpBitmap->m_bufferData);
}

/**
	@brief	時間・時刻を取り扱う。Monapi2リファレンスも参照。
	@date	2005/08/20	junjunn 作成
*/
byte* Bitmap::getPixelBuffer(int x,int y)
{
//先頭を表す引数。
	if (x<0)	return m_bufferData.getBuffer();

	int iOffset = (m_iHeight - y - 1)*m_iLineSize + x*3;
	return m_bufferData.getBuffer() + iOffset;
}

/**
	@brief	時間・時刻を取り扱う。Monapi2リファレンスも参照。
	@date	2005/08/20	junjunn 作成
*/
colort Bitmap::getPixel(int x,int y)
{
	const byte* cpBaseByte = getPixelBufferConst(x,y);
	return makeColor(cpBaseByte[0],cpBaseByte[1],cpBaseByte[2]);
}


/**
	@brief	時間・時刻を取り扱う。Monapi2リファレンスも参照。
	@date	2005/08/20	junjunn 作成
*/
void Bitmap::setPixel(int x,int y,colort color)
{
	ASSERT_PIXEL(x,y);

	byte* pBaseByte = getPixelBuffer(x,y);

	pBaseByte[0] = getBValue(color);
	pBaseByte[1] = getGValue(color);
	pBaseByte[2] = getRValue(color);
}

/**
	@brief	時間・時刻を取り扱う。Monapi2リファレンスも参照。
	@date	2005/08/20	junjunn 作成
*/
void Bitmap::fill(colort color)
{
	byte* p = getPixelBuffer(-1,0);
//dword転送では最後の一つだけはバッファの外に出てしまうので。
	for (int i=0;i<(m_iWidth*m_iHeight)-1;i++)
	{
		*(dword*)p = color;
		p+=3;
	}

	setPixel(m_iWidth-1,m_iHeight-1,color);
}

/**
	@brief	時間・時刻を取り扱う。Monapi2リファレンスも参照。
	@date	2005/08/20	junjunn 作成
*/
bool Bitmap::read(cpchar1 cszPath)
{
	File file;
	if (! file.open(cszPath))		return false;

//BMPファイルヘッダの読み込み
	int iRead=0;
	BitmapFileHeader bitmapFileHeader;
	BitmapInfoHeader bitmapInfoHeader;

	byte abyBuffer[100];
	iRead+=file.read(abyBuffer,iRead,14);
	bitmapFileHeader.set(abyBuffer);

	iRead+=file.read(abyBuffer,iRead,40);
	bitmapInfoHeader.set(abyBuffer);

//現時点では24ビット以外は対応してない。
	if (bitmapInfoHeader.m_wBitFormat != 24)		return false;
	ASSERT(m_iWidth>0 && m_iHeight>0);


//初期化。設定。
	m_strPath = cszPath;
	byte* pBuffer = getCanvas(bitmapInfoHeader.m_iWidth,bitmapInfoHeader.m_iHeight);

	file.read(pBuffer,bitmapFileHeader.m_dwOffsetToData,m_bufferData.getSize());

	return true;
}

/**
	@brief	時間・時刻を取り扱う。Monapi2リファレンスも参照。
	@date	2005/08/20	junjunn 作成
*/
void Bitmap::write(cpchar1 cszPath) const
{
	ASSERT(m_iBitCount==8 || m_iBitCount==24);

const int iBMP_HEADER_SIZE = 0x36;

//54バイト
	byte abyHeader[] = {
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

	iSize = m_iLineSize * m_iHeight + iBMP_HEADER_SIZE;
	iMainDataOffset = iBMP_HEADER_SIZE;

	*piSize = iSize;
	*piMainDataOffset = iMainDataOffset;
	*piWidth			= m_iWidth;
	*piHeight			= m_iHeight;
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
	int iLineBits = 3 * iWidth;

	int iExcess = iLineBits % 4;
	if (iExcess != 0)
	{
		iLineBits += (4-iExcess);
	}

	return iLineBits;
}


}	// namespace monapi2
