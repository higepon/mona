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
	halfWidth = halfHeight = fullWidth = fullHeight = fullBlockNum = 0;
	codepage = CODE_PAGE_UTF8;
	blockStartList = blockEndList = fullOffsetList = NULL;
	halfFp = fullFp = NULL;
	fullFpMemory = NULL;
	for (int i = 0; i < MAX_FONTLIST_LEN; i++) {
		fontList[i] = new Font();
	}

	// スレッドIDを得る
	threadID = MonAPI::System::getThreadID();

}

/** デストラクタ */
FontManager::~FontManager()
{
	free(blockStartList);
	free(blockEndList);
	free(fullOffsetList);
	free(halfFp);
	free(fullFp);
	for (int i = 0; i < MAX_FONTLIST_LEN; i++) {
		delete(fontList[i]);
	}
	free(fontList);
	monapi_cmemoryinfo_dispose(fullFpMemory);
	monapi_cmemoryinfo_delete(fullFpMemory);
}

/** インスタンスを得る */
FontManager *FontManager::getInstance()
{
	if (instance == NULL) {
		instance = new FontManager();
		// 全角フォント読み込み
		instance->loadFullByteFont("/MONA-12.MF2");
	}
	return instance;
}

/** 半角フォントロード */
void FontManager::loadHalfByteFont(char *path)
{
	int i;

	if ((halfFp = getByteArray(path)) != NULL) {
		// フォント名取得
		for (i = 0; i < 8; i++) {
			halfName[i] = halfFp[i + 6];
		}
		halfName[i] = 0;
		// 幅と高さ
		halfWidth  = halfFp[14];
		halfHeight = halfFp[15];
		//printf("open file %s [name = %s, w = %d, h = %d]\n", 
		//	path, halfName, halfWidth, halfHeight);
		return;
	} else {
		printf("[ERROR] cant open file %s\n", path);
		return;
	}
}

/** 全角フォントロード */
void FontManager::loadFullByteFont(char *path)
{
	int i, fw, fh, pos = 0;
	MessageInfo info;

	// GUIサーバーを探す
	dword guisvrID = MonAPI::Message::lookupMainThread("BAYGUI.EX2");
	if (guisvrID == 0xFFFFFFFF) {
		//printf("FontManager: GuiServer not found %d\n", guisvrID);
		fullFp = NULL;
		return;
	} else {
		//printf("FontManager: GuiServer found %d\n", guisvrID);
	}
	
	// 共有メモリーからフォントをロード
	if (guisvrID == threadID) {
		// フォント初期化
		fullFpMemory = monapi_call_file_decompress_bz2_file("MONA-12.MF2", false);
		// サーバーは直接ロード
		fullFp = fullFpMemory->Data;
	} else {
		// フォント取得メッセージを投げる
		MonAPI::Message::sendReceive(&info, guisvrID, MSG_GUISERVER_GETFONT, 0, 0, 0, NULL);
		//printf("WindowManager->FontManager: MSG_GUISERVER_GETFONT received %d, %d\n", info.arg2, info.arg3);
		byte* font_data = MonAPI::MemoryMap::map(info.arg2);
		if (font_data == NULL) {
			//printf("FontManager: cannot get font data\n");
			fullFp = NULL;
			return;
		} else {
			fullFp = (unsigned char *)malloc(info.arg3);
			memcpy(fullFp, font_data, info.arg3);
			MonAPI::MemoryMap::unmap(info.arg2);
		}
	}
	
	if (fullFp != NULL) {
	//if ((fullFp = getByteArray(path)) != NULL) {
		copyString(fullName, "MONA-12");
		// 幅と高さ
		fullWidth  = 6;
		fullHeight = 12;
		fullBlockNum = inGetUInt16(&fullFp[0]);
		// 文字までのオフセット(byte)
		fullOffsetList = (int *)malloc(65536);
		memset(fullOffsetList, 4, 65536);
		pos += 4;
		// debug
		//printf("open file %s [name = %s, w = %d, h = %d]\n", 
		//	path, fullName, fullWidth, fullHeight);
		// オフセットリストを作る
		for (i = 0; i < fullBlockNum; i++) {
			fullOffsetList[inGetUInt16(&fullFp[pos])] = pos;
			pos += 4;
			fw = fullFp[pos++];
			fh = fullFp[pos++];
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
 @param [in] str 文字列 (UTF-8 / SJIS)
 @param [out] length 文字数
 @return フォント列
*/
Font **FontManager::decodeString(char *str, int *length)
{
	// UTF-8
	if (codepage == CODE_PAGE_UTF8) {
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
				c2 = (unsigned char)str[++i];
				n = ((c1 & 0x1F) >> 6) | (c2 & 0x3F);
			// 1110 aaaa 10bb bbcc 10cc dddd -> aaaa bbbb cccc dddd (0xE0A080-0xEFBFBF)
			} else if (0xE0 <= c1 && c1 <= 0xEF) {
				// 2 バイト目
				c2 = (unsigned char)str[++i];
				// 3 バイト目
				c3 = (unsigned char)str[++i];
				n = ((c1 & 0xF) << 12) | ((c2 & 0x3F) << 6) | (c3 & 0x3F);
			} else {
				n = 0;
			}
			// デコード開始
			if (n != 0 && fullFp != NULL) {
				for (j = 0; j < fullBlockNum; j++) {
					//if (inGetUInt16(&fullFp[fullOffsetList[j]]) == n) {
						int fw = fullFp[fullOffsetList[n] + 4];
						int fh = fullFp[fullOffsetList[n] + 5];
						fontList[len]->setName(fullName);
						fontList[len]->setWidth(fw);
						fontList[len]->setHeight(fh);
						fontList[len]->setType(FULL_BYTE_FONT);
						memcpy(fontList[len]->getData(), &fullFp[fullOffsetList[n] + 6],
							(int)((fw * fh + 7) / 8));
						break;
					//}
				}
				len++;
			}
		}
		
		*length = len;
		return fontList;
	} else {
		*length = 0;
		return NULL;
	}
}

/**
 コードページを設定する
 @param [in] codepage コードページ (CODE_PAGE_UTF8 / CODE_PAGE_SJIS)
 */
void FontManager::setCodePage(int codepage)
{
	this->codepage = codepage;
}

/**
 コードページを得る
 @return コードページ (CODE_PAGE_UTF8 / CODE_PAGE_SJIS)
 */
int FontManager::getCodePage()
{
	return codepage;
}

/**
 文字列の幅を得る
 @param str 文字列 (UTF-8 / SJIS)
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
 @return 半角フォント、全角フォントのうち高いほうの高さ
*/
int FontManager::getHeight()
{
	if (halfHeight > fullHeight) {
		return halfHeight;
	} else {
		return fullHeight;
	}
}
