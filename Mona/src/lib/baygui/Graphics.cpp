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

#include "baygui.h"

/** コンストラクタ */
Graphics::Graphics()
{
	tx = ty = cx = cy = cw = ch = 0;
	rgb24 = 0;
	fontStyle = FONT_PLAIN;
	this->image = NULL;
}

/** コンストラクタ */
Graphics::Graphics(Image *image)
{
	tx = ty = cx = cy = 0;
	rgb24 = 0;
	fontStyle = FONT_PLAIN;
	this->image = image;
	this->cw = image->getWidth();
	this->ch = image->getHeight();
}

/** デストラクタ */
Graphics::~Graphics()
{
}

/**
 点描画
 @param x X座標
 @param y Y座標
 @param color 描画する色
 */
void Graphics::drawPixel(int x, int y, unsigned int color)
{
	this->image->setPixel(tx + x, ty + y, color);
}

/**
 イメージ描画
 @param image イメージ
 @param x 描画開始X座標
 @param y 描画開始Y座標
 */
void Graphics::drawImage(Image *image, int x, int y)
{
	for (int i = 0; i < image->getHeight(); i++) {
		for (int j = 0; j < image->getWidth(); j++) {
			drawPixel(j + x, i + y, image->getPixel(j, i));
		}
	}
}

/**
 直線描画
 @param x1 始点X
 @param y1 始点Y
 @param x2 終点X
 @param y2 終点Y
 */
void Graphics::drawLine(int x1, int y1, int x2, int y2)
{
	int dx = (x1 > x2) ? (x1 - x2) : (x2 - x1);
	int dy = (y1 > y2) ? (y1 - y2) : (y2 - y1);
	
	if (dx == 0 && dy == 0) {
		this->drawPixel(x1, y1, this->rgb24);
	} else if (dx >= dy) {
		if (x1 > x2) {
			int tx = x1; x1 = x2; x2 = tx;
			int ty = y1; y1 = y2; y2 = ty;
		}
		for (int x = x1; x <= x2; x++) {
			this->drawPixel(x, ((2 * y1 + 2 * (y2 - y1) * (x - x1) / (x2 - x1)) + 1) / 2, this->rgb24);
		}
	} else {
		if (y1 > y2) {
			int tx = x1; x1 = x2; x2 = tx;
			int ty = y1; y1 = y2; y2 = ty;
		}
		for (int y = y1; y <= y2; y++) {
			this->drawPixel((2 * x1 + 2 * (x2 - x1) * (y - y1) / (y2 - y1) + 1) / 2, y, this->rgb24);
		}
	}
}

/**
 矩形描画
 @param x 始点X
 @param y 始点Y
 @param width 幅
 @param height 高さ
 */
void Graphics::drawRect(int x, int y, int width, int height)
{
	if (width < 0) {
		x += width;
		width = -width;
	}
	if (height < 0) {
		y += height;
		height = -height;
	}
	int xw = x + width - 1, yh = y + height - 1;
	for (int xx = x; xx <= xw; xx++) {
		this->drawPixel(xx, y , this->rgb24);
		this->drawPixel(xx, yh, this->rgb24);
	}
	for (int yy = y + 1; yy <= yh - 1; yy++) {
		this->drawPixel(x , yy, this->rgb24);
		this->drawPixel(xw, yy, this->rgb24);
	}
}

/**
 円描画
 @param x0 中心X
 @param y0 中心Y
 @param r  半径
 */
void Graphics::drawCircle(int x0, int y0, int r){
	int x, y, f;
	x = r;
	y = 0;
	f = -2 * r + 3;

	while (x >= y) {
		drawPixel(x0 + x, y0 + y, rgb24);
		drawPixel(x0 - x, y0 + y, rgb24);
		drawPixel(x0 + y, y0 + x, rgb24);
		drawPixel(x0 - y, y0 + x, rgb24);
		drawPixel(x0 - x, y0 - y, rgb24);
		drawPixel(x0 - y, y0 - x, rgb24);
		drawPixel(x0 + y, y0 - x, rgb24);
		drawPixel(x0 + x, y0 - y, rgb24);
		if (f >= 0) {
			x--;
			f -= 4 * x;
		}
		y ++;
		f += 4 * y + 2;
	}
}

/**
 文字列描画
 @param str 文字列
 @param x 始点X
 @param y 始点Y
 */
