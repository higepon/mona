/*
Copyright (c) 2004 bayside
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. The name of the author may not be used to endorse or promote products
   derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#if !defined(_GRAPHICS_H_INCLUDED_)
#define _GRAPHICS_H_INCLUDED_

/**
 描画クラス
*/
class Graphics : public Object {
private:
	int tx, ty;
	int cx, cy, cw, ch;
	int width, height;
	/** 色（4バイト） */
	unsigned int rgb24;
	/** フォントスタイル */
	int fontStyle;
	/** 内部バッファー */
	Image *image;

public:
	/** コンストラクタ */
	Graphics::Graphics();

	/**
	 コンストラクタ
	 @param image 内部バッファー
	*/
	Graphics::Graphics(Image *image);
	
	/** デストラクタ */
	virtual Graphics::~Graphics();
	
	/**
	 点描画
	 @param x X座標
	 @param y Y座標
	 @param color 描画する色
	 */
	void drawPixel(int x, int y, unsigned int color);
	
	/**
	 イメージ描画
	 @param image イメージ
	 @param x 描画開始X座標
	 @param y 描画開始Y座標
	 */
	void drawImage(Image *image, int x, int y);
	
	/**
	 直線描画
	 @param x0 始点X
	 @param y0 始点Y
	 @param x1 終点X
	 @param y1 終点Y
	 */
	void drawLine(int x0, int y0, int x1, int y1);
	
	/**
	 矩形描画
	 @param x 始点X
	 @param y 始点Y
	 @param width 幅
	 @param height 高さ
	 */
	void drawRect(int x, int y, int width, int height);
	
	/**
	 円描画
	 @param x0 中心X
	 @param y0 中心Y
	 @param r  半径
	 */
	void drawCircle(int x0, int y0, int r);
	
	/**
	 文字列描画
	 @param str 文字列（複数行対応）
	 @param x 始点X
	 @param y 始点Y
	 */
	void drawText(char *str, int x, int y);
	
	/**
	 円塗りつぶし描画
	 @param x0 中心X
	 @param y0 中心Y
	 @param r  半径
	 */
	void fillCircle(int x0, int y0, int r);
	
	/**
	 矩形ぬりつぶし描画
	 @param x 始点X
	 @param y 始点Y
	 @param width 幅
	 @param height 高さ
	 */
	void fillRect(int x, int y, int width, int height);
	
	/**
	 座標設定
	 @param x 始点X
	 @param y 始点Y
	 */
	void translate(int x, int y);
	
	/** 幅を得る */
	inline int getWidth() { return this->width; }
	
	/** 高さを得る */
	inline int getHeight() { return this->height; }
	
	/** フォントスタイルを得る */
	inline int getFontStyle() { return this->fontStyle; }
	
	/**
	 クリッピング領域設定
	 @param cx 始点X
	 @param cy 始点Y
	 @param cw 終点X
	 @param ch 終点Y
	 */
	void setClip(int cx, int cy, int cw, int ch);
	
	/**
	 色設定
	 @param r 赤(0-255)
	 @param g 緑(0-255)
	 @param b 青(0-255)
	 */
	void setColor(unsigned char r, unsigned char g, unsigned char b);
	
	/**
	 色設定
	 @param color (0x0-0xFFFFFF)
	 */
	void setColor(unsigned int color);
	
	/**
	 フォントスタイル（通常、太字、斜字、固定幅）を設定する.
	 各スタイルを"|"で組み合わせることも可能。
	 @param style フォントスタイル (Font::PLAIN / Font::BOLD / Font::ITALIC / Font::FIXED )
	*/
	void setFontStyle(int style);
};

#endif // _GRAPHICS_H_INCLUDED_
