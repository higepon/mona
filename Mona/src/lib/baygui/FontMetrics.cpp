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

FontMetrics *FontMetrics::instance = NULL;

/** コンストラクタ */
FontMetrics::FontMetrics()
{
	// 変数初期化
	width = height = offsetListLength = 0;
	offsetList = NULL;
	fp = NULL;
}

/** デストラクタ */
FontMetrics::~FontMetrics()
{
	free(offsetList);
	free(fp);
}

/** インスタンスを得る */
FontMetrics *FontMetrics::getInstance()
{
	if (instance == NULL) {
		instance = new FontMetrics();
		// 全角フォント読み込み
		instance->loadFont(FONTFILE_NAME);
	}
	return instance;
}

/** 全角フォントロード */
void FontMetrics::loadFont(char *path)
{
	int i, fw, fh, pos = 0;
	MessageInfo info;

	// IMEサーバーを探す
	dword imesvrID = MonAPI::Message::lookupMainThread(IMESERVER_NAME);
	if (imesvrID == THREAD_UNKNOWN) {
		return;
	}
	
	// フォント取得メッセージを投げる
	MonAPI::Message::sendReceive(&info, imesvrID, MSG_IMESERVER_GETFONT, 0, 0, 0, NULL);
	//printf("WindowManager->FontMetrics: MSG_IMESERVER_GETFONT received %d, %d\n", info.arg2, info.arg3);
	byte* font_data = MonAPI::MemoryMap::map(info.arg2);
	if (font_data == NULL) {
		//printf("FontMetrics: cannot get font data\n");
		fp = NULL;
		return;
	} else {
		fp = (unsigned char *)malloc(info.arg3);
		memcpy(fp, font_data, info.arg3);
		MonAPI::MemoryMap::unmap(info.arg2);
	}
	
	if (fp != NULL) {
		// 幅と高さ
		width  = 6;
		height = 12;
		offsetListLength = inGetUInt16(&fp[0]);
		// 文字までのオフセット(byte)
		offsetList = (int *)malloc(65536);
		memset(offsetList, 0, 65536);
		pos += 4;
		// debug
		//printf("open file %s [w = %d, h = %d]\n", path, width, height);
		// オフセットリストを作る
		for (i = 0; i < offsetListLength; i++) {
			offsetList[inGetUInt16(&fp[pos])] = pos;
			pos += 4;
			fw = fp[pos++];
			fh = fp[pos++];
			pos += (int)((fw * fh + 7) / 8);
		}
		return;
	} else {
		printf("[ERROR] cant open file %s\n", path);
		return;
	}
}

/**
 フォントビットデータを得る
 @param [in] utf16 文字コード (UTF-16)
 @param [out] width 文字幅
 @param [out] height 文字高さ
 @param [out] data フォントビットデータ
 @return 成功ならtrue、失敗ならfalse
*/
bool FontMetrics::decodeCharacter(unsigned int utf16, int *width, int *height, char *data)
{
	if (fp != NULL && 0 < utf16 && utf16 <= 0xFFFF && offsetList[utf16] != 0) {
		int fw = fp[offsetList[utf16] + 4];
		int fh = fp[offsetList[utf16] + 5];
		*width  = fw;
		*height = fh;
		memcpy(data, &fp[offsetList[utf16] + 6], (int)((fw * fh + 7) / 8));
		return true;
	} else {
		return false;
	}
}

/**
 文字列を文字コード列に変換する
 @param [in] str 文字列
 @param [in] encoding 出力したいエンコーディング（デフォルトはUTF16）
 @param [out] 文字コード列
 @param [out] 文字コード列のサイズ
*/
void FontMetrics::getCharacterCode(char *str, int encoding, int *list, int *length)
{
	int i , n = 0, w = 0;
	unsigned char c1 = 0, c2 = 0, c3 = 0;

	for (i = 0; i < (int)strlen(str); i++) {
		// 文字列の終端
		if (str[i] == 0) break;
		// 1 バイト目
		c1 = (unsigned char)str[i];
		// 0aaa bbbb - > 0aaa bbbb (0x20-0x7F)
		if (0x20 <= c1 && c1 <= 0x7F) {
			n = c1;
			list[w++] = n;
		// 110a aabb 10bb cccc -> 0000 0aaa bbbb cccc (0xC280-0xDFBF)
		} else if (0xC2 <= c1 && c1 <= 0xDF) {
			// 2 バイト目
			if (str[i] == (int)strlen(str) - 1) break;
			c2 = (unsigned char)str[++i];
			n = ((c1 & 0x1F) << 6) | (c2 & 0x3F);
			list[w++] = n;
		// 1110 aaaa 10bb bbcc 10cc dddd -> aaaa bbbb cccc dddd (0xE0A080-0xEFBFBF)
		} else if (0xE0 <= c1 && c1 <= 0xEF) {
			// 2 バイト目
			if (str[i] == (int)strlen(str) - 1) break;
			c2 = (unsigned char)str[++i];
			// 3 バイト目
			if (str[i] == (int)strlen(str) - 1) break;
			c3 = (unsigned char)str[++i];
			n = ((c1 & 0xF) << 12) | ((c2 & 0x3F) << 6) | (c3 & 0x3F);
			list[w++] = n;
		} else {
			n = 0;
		}
	}
	*length = w;
}

/**
 文字列の幅を得る
 @param str 文字列 (UTF-8)
*/
int FontMetrics::getWidth(char *str)
{
	int list[128];
	int i, w, length;
	
	// NULLチェック
	if (str == NULL || strlen(str) == 0) return 0;
	
	// 文字列を文字コード列に変換する
	w = length = 0;
	getCharacterCode(str, 0, list, &length);

	for (i = 0; i < length; i++) {
		if (fp != NULL && 0 < list[i] && list[i] <= 0xFFFF) {
			w += fp[offsetList[list[i]] + 4];
		}
	}
	
	return w;
}

/**
 文字列の高さを得る
*/
int FontMetrics::getHeight()
{
	return height;
}
