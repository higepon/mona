/*
Copyright (c) 2005 bayside

Permission is hereby granted, free of charge, to any person 
obtaining a copy of this software and associated documentation files 
(the "Software"), to deal in the Software without restriction, 
including without limitation the rights to use, copy, modify, merge, 
publish, distribute, sublicense, and/or sell copies of the Software, 
and to permit persons to whom the Software is furnished to do so, 
subject to the following conditions:

The above copyright notice and this permission notice shall be 
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY 
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "baygui.h"

namespace baygui {
	// フォントデータとオフセットリストは毎回生成・開放していると遅くなるのでstaticで保持する
	int FontMetrics::offsetListLength = 0;
	int* FontMetrics::offsetList = NULL;
	unsigned char* FontMetrics::defaultFontData = NULL;

	FontMetrics::FontMetrics()
	{
		this->fontStyle = Font::PLAIN;
		if (defaultFontData != NULL) return;
		
	#if defined(MONA)
		// フォント取得メッセージを投げる
		MessageInfo info;
		MonAPI::Message::sendReceive(&info, getGuisvrID(), MSG_GUISERVER_GETFONT, 0, 0, 0, NULL);
		unsigned char* font_data = NULL;
		font_data = MonAPI::MemoryMap::map(info.arg2);
		if (font_data == NULL) {
			printf("%s:%d:ERROR: can not get font!\n", __FILE__, __LINE__);
			exit(1);
		}
		defaultFontData = (unsigned char *)malloc(info.arg3);
		memcpy(defaultFontData, font_data, info.arg3);
		MonAPI::MemoryMap::unmap(info.arg2);
	#elif defined(SDL)
		// フォントを開く
		FILE* fp;
		if((fp = fopen("./MONA12.MNF", "rb")) == NULL ) {
			printf("%s:%s:%d:ERROR: can not get font!\n", __FILE__, __FUNCTION__, __LINE__);
			fclose(fp);
			exit(-1);
		}
		fseek(fp, 0, SEEK_END);
		long len = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		// フォントを読む
		this->defaultFontData = (unsigned char *)malloc(len);
		if(fread(this->defaultFontData, 1, len, fp) <= 0) {
			printf("%s:%s:%d:ERROR: can not get font!\n", __FILE__, __FUNCTION__, __LINE__);
			fclose(fp);
			exit(-1);
		} else {
			printf("%s:%s:%d: font loaded (%d bytes)\n", __FILE__, __FUNCTION__, __LINE__, (int)len);
			fclose(fp);
		}
	#endif
		
		// モナーフォントの構造
		// 3 - 0 フォント数
		// ? - 3 フォント構造体 ( ? = フォント数 )
		// 
		// フォント構造体
		// 3 - 0 UCS-4 (UTF-16)
		//     4 幅 (fw)
		//     5 高さ (fh)
		// ? - 6 ビットパターン ( ? = (fw * fh + 7) / 8 )
		
		offsetListLength = inGetUInt16(&defaultFontData[0]);
		
		// 文字までのオフセット(byte)
		offsetList = (int *)malloc(65536 * sizeof(int));
		memset(offsetList, 0, 65536 * sizeof(int));
		
		// オフセットリストを作る
		int pos = 4;
		for (int i = 0; i < offsetListLength; i++) {
			offsetList[inGetUInt16(&defaultFontData[pos])] = pos;
			pos += 4;
			int fw = defaultFontData[pos++];
			int fh = defaultFontData[pos++];
			pos += (int)((fw * fh + 7) / 8);
		}
	}

	FontMetrics::~FontMetrics()
	{
	}

	bool FontMetrics::decodeCharacter(wchar ucs4, int* offset, int* width, int* height, char* data)
	{
		if (ucs4 <= 0xFFFF && defaultFontData != NULL && offsetList[ucs4] != 0) {
			int fw = defaultFontData[offsetList[ucs4] + 4];
			int fh = defaultFontData[offsetList[ucs4] + 5];
			//printf("fontStyle = %d,", this->fontStyle);
			if ((this->fontStyle & 0x100) == Font::FIXED) {
				if (ucs4 < 128 || 0xff60 < ucs4) {
					*offset = 8;
				} else {
					*offset = 12;
				}
			} else {
				*offset = fw;
			}
			*width  = fw;
			*height = fh;
			memcpy(data, &defaultFontData[offsetList[ucs4] + 6], (int)((fw * fh + 7) / 8));
			return true;
		} else {
			return false;
		}
	}

	int FontMetrics::getWidth(String str)
	{
		// NULLチェック
		if (str.length() == 0 || defaultFontData == NULL) return 0;
		
		int w = 0;
		for (int i = 0; i < str.length(); i++) {
			wchar c = str.charAt(i);
			if (c == '\n') {
				break;
			}
			if (c <= 0xFFFF) {
				if ((this->fontStyle & 0x100) == Font::FIXED) {
					if (c < 128 || 0xff60 < c) {
						w += 8;
					} else {
						w += 12;
					}
				} else {
					w += defaultFontData[offsetList[c] + 4];
				}
			}
		}
		
		return w;
	}

	int FontMetrics::getHeight(String str)
	{
		int h = 12;
		for (int i = 0; i < str.length(); i++) {
			if (str.charAt(i) == '\n') {
				h += 12; // モナーフォントは高さが12ドット
			}
		}
		
		return h;
	}
}
