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
	_textEvent = new Event(TEXT_CHANGED, this);
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
		_g->setColor(200,200,200);
		_g->drawRect(0, 0, width, height);
	}
	_g->setColor(0,0,0);
	_g->drawRect(1, 1, width - 2, height - 2);
	_g->setColor(255,255,255);
	_g->fillRect(2, 2, width - 3, height - 3);

	// 文字
	int fh = FontManager::getInstance()->getHeight();
	if (enabled == true) {
		_g->setColor(0,0,0);
	} else {
		_g->setColor(128,128,128);
	}
	_g->drawText(text, offx, (height - fh) / 2);

	// キャレット
	if (focused == true && enabled == true) {
		int i;
		char temp[MAX_TEXT_LEN];
		for (i = 0; i <= textPtr; i++) {
			temp[i] = text[i];
		}
		temp[i] = '\0';
		int fw = FontManager::getInstance()->getWidth(temp);
		_g->drawLine(offx + fw, offy, offx + fw, offy + 14);
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

/** イベント処理 */
void TextField::postEvent(Event *event)
{
	// 非活性の時はイベントを受け付けない
	if (enabled == false) return;
	
	// キー押下
	if (event->type == KEY_PRESSED) {
		int keycode = ((KeyEvent *)event)->keycode;
		if (keycode == VKEY_BACKSPACE) {
			if (textPtr >= 0) {
				// バックスペース
				deleteCharacter();
				if (firstpaint == true) {
					repaint();
				}
			}
		} else if (keycode == VKEY_LEFT) {
			// ←移動
			if (textPtr >= 0) {
				textPtr--;
				if (firstpaint == true) {
					repaint();
				}
			}
		} else if (keycode == VKEY_RIGHT) {
			// →移動
			if (textPtr < textLen - 1) {
				textPtr++;
				if (firstpaint == true) {
					repaint();
				}
			}
		} else if (keycode == VKEY_ENTER) {
			// 確定
			Control::postEvent(_textEvent);
			return;
		} else if (keycode < 128) {
			// 1文字挿入
			insertCharacter(keycode);
			if (firstpaint == true) {
				repaint();
			}
		}
		Control::postEvent(event);
	// フォーカス状態変更
	} else if (event->type == FOCUS_IN || event->type == FOCUS_OUT) {
		if (firstpaint == true) {
			repaint();
			Control::postEvent(_focusEvent);
		}
	}
}
