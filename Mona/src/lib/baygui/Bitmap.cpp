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

/**
 コンストラクタ.
 内部にwidth*height*4バイトのバッファーを作る。
 @param width 幅
 @param height 高さ
 */
Bitmap::Bitmap(int width, int height)
{
	this->width = width;
	this->height = height;
	data = (unsigned int *)malloc(width * height * 4);
}

/**
 コンストラクタ
 @param path ファイル名
 */
Bitmap::Bitmap(char *path)
{
	decode(getByteArray(path));
}

/** デストラクタ */
Bitmap::~Bitmap()
{
	free(data);
}

/**
 BMP形式ファイルのデコード
 @param fp ファイルバッファーポインター
 */
bool Bitmap::decode(unsigned char *fp)
{
	unsigned int bitmapOffset, infoSize, bpp, compression, colors, scanlen;
	unsigned int palette256[256];
	unsigned char *ip, c;
	int x, y, i;

	// header (54 bytes)
	// 0-1   magic chars 'BM'
	// 2-5   uint32 filesize (not reliable)
	// 6-7   uint16 0
	// 8-9   uint16 0
	// 10-13 uint32 bitmapOffset
	// 14-17 uint32 info size
	// 18-21 uint32 width
	// 22-25 uint32 height
	// 26-27 uint16 nplanes
	// 28-29 uint16 bits per pixel
	// 30-33 uint32 compression flag
	// 34-37 uint32 image size in bytes
	// 38-41 uint32 biXPelsPerMeter
	// 32-45 uint32 biYPelsPerMeter
	// 46-49 uint32 colors used
	// 50-53 uint32 important color count

	if (fp == NULL)
		return false; // NULL Check
	if (fp[0] != 'B' || fp[1] != 'M')
		return false; // not a BMP file
	infoSize = inGetUInt32(&fp[14]);
	if (infoSize != 40)
		return false; // old-style BMP
	width = inGetUInt32(&fp[18]);
	height = inGetUInt32(&fp[22]);
	if (width > 65535 || height > 65535)
		return false; // bad width/height
	bpp = inGetUInt16(&fp[28]);
	if (bpp != 1 && bpp != 4 && bpp != 8)
		return false; // not a 2, 16 or 256 color image
	compression = inGetUInt32(&fp[30]);
	if (compression != 0)
		return false; // compressed image
	colors = inGetUInt32(&fp[46]);
	bitmapOffset = inGetUInt32(&fp[10]);
	
	// set default system color
	palette256[0]  = 0x000000; palette256[1]  = 0x800000;
	palette256[2]  = 0x008000; palette256[3]  = 0x808000;
	palette256[4]  = 0x000080; palette256[5]  = 0x800080;
	palette256[6]  = 0x008080; palette256[7]  = 0xC6C6C6;
	palette256[8]  = 0x808080; palette256[9]  = 0xff0000;
	palette256[10] = 0x00ff00; palette256[11] = 0xffff00;
	palette256[12] = 0x0000ff; palette256[13] = 0xff00ff;
	palette256[14] = 0x00ffff; palette256[15] = 0xffffff;
	
	// initialize color palette
	for (i = 0; i < (int)colors; i++){
		palette256[i] = inGetUInt32(&fp[54 + i * 4]);
	}
	
	// allocate image buffer
	data = (unsigned int *)malloc(width * height * 4);
	
	// calcurate scanlen
	scanlen = (int)((width * bpp + 31) / 32) * 4;
	
	// start decode
	ip = &fp[bitmapOffset];
	for (y = 0; y < (int)height; y++) {
		if (bpp == 1) {
			for (x = 0; x < (int)(width/8); x++) {
				c = *ip;
				for (i = 7; i >= 0; i--) {
					if (((c >> i) & 1) == 1) {
						//sys_gs_set_pixel_RGB(x * 8 + (7 - i), height - y - 1, 0xffffff);
						data[width * (height - y - 1) + (x * 8 + (7 - i))] = 0xffffff;
					} else {
						//sys_gs_set_pixel_RGB(x * 8 + (7 - i), height - y - 1, 0x000000);
						data[width * (height - y - 1) + (x * 8 + (7 - i))] = 0x000000;
					}
				}
				ip++;
			}
			ip += (scanlen - width/8);
		} else if (bpp == 4) {
			for (x = 0; x < (int)(width/2); x++) {
				c = *ip;
				//sys_gs_set_pixel_RGB(x * 2, height - y - 1, palette256[(c >> 4) & 0xf]);
				//sys_gs_set_pixel_RGB(x * 2 + 1, height - y - 1, palette256[c & 0xf]);
				data[width * (height - y - 1) + (x * 2)] = palette256[(c >> 4) & 0xf];
				data[width * (height - y - 1) + (x * 2 + 1)] = palette256[c & 0xf];
				ip++;
			}
			ip += (scanlen - width/2);
		} else if (bpp == 8) {
			for (x = 0; x < (int)width; x++) {
				c = *ip;
				//sys_gs_set_pixel_RGB(x, height - y - 1, palette256[c & 0xff]);
				data[width * (height - y - 1) + (x)] = palette256[c & 0xff];
				ip++;
			}
			ip += (scanlen - width);
		}
	}

	return true;
}

/** 画像のバッファーを得る */
unsigned int *Bitmap::getData()
{
	return data;
}

/** 画像のタイプを得る */
int Bitmap::getType()
{
	return BITMAP_IMAGE;
}