void Graphics::drawText(char *str, int x, int y)
{
	int i, j, k, pos, bit, width, height, w = 0, h = 0;
	FontMetrics metrics;
	
	// NULLチェック
	if (str == NULL || strlen(str) == 0) return;
	
	String s = str;
	
	for (i = 0; i < s.length(); i++) {
		pos = 0;
		bit = 1;
		char fp[256];
		// 改行
		if (s[i] == '\n') {
			w = 0;
			h += 12;
		}
		if (metrics.decodeCharacter(s[i], &width, &height, fp) == true) {
			for (j = 0; j < height; j++) {
				for (k = 0; k < width; k++) {
					// 行パディングなし
					if ((fp[pos] & bit) != 0) {
						// 通常書体
						if (getFontStyle() == FONT_PLAIN) {
							drawPixel(x + w + k, y + h + j, rgb24);
						// 太字体
						} else if (getFontStyle() == FONT_BOLD) {
							drawPixel(x + w + k, y + j, rgb24);
							drawPixel(x + w + k + 1, y + j, rgb24);
						// 斜字体
						} else if (getFontStyle() == FONT_ITALIC) {
							drawPixel(x + w + k + (height - j) / 4, y + j, rgb24);
						// 太字体＋斜字体
						} else if (getFontStyle() == FONT_BOLD | FONT_ITALIC) {
							drawPixel(x + w + k + (height - j) / 4, y + j, rgb24);
							drawPixel(x + w + k + (height - j) / 4 + 1, y + j, rgb24);
						}
					}
					bit <<= 1;
					if (bit == 256) {
						pos++;
						bit = 1;
					}
				}
			}
			w += width;
		}
	}
}

/**
 円塗りつぶし描画描画
 @param x0 中心X
 @param y0 中心Y
 @param r  半径
 */
void Graphics::fillCircle(int x0, int y0, int r)
{
	int i;
	int x, y;
	x = r;
	y = 0;
	int f = -2 * r + 3;

	while(x >= y) {
		for (i = x0 - x; i <= x0 + x; i ++) {
			drawPixel(i, y0 + y, rgb24);
			drawPixel(i, y0 - y, rgb24);
		}
		for (i = x0 - y; i <= x0 + y; i ++) {
			drawPixel(i, y0 - x, rgb24);
			drawPixel(i, y0 + x, rgb24);
		}
		if (f >= 0) {
			x--;
			f -= 4 * x;
		}
		y ++;
		f += 4 * y + 2;
	}
}

/**
 矩形ぬりつぶし描画
 @param x 始点X
 @param y 始点Y
 @param width 幅
 @param height 高さ
 */
void Graphics::fillRect(int x, int y, int width, int height)
{
	if (width < 0) {
		x += width;
		width = -width;
	}
	if (height < 0) {
		y += height;
		height = -height;
	}
	
	int xw = x + width, yh = y + height;
	
	for (int yy = y; yy < yh; yy++) {
		for (int xx = x; xx < xw; xx++) {
			this->drawPixel(xx, yy, this->rgb24);
		}
	}
}

/**
 座標設定
 @param x 始点X
 @param y 始点Y
 */
void Graphics::translate(int x, int y)
{
	tx = x;
	ty = y;
}

/**
 クリッピング領域設定
 @param cx 始点X
 @param cy 始点Y
 @param cw 終点X
 @param ch 終点Y
 */
void Graphics::setClip(int cx, int cy, int cw, int ch)
{
	this->cx = cx;
	this->cy = cy;
	this->cw = cw;
	this->ch = ch;
}

/**
 色設定
 @param r 赤(0-255)
 @param g 緑(0-255)
 @param b 青(0-255)
 */
void Graphics::setColor(unsigned char r, unsigned char g, unsigned char b)
{
	rgb24 = 0xff000000 | r << 16 | g << 8 | b;
}

/**
 色設定
 @param color (0x0-0xFFFFFF)
 */
void Graphics::setColor(unsigned int color)
{
	rgb24 = 0xff000000 | color;
}

/**
 フォントスタイル（通常、太字、斜字、太字＆斜字）を設定する
 @param style フォントスタイル (FONT_PLAIN/FONT_BOLD/FONT_ITALIC/FONT_BOLD|FONT_ITALIC)
 */
void Graphics::setFontStyle(int style)
{
	this->fontStyle = style;
}
