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
#include "te_roma0.h"

/** コンストラクタ */
ImeManager::ImeManager()
{
	clearBuffer(inputBuffer);
	clearBuffer(translateBuffer);
	clearBuffer(decideBuffer);
	imemode = false;
	_imeEvent = new ImeEvent(IME_SETCONTEXT, this);
}

/** デストラクタ */
ImeManager::~ImeManager()
{
	delete(_imeEvent);
}

/** バッファーをクリアする */
void ImeManager::clearBuffer(char *buffer)
{
	memset(buffer, 0, MAX_TEXT_LEN);
}

/** バッファーに文字を追加する */
void ImeManager::insertCharacter(char *buffer, char c)
{
	buffer[strlen(buffer)] = c;
}

/** バッファーに文字列を追加する */
void ImeManager::insertString(char *buffer, char *str)
{
	int len = strlen(buffer);
	for(int i = len; i < len + (int)strlen(str); i++) {
		buffer[i] = str[i - len];
	}
}

/**
 バッファーの文字を1文字削除する
 @return 削除した文字のバイト数(1-3)
 */
int ImeManager::deleteCharacter(char *buffer)
{
	int len = strlen(buffer);

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

/**
 入力文字→かな変換
 @return マッチしたら「かな」、マッチしなかったら「NULL」
 */
char *ImeManager::getKana()
{
	// デフォルトは
	for (int i = 0; i < HENKAN0LENGTH; i++) {
		if (strcmp(henkan00[i], inputBuffer) == 0) {
			clearBuffer(inputBuffer);
			return henkan01[i];
		}
	}
	return NULL;
}

/** 親部品登録 */
void ImeManager::setParent(Control *parent) {
	this->parent = parent;
}

/** 確定済みバッファーを得る */
char *ImeManager::getText()
{
	return decideBuffer;
}

/** 確定済みバッファーを設定する */
void ImeManager::setText(char *text)
{
	copyString(decideBuffer, text);
}

/** 確定済みバッファーを得る */
void ImeManager::clearBuffer()
{
	clearBuffer(inputBuffer);
	clearBuffer(translateBuffer);
	clearBuffer(decideBuffer);
}

/** 再描画 */
void ImeManager::repaint()
{
	int fw1 = 0, fw2 = 0, fw3 = 0, fh = 0;
	
	// 塗りつぶし
	_g->setColor(~foreColor);
	_g->fillRect(0, 0, width, height);
	fh  = FontManager::getInstance()->getHeight();

	// 確定文字列
	if (strlen(decideBuffer) > 0) {
		fw1 = FontManager::getInstance()->getWidth(decideBuffer);
		_g->setColor(foreColor);
		_g->drawText(decideBuffer, 0, (height - fh) / 2);
	}
	
	// 変換対象文字列
	if (strlen(translateBuffer) > 0) {
		fw2 = FontManager::getInstance()->getWidth(translateBuffer);
		fw3 = FontManager::getInstance()->getWidth(inputBuffer);
		_g->setColor(0, 0, 255);
		_g->drawText(translateBuffer, fw1, (height - fh) / 2);
		_g->drawText(inputBuffer, fw1 + fw2, (height - fh) / 2);
		_g->drawLine(fw1, 13, fw1 + fw2 + fw3, 13);
	}
	
	// キャレット
	if (focused == true && enabled == true) {
		_g->setColor(0, 0, 0);
		_g->drawLine(fw1 + fw2 + fw3, 0, fw1 + fw2 + fw3, 13);
	}
}

/** イベント処理 */
void ImeManager::postEvent(Event *event)
{
	// キー押下
	if (event->type == KEY_PRESSED) {
		int keycode = ((KeyEvent *)event)->keycode;
		int modifiers = ((KeyEvent *)event)->modifiers;
		
		// IMEオン・オフ切り替え
		if (modifiers == VKEY_CTRL && keycode == '\\') {
			clearBuffer(inputBuffer);
			clearBuffer(translateBuffer);
			if (imemode == true) {
				imemode = false;
			} else {
				imemode = true;
			}
		// バックスペース
		} else if (keycode == VKEY_BACKSPACE) {
			int len = 0;
			
			// 入力文字列を削除
			if (strlen(inputBuffer) > 0) {
				len = deleteCharacter(inputBuffer);
			// 変換対象文字列を削除
			} else if (strlen(translateBuffer) > 0) {
				len = deleteCharacter(translateBuffer);
			// 確定済み文字列を削除
			} else if (strlen(decideBuffer) > 0) {
				len = deleteCharacter(decideBuffer);
			}
			
			// バックスペース送信
			for (int i = 0; i < len; i++) {
				_imeEvent->type = IME_CHAR;
				_imeEvent->charcode = VKEY_BACKSPACE;
				parent->postEvent(_imeEvent);
			}
		// 変換
		} else if (keycode == ' ' && strlen(translateBuffer) > 0) {
			clearBuffer(translateBuffer);
			insertString(translateBuffer, "オマエモナ");
		// 確定
		} else if (keycode == VKEY_ENTER) {
			if (strlen(translateBuffer) == 0) {
				// 確定イベント送信
				_imeEvent->type = IME_NOTIFY;
				_imeEvent->message = TEXT_CHANGED;
		syscall_print("0,");
				parent->postEvent(_imeEvent);
		syscall_print("9,");
			} else {
				insertString(decideBuffer, translateBuffer);
				clearBuffer(inputBuffer);
				
				// 確定イベント送信
				_imeEvent->type = IME_ENDCOMPOSITION;
				parent->postEvent(_imeEvent);
				
				// １文字イベント送信
				for (int i = 0; i < (int)strlen(translateBuffer); i++) {
					_imeEvent->type = IME_CHAR;
					_imeEvent->charcode = translateBuffer[i];
					parent->postEvent(_imeEvent);
				}
				clearBuffer(translateBuffer);
			}
		// 一般文字
		} else if (keycode < 128) {
			if (imemode == true) {
				insertCharacter(inputBuffer, keycode);
				// まちがっていっぱい入力してしまったとき（xjrakjra等）
				if (strlen(inputBuffer) > 4) {
					clearBuffer(inputBuffer);
				// 入力文字列を変換して変換文字列バッファーに移す
				} else {
					char *kana = getKana();
					if (kana != NULL) {
						insertString(translateBuffer, kana);
					}
				}
			// 直接入力
			} else {
				insertCharacter(decideBuffer, keycode);
				_imeEvent->type = IME_CHAR;
				_imeEvent->charcode = keycode;
				parent->postEvent(_imeEvent);
			}
		}
		repaint();
	}
}
