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
	imeEvent = new Event(IME_SETCONTEXT, this);
}

/** デストラクタ */
ImeManager::~ImeManager()
{
	delete(imeEvent);
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

/** バッファーの文字を1文字削除する */
void ImeManager::deleteCharacter(char *buffer)
{
	buffer[strlen(buffer) - 1] = 0;
}

/**
 hanken0[i]と入力文字列の比較
 @return マッチしたらhenkan1の添え字番号、マッチしなかったら-1
 */
int ImeManager::checkHenkanX()
{
	for (int i = 0; i < HENKAN0LENGTH; i++) {
		if (strcmp(henkan0[i], inputBuffer) == 0) {
			clearBuffer(inputBuffer);
			return i;
		}
	}
	return -1;
}

/** 親部品登録 */
void ImeManager::setParent(Control *parent) {
	this->parent = parent;
}

/** 確定済みバッファーを得る */
char *ImeManager::getBuffer()
{
	return decideBuffer;
}

/** 確定済みバッファーを得る */
void ImeManager::clearBuffer()
{
	int fw = FontManager::getInstance()->getWidth(decideBuffer);
	setRect(x + fw, y, width, height);
	clearBuffer(decideBuffer);
}

/** 再描画 */
void ImeManager::repaint()
{
	int fw1 = 0, fw2 = 0;
	
	// 確定文字列
	if (strlen(decideBuffer) > 0) {
		fw1 = FontManager::getInstance()->getWidth(decideBuffer);
		_g->setColor(255, 255, 255);
		_g->fillRect(0, 0, fw1 + 13, 14);
		_g->setColor(0, 0, 0);
		_g->drawText(decideBuffer, 0, 0);
	}
	
	// 変換対象文字列
	if (strlen(translateBuffer) > 0) {
		fw2 = FontManager::getInstance()->getWidth(translateBuffer);
		_g->setColor(255, 255, 255);
		_g->fillRect(fw1, 0, fw2 + 13, 14);
		_g->setColor(0, 0, 255);
		_g->drawText(translateBuffer, fw1, 0);
		_g->drawLine(fw1, 13, fw1 + fw2, 13);
	}
	
	// 1文字残る場合があるのでそれを消す
	if (strlen(decideBuffer) == 0 && strlen(translateBuffer) == 0) {
		_g->setColor(255, 255, 255);
		_g->fillRect(0, 0, 13, 14);
	}
	
	// キャレット
	_g->drawLine(fw1 + fw2, 0, fw1 + fw2, 13);
}

/** イベント処理 */
void ImeManager::postEvent(Event *event)
{
	// キー押下
	if (event->type == KEY_PRESSED) {
		int keycode = ((KeyEvent *)event)->keycode;
		//int modifiers = ((KeyEvent *)event)->modifiers;
		// バックスペース
		if (keycode == VKEY_BACKSPACE) {
			// 変換対象文字列を削除
			if (strlen(translateBuffer) > 0) {
				deleteCharacter(translateBuffer);
				deleteCharacter(translateBuffer);
				deleteCharacter(translateBuffer);
			// 確定済み文字列を削除
			} else if (strlen(decideBuffer) > 0) {
				deleteCharacter(decideBuffer);
				deleteCharacter(decideBuffer);
				deleteCharacter(decideBuffer);
			}
		// 確定
		} else if (keycode == VKEY_ENTER && strlen(translateBuffer) > 0) {
			insertString(decideBuffer, translateBuffer);
			clearBuffer(translateBuffer);
			imeEvent->type = IME_ENDCOMPOSITION;
			parent->postEvent(imeEvent);
			return;
		// 一般文字
		} else if (keycode < 128) {
			insertCharacter(inputBuffer, keycode);
			// まちがっていっぱい入力してしまったとき（xjrakjra等）
			if (strlen(inputBuffer) > 4) {
				clearBuffer(inputBuffer);
			// 入力文字列をバッファーの内容を変換文字列バッファーに移す
			} else {
				int ret = checkHenkanX();
				if (ret >= 0) {
					insertString(translateBuffer, henkan1[ret]);
				}
			}
		}
		repaint();
	}
}
