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

FontManager *FontManager::instance = NULL;

/** コンストラクタ */
FontManager::FontManager()
{
	// 変数初期化
	width = height = offsetListLength = 0;
	offsetList = NULL;
	fp = NULL;
	fpMemory = NULL;
	for (int i = 0; i < MAX_FONTLIST_LEN; i++) {
		fontList[i] = new Font();
	}

	// スレッドIDを得る
	threadID = MonAPI::System::getThreadID();

}

/** デストラクタ */
FontManager::~FontManager()
{
	free(offsetList);
	free(fp);
	for (int i = 0; i < MAX_FONTLIST_LEN; i++) {
		delete(fontList[i]);
	}
	free(fontList);
	monapi_cmemoryinfo_dispose(fpMemory);
	monapi_cmemoryinfo_delete(fpMemory);
}

/** インスタンスを得る */
FontManager *FontManager::getInstance()
{
	if (instance == NULL) {
		instance = new FontManager();
		// 全角フォント読み込み
		instance->loadFont(FONTFILE_NAME);
	}
	return instance;
}

/** 全角フォントロード */
void FontManager::loadFont(char *path)
{
	int i, fw, fh, pos = 0;
	MessageInfo info;

	// GUIサーバーを探す
	dword guisvrID = MonAPI::Message::lookupMainThread(GUISERVER_NAME);
	if (guisvrID == 0xFFFFFFFF) {
		//printf("FontManager: GuiServer not found %d\n", guisvrID);
		fp = NULL;
		return;
	} else {
		//printf("FontManager: GuiServer found %d\n", guisvrID);
	}
	
	// 共有メモリーからフォントをロード
	if (guisvrID == threadID) {
		// フォント初期化
		fpMemory = monapi_call_file_decompress_bz2_file(path, false);
		// サーバーは直接ロード
		fp = fpMemory->Data;
	} else {
		// フォント取得メッセージを投げる
		MonAPI::Message::sendReceive(&info, guisvrID, MSG_GUISERVER_GETFONT, 0, 0, 0, NULL);
		//printf("WindowManager->FontManager: MSG_GUISERVER_GETFONT received %d, %d\n", info.arg2, info.arg3);
		byte* font_data = MonAPI::MemoryMap::map(info.arg2);
		if (font_data == NULL) {
			//printf("FontManager: cannot get font data\n");
			fp = NULL;
			return;
		} else {
			fp = (unsigned char *)malloc(info.arg3);
			memcpy(fp, font_data, info.arg3);
			MonAPI::MemoryMap::unmap(info.arg2);
		}
	}
	
	if (fp != NULL) {
		xstrncpy(this->name, FONT_NAME, 8 + 1);
		// 幅と高さ
		width  = 6;
		height = 12;
		offsetListLength = inGetUInt16(&fp[0]);
		// 文字までのオフセット(byte)
		offsetList = (int *)malloc(65536);
		memset(offsetList, 4, 65536);
		pos += 4;
		// debug
		//printf("open file %s [name = %s, w = %d, h = %d]\n", path, name, width, height);
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
 展開済みフォント列を得る
 @param [in] str 文字列 (UTF-8)
 @param [out] length 文字数
 @return フォント列
*/
Font **FontManager::decodeString(char *str, int *length)
{
	int i , j, n = 0, len = 0;
	unsigned char c1 = 0, c2 = 0, c3 = 0;

	for (i = 0; i < (int)strlen(str); i++) {
		// 文字列の終端
		if (str[i] == 0) break;
		// 1 バイト目
		c1 = (unsigned char)str[i];
		// 0aaa bbbb - > 0aaa bbbb (0x20-0x7F)
		if (0x20 <= c1 && c1 <= 0x7F) {
			n = c1;
		// 110a aabb 10bb cccc -> 0000 0aaa bbbb cccc (0xC280-0xDFBF)
		} else if (0xC2 <= c1 && c1 <= 0xDF) {
			// 2 バイト目
			if (str[i] == strlen(str) - 1) break;
			c2 = (unsigned char)str[++i];
			n = ((c1 & 0x1F) >> 6) | (c2 & 0x3F);
		// 1110 aaaa 10bb bbcc 10cc dddd -> aaaa bbbb cccc dddd (0xE0A080-0xEFBFBF)
		} else if (0xE0 <= c1 && c1 <= 0xEF) {
			// 2 バイト目
			if (str[i] == strlen(str) - 1) break;
			c2 = (unsigned char)str[++i];
			// 3 バイト目
			if (str[i] == strlen(str) - 1) break;
			c3 = (unsigned char)str[++i];
			n = ((c1 & 0xF) << 12) | ((c2 & 0x3F) << 6) | (c3 & 0x3F);
		} else {
			*length = 0;
			return NULL;
		}
		// デコード開始
		if (0 < n && n <= 0xFFFF && fp != NULL) {
			int fw = fp[offsetList[n] + 4];
			int fh = fp[offsetList[n] + 5];
			fontList[len]->setName(this->name);
			fontList[len]->setWidth(fw);
			fontList[len]->setHeight(fh);
			memcpy(fontList[len]->getData(), &fp[offsetList[n] + 6],
				(int)((fw * fh + 7) / 8));
			len++;
		}
	}
	
	*length = len;
	return fontList;
}

/**
 文字列の幅を得る
 @param str 文字列 (UTF-8)
*/
int FontManager::getWidth(char *str)
{
	int i, len, w = 0;
	
	decodeString(str, &len);
	for (i = 0; i < len; i++) {
		w += fontList[i]->getWidth();
	}
	
	return w;
}

/**
 文字列の高さを得る
*/
int FontManager::getHeight()
{
	return height;
}
