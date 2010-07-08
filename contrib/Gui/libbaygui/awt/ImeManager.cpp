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
#include <monapi/CString.h>
#include <monapi.h>
#include "baygui/awt/ImeManager.h"
#include <servers/ime.h>

#define IMESERVER_NAME   "IME.EX5"

static void xstrncpy(char *dst, char *src, int len)
{
    memset(dst, 0, len);
    strncpy(dst, src, len - 1);
    dst[len - 1] = 0;
}

/** コンストラクタ */
ImeManager::ImeManager()
{
	clearBuffer(inputBuffer);
	clearBuffer(translateBuffer);
	imemode = false;
	_imeEvent = new Event(Event::Event::IME_SETCONTEXT, this);
	imesvrID = MonAPI::Message::lookupMainThread(IMESERVER_NAME);
	kanjiListPtr = -1;
}

/** デストラクタ */
ImeManager::~ImeManager()
{
	delete(_imeEvent);
}

/** 指定したバッファーをクリアする */
void ImeManager::clearBuffer(char *buffer)
{
	memset(buffer, 0, MAX_TEXT_LEN);
}

/** 指定したバッファーに文字を追加する */
void ImeManager::insertCharacter(char *buffer, char c)
{
	if (strlen(buffer) >= MAX_TEXT_LEN - 1) return;
	buffer[strlen(buffer)] = c;
}

/** 指定したバッファーに文字列を追加する */
void ImeManager::insertString(char *buffer, const char *str)
{
	int len = strlen(buffer);
	for(int i = len; i < len + (int)strlen(str); i++) {
		if (i >= MAX_TEXT_LEN - 1) return;
		buffer[i] = str[i - len];
	}
}

/**
 指定したバッファーの文字を1文字削除する
 @return 削除した文字のバイト数(0-3)
 */
int ImeManager::deleteCharacter(char *buffer)
{
	int len = strlen(buffer);

	// NULLチェック
	if (len <= 0) return 0;
	
	// 3バイト文字
	if (len >= 3 && 0xE0 <= (0xFF & buffer[len - 3]) && (0xFF & buffer[len - 3]) <= 0xEF) {
		buffer[strlen(buffer) - 1] = 0;
		buffer[strlen(buffer) - 1] = 0;
		buffer[strlen(buffer) - 1] = 0;
		return 3;
	// 2バイト文字
	} else if (len >= 2 && 0xC2 <= (0xFF & buffer[len - 2]) && (0xFF & buffer[len - 2]) <= 0xDF) {
		buffer[strlen(buffer) - 1] = 0;
		buffer[strlen(buffer) - 1] = 0;
		return 2;
	// 1バイト文字
	} else {
		buffer[strlen(buffer) - 1] = 0;
		return 1;
	}
}

/** かな→漢字変換 */
bool ImeManager::getKanji(char *str, HList<MonAPI::CString> *result)
{
	// NULLチェック
	if (imesvrID == THREAD_UNKNOWN) return false;

	MessageInfo info;
	//printf("ImeManager: connected %s\n", IMESERVER_NAME);
	MonAPI::Message::sendReceive(&info, imesvrID, MSG_IMESERVER_GETKANJI, 0, 0, 0, str);
	if (info.arg2 > 0) {
		bool flag = true;
		while (flag) {
			if (!MonAPI::Message::receive(&info)) {
				switch(info.header) {
				case MSG_IMESERVER_STARTKANJI:
					//printf("MSG_IMESERVER_STARTKANJI\n");
					MonAPI::Message::reply(&info);
					break;
				case MSG_IMESERVER_KANJI:
					//printf("%d: %s\n", info.arg2, info.str);
					MonAPI::Message::reply(&info);
					result->add(info.str);
					break;
				case MSG_IMESERVER_ENDKANJI:
					//printf("MSG_IMESERVER_ENDKANJI\n");
					MonAPI::Message::reply(&info);
					flag = false;
					break;
				}
			}
		}
		return true;
	} else {
		return false;
	}
}

/**
 入力文字→かな変換
 @param [in] str 入力文字
 @param [out] result かな
 @return マッチしたらtrue、マッチしなかったらfalse
 */
bool ImeManager::getKana(char *str, char *result)
{
	// NULLチェック
	if (imesvrID == THREAD_UNKNOWN) return false;
	
	MessageInfo info;
	//printf("ImeManager: connected %s\n", IMESERVER_NAME);
	MonAPI::Message::sendReceive(&info, imesvrID, MSG_IMESERVER_GETKANA, 0, 0, 0, str);
	if (info.arg2 > 0) {
		//printf("MSG_IMESERVER_GETKANA: %s -> %s\n", str, info.str);
		xstrncpy(result, info.str, MAX_TEXT_LEN);
		return true;
	} else {
		return false;
	}
}

/** 親部品登録 */
void ImeManager::setParent(Component *parent) {
	this->parent = parent;
}

/** 漢字リストをクリアする */
void ImeManager::clearKanjiList()
{
	// 漢字リスト初期化
	kanjiListPtr = -1;
	int I = kanjiList.size();
	for (int i = 0; i < I; i++) {
		kanjiList.removeAt(0);
	}
}

/** すべての内部バッファーをクリアする */
void ImeManager::clearBuffer()
{
	clearBuffer(inputBuffer);
	clearBuffer(translateBuffer);
}

