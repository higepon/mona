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
	r = g = b = rgb24 = 0;
#ifdef MONA
	screen = new MonAPI::Screen();
	width = screen->getWidth();
	height = screen->getHeight();
	bytesPerPixel = screen->getBpp() / 8;
	vram = screen->getVRAM();
	font = new Font();
	font->setName(FONT_NAME);
	font->setWidth(6);
	font->setHeight(12);
	font->setStyle(FONT_PLAIN);
#endif
	xormode = locked = false;
}

/** デストラクタ */
Graphics::~Graphics()
{
	delete(screen);
	delete(font);
}

/**
 イメージ描画
 @param image イメージ
 @param x 描画開始X座標
 @param y 描画開始Y座標
 */
void Graphics::drawImage(Image *image, int x, int y)
{
	int width  = image->getWidth();
	int height = image->getHeight();
	int I = width;
	int J = height;
	
	if (locked == true) return;
	
	// 透過イメージのためにα値が必要なのでbitbltは使えない
	unsigned int *data = ((Bitmap *)image)->getData();
	// NULL チェック
	if (data == NULL) return;
	// 範囲チェック
	if (tx + x + width > cx + cw) I = cw;
	if (ty + y + height > cy + ch) J = ch;
	// 1ドットずつ描画
	for (int j = 0; j < J; j++) {
		for (int i = 0; i < I; i++) {
			if (data[width * j + i] < 0xff000000) {
				drawPixel(x + i ,y + j, data[width * j + i]);
			}
		}
	}
}

/**
 イメージ描画
 @param image イメージ
 @param x イメージ内描画開始X座標
 @param y イメージ内描画開始Y座標
 @param w イメージ内幅
 @param h イメージ内高さ
 */
void Graphics::drawImage(Image *image, int x, int y, int w, int h)
{
	int width  = image->getWidth();
	//int height = image->getHeight();
	
	if (locked == true) return;

	// 透過イメージのためにα値が必要なのでbitbltは使えない
	unsigned int *data = ((Bitmap *)image)->getData();
	// NULL チェック
	if (data == NULL) return;
	// 1ドットずつ描画
	for (int j = y; j < y + h; j++) {
		for (int i = x; i < x + w; i++) {
			if (data[width * j + i] < 0xff000000) {
				drawPixel(i ,j, data[width * j + i]);
			}
		}
	}
}

/**
 点描画
 @param x X座標
 @param y Y座標
 @param color 描画する色
 */
void Graphics::drawPixel(int x, int y, unsigned int color)
{
	if (locked == true) return;
#if defined(PEKOE)
	sys_gs_set_pixel_RGB(tx + x, ty + y, color);
#elif defined(MONA)
	screen->putPixel16(tx + x, ty + y, color);
#endif
}

/**
 点描画（XOR描画）
 @param x X座標
 @param y Y座標
 @param color 描画する色
 */
void Graphics::drawPixelXOR(int x, int y, unsigned int color)
{
#if defined(MONA)
	// 
	// MonAPI::Screen::putPixel16 をそのまま持ってきている
	//

	byte* vramPtr = &vram[((tx + x) + (ty + y) * width) * bytesPerPixel];
	byte* colorPtr = (byte*)&color;

	if (locked == true) return;

	switch (bytesPerPixel) {
	case 2: // 16bpp
		*((word*)vramPtr) = *((word*)vramPtr) ^ MonAPI::Color::bpp24to565(color); // XOR演算
		break;
	default: // 24bpp
		vramPtr[0] = vramPtr[0] ^ colorPtr[0]; // XOR演算
		vramPtr[1] = vramPtr[0] ^ colorPtr[1]; // XOR演算
		vramPtr[2] = vramPtr[0] ^ colorPtr[2]; // XOR演算
		break;
	}
#endif
}

/**
 直線描画
 @param x0 始点X
 @param y0 始点Y
 @param x1 終点X
 @param y1 終点Y
 */
