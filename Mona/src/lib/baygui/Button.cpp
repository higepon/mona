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
	this->pushed = false;
	this->label  = label;
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
	this->label = label;
	repaint();
}

/** 再描画 */
void Button::onPaint(Graphics *g)
{
	int w = this->width, h = this->height;
	
	// 一度背景色でクリア
	g->setColor(this->backColor);
	g->fillRect(0, 0, w, h);
	
	// 枠を描画
	g->setColor(COLOR_BLACK);
	g->drawLine(2, 0, w - 3, 0);
	g->drawLine(2, h - 1, w - 3, h - 1);
	g->drawLine(0, 2, 0, h - 3);
	g->drawLine(w - 1, 2, w - 1, h - 3);
	g->drawLine(1, 1, 1, 1);
	g->drawLine(1, h - 2 , 1, h - 2);
	g->drawLine(w - 2 , 1, w - 2, 1);
	g->drawLine(w - 2 , h - 2, w - 2, h - 2);
	
	if (this->pushed) {
		g->setColor(COLOR_WHITE);
		g->drawLine(2, h - 2, w - 3, h - 2);
		g->drawLine(w - 2, 2, w - 2, h - 3);
		g->drawLine(w - 3 , h - 3, w - 3, h - 3);
		g->setColor(COLOR_GRAY);
		g->drawLine(1, 2, 1, h - 3);
		g->drawLine(2, 1, w - 3, 1);
	} else {
		g->setColor(COLOR_GRAY);
		g->drawLine(2, h - 2, w - 3, h - 2);
		g->drawLine(w - 2, 2, w - 2, h - 3);
		g->drawLine(w - 3 , h - 3, w - 3, h - 3);
		g->setColor(COLOR_WHITE);
		g->drawLine(1, 2, 1, h - 3);
		g->drawLine(2, 1, w - 3, 1);
	}
	
	// 文字
	int fw = this->_metrics->getWidth(getLabel());
	int fh = this->_metrics->getHeight(getLabel());
	int x = (w - fw) / 2;
	int y = (h - fh) / 2;
	if (this->pushed) {
		x++;
		y++;
	}
	if (enabled == true) {
		g->setColor(this->foreColor);
	} else {
		g->setColor(COLOR_GRAY);
	}
	g->drawText(getLabel(), x, y);
}

/** イベント処理 */
void Button::onEvent(Event *event) {
	// 非活性の時はイベントを受け付けない
	if (this->enabled == false) return;

	if (event->type == MOUSE_PRESSED) {
		this->pushed = true;
		repaint();
		getParent()->onEvent(event);
	} else if (event->type == MOUSE_RELEASED) {
		this->pushed = false;
		repaint();
		getParent()->onEvent(event);
	}
}