/** 再描画 */
void ImeManager::paint(Graphics* g)
{
	int fw1 = 0, fw2 = 0;
	int fh  = getFontMetrics()->getHeight(" ");
	
	// 塗りつぶし
	g->setColor(~getForeground());
	//g->setColor(255,255,128);
	g->fillRect(0, 0, getWidth() + 1, getHeight() + 1);

	// 確定文字列
	//if (strlen(decideBuffer) > 0) {
	//	fw1 = FontMetrics::getInstance()->getWidth(decideBuffer);
	//	g->setColor(foreColor);
	//	g->drawText(decideBuffer, 0, (getHeight() - fh) / 2);
	//}
	
	// 変換対象文字列
	if (strlen(translateBuffer) > 0) {
		fw1 = getFontMetrics()->getWidth(translateBuffer);
		g->setColor(0, 0, 255);
		g->drawString(translateBuffer, 0, (getHeight() - fh) / 2);
		g->drawLine(0, 13, fw1, 13);
	}
	
	// 入力中文字列
	//if (strlen(inputBuffer) > 0) {
	//	fw2 = FontMetrics::getInstance()->getWidth(inputBuffer);
	//	g->setColor(0, 0, 255);
	//	g->drawText(inputBuffer, fw1, (getHeight() - fh) / 2);
	//	g->drawLine(fw1, 13, fw1 + fw2, 13);
	//}
	
	// キャレット
	if (getFocused() == true && getEnabled() == true) {
		g->setColor(0, 0, 0);
		g->drawLine(fw1 + fw2, 0, fw1 + fw2, 13);
	}
}

/** イベント処理 */
void ImeManager::processEvent(Event *event)
{
	// キー押下
	if (event->getType() == Event::KEY_PRESSED) {
		int keycode = ((KeyEvent *)event)->getKeycode();
		int modifiers = ((KeyEvent *)event)->getModifiers();
		
		// IMEオン・オフ切り替え
		if (imesvrID != THREAD_UNKNOWN && 
			((modifiers == KeyEvent::VKEY_CTRL && keycode == '\\') ||
			(modifiers == KeyEvent::VKEY_LSHIFT && keycode == ' '))) {
			#if 0
			clearBuffer();
			clearKanjiList();
			if (imemode == true) {
				imemode = false;
			} else {
				imemode = true;
			}
			repaint();
			#endif
		// バックスペース
		} else if (keycode == KeyEvent::VKEY_BACKSPACE) {
			int len = 0;
			
			if (0 <= kanjiListPtr && kanjiListPtr < kanjiList.size() - 1) {
				// 変換中にバックスペース押したときは無変換と同等に扱う
				clearBuffer();
				insertString(translateBuffer, (const char *)kanjiList.get(kanjiList.size() - 1));
				clearKanjiList();
				repaint();
			} else {
				// 入力文字列を削除
				if (strlen(inputBuffer) > 0) {
					len = deleteCharacter(inputBuffer);
					repaint();
				// 変換対象文字列を削除
				} else if (strlen(translateBuffer) > 0) {
					len = deleteCharacter(translateBuffer);
					repaint();
				// 親部品の確定文字列を削除
				} else {
					// バックスペース送信
					_imeEvent->setType(Event::IME_CHAR | (KeyEvent::VKEY_BACKSPACE << 16));
					parent->dispatchEvent(_imeEvent);
				}
			}
		// 変換
		} else if (keycode == ' ' && strlen(translateBuffer) > 0) {
			if (kanjiListPtr == -1) {
				// 変換成功
				if (getKanji(translateBuffer, &kanjiList) == true) {
					clearBuffer();
					insertString(translateBuffer, (const char *)kanjiList.get(++kanjiListPtr));
				// 変換失敗
				} else {
					clearBuffer();
					insertString(translateBuffer, "変換失敗");
				}
			} else {
				// 次候補
				if (kanjiListPtr == kanjiList.size() - 1) {
					kanjiListPtr = -1;
				}
				clearBuffer();
				insertString(translateBuffer, (const char *)kanjiList.get(++kanjiListPtr));
			}
			repaint();
		// 確定
		} else if (keycode == KeyEvent::VKEY_ENTER) {
			if (strlen(translateBuffer) == 0) {
				clearBuffer();
				clearKanjiList();
				
				// ENTER送信
				_imeEvent->setType(Event::IME_CHAR | (KeyEvent::VKEY_ENTER << 16));
				parent->dispatchEvent(_imeEvent);
			} else {
				// １文字送信
				for (int i = 0; i < (int)strlen(translateBuffer); i++) {
					_imeEvent->setType(Event::IME_CHAR | (translateBuffer[i] << 16));
					parent->dispatchEvent(_imeEvent);
				}
				
				clearBuffer();
				clearKanjiList();
				
				// 確定イベント送信
				_imeEvent->setType(Event::IME_ENDCOMPOSITION);
				parent->dispatchEvent(_imeEvent);
			}
		// 一般文字
		} else if (keycode < 128) {
			if (imemode == true) {
				insertCharacter(inputBuffer, keycode);
				// まちがっていっぱい入力してしまったとき（xjrakjra等）
				if (strlen(inputBuffer) > 4) {
					clearBuffer(inputBuffer);
					clearKanjiList();
				// 入力文字列を変換して変換文字列バッファーに移す
				} else {
					char kana[MAX_TEXT_LEN];
					if (getKana(inputBuffer, kana) == true) {
						clearBuffer(inputBuffer);
						clearKanjiList();
						insertString(translateBuffer, kana);
					}
				}
				// 再描画
				repaint();
			// 直接入力
			} else {
				// １文字送信
				_imeEvent->setType(Event::IME_CHAR | (keycode << 16));
				parent->dispatchEvent(_imeEvent);
				
				// 確定イベント送信
				_imeEvent->setType(Event::IME_ENDCOMPOSITION);
				parent->dispatchEvent(_imeEvent);
			}
		}
	}
}
