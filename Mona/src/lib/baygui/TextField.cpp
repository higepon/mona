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
	offx = 3;
	offy = 3;
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
	_imeManager->setText(text);
	if (firstpaint == true) {
		repaint();
	}
}

/** テキスト取得 */
char *TextField::getText()
{
	return _imeManager->getText();
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

	// IME入力部再描画
	_imeManager->setRect(x + offx, y + offy, width - offx * 2, height - offy * 2);
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
	
	// IME通知イベント
	if (event->type == IME_NOTIFY) {
		int message = ((ImeEvent *)event)->message;
		// 確定イベント
		if (message == TEXT_CHANGED) {
			Control::postEvent(_textEvent);
		}
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
