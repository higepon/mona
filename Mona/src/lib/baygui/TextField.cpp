/*
Copyright (c) 2005 bayside

Permission is hereby granted, free of charge, to any person 
obtaining a copy of this software and associated documentation files 
(the "Software"), to deal in the Software without restriction, 
including without limitation the rights to use, copy, modify, merge, 
publish, distribute, sublicense, and/or sell copies of the Software, 
and to permit persons to whom the Software is furnished to do so, 
subject to the following conditions:

The above copyright notice and this permission notice shall be 
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY 
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "baygui.h"

namespace baygui {
	TextField::TextField()
	{
		this->textPtr = -1;
		this->textLen = 0;
		this->offx = 3;
		this->offy = 3;
		memset(text, 0, MAX_TEXT_LEN);
		this->textEvent.setType(Event::TEXT_CHANGED);
		this->textEvent.setSource(this);
		setBackground(Color::white);
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

	void TextField::setText(char* text)
	{
		strcpy(this->text, text);
		this->textLen = strlen(text);
		this->textPtr = this->textLen - 1;
		repaint();
	}

	void TextField::paint(Graphics* g)
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
			g->setColor(Color::gray);
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

	void TextField::processEvent(Event* event)
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
				getParent()->processEvent(&this->textEvent);
				return;
			} else if (keycode < 128) {
				// 1文字挿入
				insertCharacter(keycode);
				repaint();
			}
		// フォーカス状態変更
		} else if (event->getType() == Event::FOCUS_IN || event->getType() == Event::FOCUS_OUT) {
			repaint();
			getParent()->processEvent(&this->focusEvent);
		}
	}
}
