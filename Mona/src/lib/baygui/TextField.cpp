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
TextField::TextField()
{
	textPtr = -1;
	textLen = 0;
	offx = 3;
	offy = 3;
	memset(text, 0, MAX_TEXT_LEN);
	imemode = false;
	_textEvent = new Event(TEXT_CHANGED, this);
	imeManager = new ImeManager();
	imeManager->setParent(this);
}

/** デストラクタ */
TextField::~TextField()
{
	delete(_textEvent);
}

/**
 テキスト設定
 @param text
 */
void TextField::setText(char *text)
{
	copyString(this->text, text);
	textLen = strlen(text);
	textPtr = textLen - 1;
	if (firstpaint == true) {
		repaint();
	}
}

/** テキスト取得 */
char *TextField::getText()
{
	return text;
}

/** 再描画 */
void TextField::repaint()
{
	if (firstpaint == false)
		firstpaint = true;

	// 枠線
	if (focused == true && enabled == true) {
		_g->setColor(0,128,255);
		_g->drawRect(0, 0, width, height);
	} else {
		_g->setColor(getParent()->getBackground());
		_g->drawRect(0, 0, width, height);
	}
	_g->setColor(foreColor);
	_g->drawRect(1, 1, width - 2, height - 2);
	_g->setColor(~foreColor);
	_g->fillRect(2, 2, width - 3, height - 3);

	// 文字
	int fh = FontManager::getInstance()->getHeight();
	_g->setFont(this->font);
	if (enabled == true) {
		_g->setColor(foreColor);
	} else {
		_g->setColor(128,128,128);
	}
	_g->drawText(text, offx, (height - fh) / 2);

	// キャレット
	if (focused == true && enabled == true) {
		int fw = getCaretWidth();
		if (imemode == false) {
			_g->drawLine(offx + fw, offy, offx + fw, offy + 14);
		} else {
			imeManager->repaint();
		}
	}
}

/** 1文字挿入 */
void TextField::insertCharacter(char c)
{
	int i;

	for (i = textLen; i >= textPtr + 1; i--) {
		text[i] = text[i - 1];
	}
	text[textLen + 1] = '\0';
	text[++textPtr] = c;
	textLen++;
}

/** 1文字削除 */
void TextField::deleteCharacter()
{
	int i;
	for (i = textPtr; i < textLen - 1; i++) {
		text[i] = text[i + 1];
	}
	text[i] = '\0';
	textPtr--;
	textLen--;
}

int TextField::getCaretWidth()
{
	int i;
	char temp[MAX_TEXT_LEN];
	for (i = 0; i <= textPtr; i++) {
		temp[i] = text[i];
	}
	temp[i] = '\0';
	return FontManager::getInstance()->getWidth(temp);
}

/** イベント処理 */
void TextField::postEvent(Event *event)
{
	// 非活性の時はイベントを受け付けない
	if (enabled == false) return;
	
	// IME確定
	if (event->type == IME_ENDCOMPOSITION) {
		// 確定済みバッファーを得る
		char *buffer = imeManager->getBuffer();
		if (strlen(buffer) > 0) {
			for (int i = 0; i < (int)strlen(buffer); i++) {
				insertCharacter(buffer[i]);
			}
			imeManager->clearBuffer();
			repaint();
		}
	// キー押下
	} else if (event->type == KEY_PRESSED) {
		int keycode = ((KeyEvent *)event)->keycode;
		int modifiers = ((KeyEvent *)event)->modifiers;
		if (imemode == true) {
			if (modifiers == VKEY_CTRL && keycode == '\\') {
				// IMEオフ
				imemode = false;
			} else {
				// IMEマネージャに丸投げ
				imeManager->postEvent(event);
			}
		} else {
			if (modifiers == VKEY_CTRL && keycode == '\\') {
				// IMEオン
				imemode = true;
				imeManager->setRect(x + offx + getCaretWidth(), y + offy, width, height);
			} else if (keycode == VKEY_BACKSPACE) {
				// バックスペース
				if (textPtr >= 0) {
					// 2バイト文字
					if (textPtr >= 1 && 0xC2 <= (0xFF & text[textPtr - 1]) && (0xFF & text[textPtr - 2]) <= 0xDF) {
						deleteCharacter();
						deleteCharacter();
					// 3バイト文字
					} else if (textPtr >= 2 && 0xE0 <= (0xFF & text[textPtr - 2]) && (0xFF & text[textPtr - 2]) <= 0xEF) {
						deleteCharacter();
						deleteCharacter();
						deleteCharacter();
					// 1バイト文字
					} else {
						deleteCharacter();
					}
				}
			} else if (keycode == VKEY_LEFT) {
				// ←移動
				if (textPtr >= 0) {
					textPtr--;
				}
			} else if (keycode == VKEY_RIGHT) {
				// →移動
				if (textPtr < textLen - 1) {
					textPtr++;
				}
			} else if (keycode == VKEY_ENTER) {
				// 確定
				Control::postEvent(_textEvent);
			} else if (keycode < 128) {
				// 1文字挿入
				insertCharacter(keycode);
			}
			if (firstpaint == true) repaint();
			Control::postEvent(event);
		}
	// フォーカス状態変更
	} else if (event->type == FOCUS_IN || event->type == FOCUS_OUT) {
		if (firstpaint == true) {
			repaint();
			Control::postEvent(_focusEvent);
		}
	}
}
