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
 @param text ラベル
 */
Label::Label(char *text)
{
	this->align = ALIGN_LEFT;
	this->text = text;
}

/**
 コンストラクタ
 @param text ラベル
 @param align 描画位置 (ALIGN_LEFT / ALIGN_CENTER / ALIGN_RIGHT)
 */
Label::Label(char *text, int align)
{
	this->align = align;
	this->text = text;
}

/** デストラクタ */
Label::~Label()
{
}

/**
 テキスト設定
 @param text
 */
void Label::setText(char *text)
{
	this->text = text;
	repaint();
}

/** 再描画 */
void Label::onPaint(Graphics *g)
{
	int w = this->width, h = this->height;
	
	// 塗りつぶし
	g->setColor(this->backColor);
	g->fillRect(0, 0, w, h);

	// 文字
	int fw = this->_metrics->getWidth(getText());
	int fh = this->_metrics->getHeight(getText());
	if (enabled == true) {
		g->setColor(this->foreColor);
	} else {
		g->setColor(COLOR_GRAY);
	}
	if (this->align == ALIGN_RIGHT) {
		g->drawText(getText(), (w - fw), (h - fh) / 2);
	} else if (this->align == ALIGN_CENTER) {
		g->drawText(getText(), (w - fw) / 2, (h - fh) / 2);
	} else {
		g->drawText(getText(), 0, (h - fh) / 2);
	}
}
