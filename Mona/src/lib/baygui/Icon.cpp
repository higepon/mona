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
 内部にwidth*height*4バイトのバッファーを作る
 @param width 幅
 @param height 高さ
 */
Icon::Icon(int width, int height)
{
	this->width = width;
	this->height = height;
	data = (unsigned int *)malloc(width * height * 4);
}

/**
 コンストラクタ
 @param path ファイル名
 */
Icon::Icon(char *path)
{
	decode(getByteArray(path));
}

/** デストラクタ */
Icon::~Icon()
{
	free(data);
}

/**
 ICO形式ファイルのデコード
 @param fp ファイルバッファーポインター
 */
bool Icon::decode(unsigned char *fp)
{
	unsigned int bitmapOffset, infoSize, bpp, compression, colors, scanlen;
	unsigned int palette256[256];
	unsigned char *ip, c;
	int x, y, i;

	// header (62 bytes)
	// 0-1   uint16 reserved (not reliable)
	// 2-3   uint16 1
	// 4-5   uint16 entries
	// 6     uchar  width
	// 7     uchar  height
	// 8     uchar  colors (0 is 256)
	// 9     uchar  reserved (not reliable)
	// 10-11 uint16 nplanes
	// 12-13 uint16 bits per pixel
	// 14-17 uint32 resource size
	// 18-21 uint32 image offset
	// 22-25 uint32 info size
	// 26-29 uint32 width
	// 30-33 uint32 height
	// 34-35 uint16 nplanes
	// 36-37 uint16 bits per pixel
	// 38-41 uint32 compression flag
	// 42-45 uint32 image size in bytes
	// 46-49 uint32 biXPelsPerMeter
	// 50-53 uint32 biYPelsPerMeter
	// 54-57 uint32 colors used
	// 58-61 uint32 important color count

	if (fp == NULL)
		return false; // NULL Check
	infoSize = inGetUInt32(&fp[22]);
	if (infoSize != 40)
		return false; // old-style BMP
	width = inGetUInt32(&fp[26]);
	height = inGetUInt32(&fp[30]);
	height = height/2; // icon has double height size
	if (width > 65535 || height > 65535)
		return false; // bad width/height
	bpp = inGetUInt16(&fp[36]);
	if (bpp != 1 && bpp != 4 && bpp != 8)
		return false; // not a 2, 16 or 256 color image
	compression = inGetUInt32(&fp[38]);
	if (compression != 0)
		return false; // compressed image
	colors = inGetUInt32(&fp[54]);
	if (colors == 0) colors = 256;
	bitmapOffset = 62 + (colors * 4);

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
		palette256[i] = inGetUInt32(&fp[62 + i * 4]);
	}
	
	// allocate image buffer
	data = (unsigned int *)malloc(width * height * 4);

	// calcurate scanlen
	scanlen = (int)((width * bpp + 31) / 32) * 4;
	
	// start data decode
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
	
	// mask
	unsigned int transColor = data[0];
	for (i = 0; i < (int)(width * height); i++) {
		if (transColor == data[i]) {
			data[i] |= 0xFF000000;
		}
	}
	
	return true;
}
