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
	memset(text, 0, MAX_TEXT_LEN);
	_textEvent = new Event(TEXT_CHANGED, this);
	_imeManager = new ImeManager();
	_imeManager->setParent(this);
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
	xstrncpy(this->text, text, MAX_TEXT_LEN);
	if (firstpaint == true) {
		repaint();
	}
}

/** テキスト取得 */
char *TextField::getText()
{
	return this->text;
}

/** 再描画 */
void TextField::repaint()
{
	int offx = 3, offy = 3;

	if (firstpaint == false)
		firstpaint = true;

	// 枠線
	if (focused == true && enabled == true) {
		_g->setColor(0,128,255);
		_g->drawRect(0, 0, _width, _height);
	} else {
		_g->setColor(getParent()->getBackground());
		_g->drawRect(0, 0, _width, _height);
	}
	_g->setColor(foreColor);
	_g->drawRect(1, 1, _width - 2, _height - 2);

	// 塗りつぶし
	_g->setColor(~foreColor);
	_g->fillRect(2, 2, _width - 3, _height - 3);

	// 文字
	int fw = FontManager::getInstance()->getWidth(text);
	int fh = FontManager::getInstance()->getHeight();
	_g->setColor(foreColor);
	_g->drawText(text, offx, (_height - fh) / 2);

	// IME再描画
	_imeManager->setRect(_x + offx + fw, _y + offy, _width - offx * 2 - fw, _height - offy * 2);
	_imeManager->setForeground(foreColor);
	_imeManager->setBackground(~foreColor);
	_imeManager->setFocused(focused);
	_imeManager->setFont(this->font);
	_imeManager->repaint();
}

/** イベント処理 */
void TextField::postEvent(Event *event)
{
	// 非活性の時はイベントを受け付けない
	if (enabled == false) return;
	
	// １文字イベント
	if ((event->type & 0xFFFF) == IME_CHAR) {
		int keycode = (event->type >> 16) & 0xFFFF;
		// 確定イベント
		if (keycode == VKEY_ENTER) {
			Control::postEvent(_textEvent);
		// １文字削除
		} else if (keycode == VKEY_BACKSPACE) {
			//text[strlen(text) - 1] = 0;
			_imeManager->deleteCharacter(text);
			repaint();
		// １文字挿入
		} else {
			//text[strlen(text)] = (char)(keycode & 0xFF);
			_imeManager->insertCharacter(text, (char)(keycode & 0xFF));
		}
	// 確定イベント
	} else if (event->type == IME_ENDCOMPOSITION) {
		repaint();
	// キー押下
	} else if (event->type == KEY_PRESSED) {
		// IMEマネージャに丸投げ
		_imeManager->postEvent(event);
		Control::postEvent(event);
	// フォーカス状態変更
	} else if (event->type == FOCUS_IN || event->type == FOCUS_OUT) {
		if (firstpaint == true) {
			repaint();
			Control::postEvent(_focusEvent);
		}
	}
}