void Graphics::drawLine(int x0, int y0, int x1, int y1)
{
	int E, xx0, xx1, yy0, yy1, dx, dy, sx, sy, i;

	if (locked == true) return;

	// 領域チェック
	if (tx + x0 > cx + cw) return;
	if (ty + y0 > cy + ch) return;

	sx = ( x1 > x0 ) ? 1 : -1;
	dx = ( x1 > x0 ) ? x1 - x0 : x0 - x1;
	sy = ( y1 > y0 ) ? 1 : -1;
	dy = ( y1 > y0 ) ? y1 - y0 : y0 - y1;
	xx0 = x0;
	xx1 = x1;
	yy0 = y0;
	yy1 = y1;
	
	if ( dx >= dy ) {
		E = -dx;
		for ( i = 0; i < ( ( dx + 1 ) >> 1 ); i++ ) {
			if (xormode == true) {
				drawPixelXOR(xx0, yy0, rgb24);
				drawPixelXOR(xx1, yy1, rgb24);
			} else {
				drawPixel(xx0, yy0, rgb24);
				drawPixel(xx1, yy1, rgb24);
			}
			xx0 += sx;
			xx1 -= sx;
			E += 2 * dy;
			if ( E >= 0 ) {
				yy0 += sy;
				yy1 -= sy;
				E -= 2 * dx;
			}
		}
		if ( ( ( dx + 1 ) % 2 ) != 0 ) {
			if (xormode == true) {
				drawPixelXOR(xx0, yy0, rgb24);
			} else {
				drawPixel(xx0, yy0, rgb24);
			}
		}
	} else {
		E = -dy;
		for ( i = 0; i < ( (dy + 1) >> 1 ); i++ ) {
			if (xormode == true) {
				drawPixelXOR(xx0, yy0, rgb24);
				drawPixelXOR(xx1, yy1, rgb24);
			} else {
				drawPixel(xx0, yy0, rgb24);
				drawPixel(xx1, yy1, rgb24);
			}
			yy0 += sy;
			yy1 -= sy;
			E += 2 * dx;
			if ( E >= 0 ) {
				xx0 += sx;
				xx1 -= sx;
				E -= 2 * dy;
			}
		}
		if ( ( ( dy + 1 ) % 2 ) != 0 ) {
			if (xormode == true) {
				drawPixelXOR(xx0, yy0, rgb24);
			} else {
				drawPixel(xx0, yy0, rgb24);
			}
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
	if (locked == true) return;

	drawLine(x, y, x + width, y);
	drawLine(x, y, x, y + height);
	drawLine(x + width, y, x + width, y + height);
	drawLine(x, y + height, x + width, y + height);
}

/**
 文字列描画
 @param str 文字列
 @param x 始点X
 @param y 始点Y
 */
void Graphics::drawText(char *str, int x, int y)
{
	int len = 0;

	if (locked == true) return;

	FontManager *manager = FontManager::getInstance();
	Font **list = manager->decodeString(str, &len);
	drawText(list, len, x, y);
}

/**
 文字列描画
 @param list フォント列
 @param len 文字数（フォント列の長さ）
 @param x 始点X
 @param y 始点Y
 */
void Graphics::drawText(Font **list, int len, int x, int y) {
	int i, j, k, pos, bit, w = 0;

	if (locked == true) return;

	for (i = 0; i < len; i++) {
		pos = 0;
		bit = 1;
		unsigned char *fp = list[i]->getData();
		for (j = 0; j < list[i]->getHeight(); j++) {
			// 領域チェック
			if (tx + x + w + list[i]->getWidth() > cx + cw) return;
			for (k = 0; k < list[i]->getWidth(); k++) {
				// 行パディングなし
				if ((fp[pos] & bit) != 0) {
					drawPixel(x + w + k, y + j, rgb24);
					// 太字体
					if ((font->getStyle() & FONT_BOLD) == FONT_BOLD) {
						drawPixel(x + w + k + 1, y + j, rgb24);
					}
				}
				bit <<= 1;
				if (bit == 256) {
					pos++;
					bit = 1;
				}
			}
		}
		w += list[i]->getWidth();
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
	if (locked == true) return;

	screen->fillRect16(tx + x, ty + y, width, height, rgb24);
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

/** 画面の幅を得る */
int Graphics::getWidth()
{
	return width;
}

/** 画面の高さを得る */
int Graphics::getHeight()
{
	return height;
}

/** フォントを得る */
Font *Graphics::getFont()
{
	return font;
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
	this->r = r;
	this->g = g;
	this->b = b;
	rgb24 = r << 16 | g << 8 | b;
}

/**
 色設定
 @param color (0x0-0xFFFFFF)
 */
void Graphics::setColor(unsigned int color)
{
	this->r = (color >> 16) & 0xFF;
	this->g = (color >> 8) & 0xFF;
	this->b = color & 0xFF;
	rgb24 = color;
}

/**
 XOR描画モード設定.
 【注意】MONAでは直線描画と矩形描画のみ対応しています。
 */
void Graphics::setXORMode(bool mode)
{
	this->xormode = mode;
}

/**
 ロック、ロック解除.
 【注意】ロックされている間は描画を一切行ないません。
 */
void Graphics::setLocked(bool locked)
{
	this->locked = locked;
}

/**
 フォントを設定する.
 【注意】現状は通常文字か太字かを指定できる程度です。
 */
void Graphics::setFont(Font *font)
{
	this->font->setName(font->getName());
	this->font->setWidth(font->getWidth());
	this->font->setHeight(font->getHeight());
	this->font->setStyle(font->getStyle());
}
