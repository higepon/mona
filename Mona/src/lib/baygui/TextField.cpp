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

TextField::TextField()
{
	this->textPtr = -1;
	this->textLen = 0;
	this->offx = 3;
	this->offy = 3;
	memset(text, 0, MAX_TEXT_LEN);
	this->textEvent.setType(Event::TEXT_CHANGED);
	this->textEvent.setSource(this);
	setBackground(Color::WHITE);
}

TextField::~TextField()
{
}


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

void TextField::setText(char *text)
{
	strcpy(this->text, text);
	this->textLen = strlen(text);
	this->textPtr = this->textLen - 1;
	repaint();
}

void TextField::onPaint(Graphics *g)
{
	int w = getWidth(), h = getHeight();

	// 外枠
	if (getFocused() == true && getEnabled() == true) {
		g->setColor(0, 128, 255);
		g->drawRect(0, 0, w, h);
	} else {
		g->setColor(getParent()->getBackground());
		g->drawRect(0, 0, w, h);
	}
	
	// 内枠
	g->setColor(getBackground());
	g->fillRect(1, 1, w - 2, h - 2);
	g->setColor(getForeground());
	g->drawRect(1, 1, w - 2, h - 2);

	// 文字
	int fh = getFontMetrics()->getHeight(getText());
	if (getEnabled() == true) {
		g->setColor(getForeground());
	} else {
		g->setColor(Color::GRAY);
	}
	g->drawText(text, this->offx, (h - fh) / 2);

	// キャレット
	if (getFocused() == true && getEnabled() == true) {
		int i;
		char temp[MAX_TEXT_LEN];
		for (i = 0; i <= textPtr; i++) {
			temp[i] = text[i];
		}
		temp[i] = '\0';
		int fw = getFontMetrics()->getWidth(temp);
		g->drawLine(offx + fw, offy, offx + fw, offy + 12);
	}
	//g->drawLine(offx + textPtr * 8 + 8, offy, offx + textPtr * 8 + 8, offy + 12);
}

void TextField::onEvent(Event *event)
{
	// 非活性の時はイベントを受け付けない
	if (getEnabled() == false) return;
	
	// キー押下
	if (event->getType() == KeyEvent::KEY_PRESSED) {
		int keycode = ((KeyEvent *)event)->getKeycode();
		if (keycode == KeyEvent::VKEY_BACKSPACE) {
			if (textPtr >= 0) {
				// バックスペース
				deleteCharacter();
				repaint();
			}
		} else if (keycode == KeyEvent::VKEY_LEFT) {
			// ←移動
			if (textPtr >= 0) {
				textPtr--;
				repaint();
			}
		} else if (keycode == KeyEvent::VKEY_RIGHT) {
			// →移動
			if (textPtr < textLen - 1) {
				textPtr++;
				repaint();
			}
		} else if (keycode == KeyEvent::VKEY_ENTER) {
			// 確定
			getParent()->onEvent(&this->textEvent);
			return;
		} else if (keycode < 128) {
			// 1文字挿入
			insertCharacter(keycode);
			repaint();
		}
	// フォーカス状態変更
	} else if (event->getType() == Event::FOCUS_IN || event->getType() == Event::FOCUS_OUT) {
		repaint();
		getParent()->onEvent(&this->focusEvent);
	}
}
