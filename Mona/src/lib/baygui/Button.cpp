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

/**
 コンストラクタ
 @param label ラベル
 */
Button::Button(char *label)
{
	pushed = false;
	copyString(this->label, label);
}

/** デストラクタ */
Button::~Button()
{
}

/**
 ラベルを設定する
 @param label ラベル
 */
void Button::setLabel(char *label)
{
	copyString(this->label, label);
	if (firstpaint == true) {
		repaint();
	}
}

/** ラベル取得 */
char *Button::getLabel()
{
	return label;
}

/** 再描画 */
void Button::repaint()
{
	if (firstpaint == false)
		firstpaint = true;

	// 領域
	_g->setColor(backColor);
	_g->fillRect(1, 1, width - 2, height - 2);
	
	// 枠線
	_g->setColor(foreColor);
	_g->drawLine(2, 0, width - 2, 0);
	_g->drawLine(2, height, width - 2, height);
	_g->drawLine(0, 2, 0, height - 2);
	_g->drawLine(width, 2, width, height - 2);
	_g->drawLine(1, 1, 1, 1);
	_g->drawLine(1, height - 1 , 1, height - 1);
	_g->drawLine(width - 1 , 1, width - 1, 1);
	_g->drawLine(width - 1 , height - 1, width - 1, height - 1);
	if (pushed == true) {
		_g->setColor(~foreColor);
		_g->drawLine(2, height - 1, width - 2, height - 1);
		_g->drawLine(width - 1, 2, width - 1, height - 2);
		_g->drawLine(width - 2 , height - 2, width - 2, height - 2);
		_g->setColor(foreColor);
		_g->drawLine(1, 2, 1, height - 2);
		_g->drawLine(2, 1, width - 2, 1);
	} else {
		_g->setColor(128,128,128);
		_g->drawLine(2, height - 1, width - 2, height - 1);
		_g->drawLine(width - 1, 2, width - 1, height - 2);
		_g->drawLine(width - 2 , height - 2, width - 2, height - 2);
		_g->setColor(~foreColor);
		_g->drawLine(1, 2, 1, height - 2);
		_g->drawLine(2, 1, width - 2, 1);
	}
	
	// 文字
	if (enabled == true) {
		_g->setColor(foreColor);
	} else {
		_g->setColor(128,128,128);
	}
	int fw = FontManager::getInstance()->getWidth(label);
	int fh = FontManager::getInstance()->getHeight();
	if (pushed == true) {
		_g->drawText(label, (width - fw) / 2 + 1, (height - fh) / 2 + 1);
	} else {
		_g->drawText(label, (width - fw) / 2, (height - fh) / 2);
	}
}

/** イベント処理 */
void Button::postEvent(Event *event) {
	// 非活性の時はイベントを受け付けない
	if (enabled == false) return;

	if (event->type == MOUSE_PRESSED) {
		pushed = true;
		if (firstpaint == true) {
			repaint();
			Control::postEvent(event);
		}
	} else if (event->type == MOUSE_RELEASED) {
		pushed = false;
		if (firstpaint == true) {
			repaint();
			Control::postEvent(event);
		}
	}
}
