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

Button::Button()
{
	this->pushed = false;
	this->label  = "button";
	setBounds(0, 0, 40, 20);
}

Button::Button(char* label)
{
	this->pushed = false;
	this->label  = label;
	setBounds(0, 0, 40, 20);
}

Button::~Button()
{
}

void Button::setLabel(char* label)
{
	this->label = label;
	repaint();
}

void Button::onPaint(Graphics* g)
{
	int w = getWidth();
	int h = getHeight();
	
	// 一度背景色でクリア
	g->setColor(getBackground());
	g->fillRect(0, 0, w, h);
	
	// 枠を描画
	g->setColor(Color::black);
	g->drawLine(2, 0, w - 3, 0);
	g->drawLine(2, h - 1, w - 3, h - 1);
	g->drawLine(0, 2, 0, h - 3);
	g->drawLine(w - 1, 2, w - 1, h - 3);
	g->drawLine(1, 1, 1, 1);
	g->drawLine(1, h - 2 , 1, h - 2);
	g->drawLine(w - 2 , 1, w - 2, 1);
	g->drawLine(w - 2 , h - 2, w - 2, h - 2);
	
	if (this->pushed) {
		g->setColor(Color::white);
		g->drawLine(2, h - 2, w - 3, h - 2);
		g->drawLine(w - 2, 2, w - 2, h - 3);
		g->drawLine(w - 3 , h - 3, w - 3, h - 3);
		g->setColor(Color::gray);
		g->drawLine(1, 2, 1, h - 3);
		g->drawLine(2, 1, w - 3, 1);
	} else {
		g->setColor(Color::gray);
		g->drawLine(2, h - 2, w - 3, h - 2);
		g->drawLine(w - 2, 2, w - 2, h - 3);
		g->drawLine(w - 3 , h - 3, w - 3, h - 3);
		g->setColor(Color::white);
		g->drawLine(1, 2, 1, h - 3);
		g->drawLine(2, 1, w - 3, 1);
	}
	
	// 文字
	int fw = getFontMetrics()->getWidth(getLabel());
	int fh = getFontMetrics()->getHeight(getLabel());
	int x = (w - fw) / 2;
	int y = (h - fh) / 2;
	if (this->pushed) {
		x++;
		y++;
	}
	if (getEnabled() == true) {
		g->setColor(getForeground());
	} else {
		g->setColor(Color::gray);
	}
	g->drawText(getLabel(), x, y);
}

void Button::onEvent(Event* event) {
	// 非活性の時はイベントを受け付けない
	if (getEnabled() == false) return;

	if (event->getType() == MouseEvent::MOUSE_PRESSED) {
		this->pushed = true;
		repaint();
		getParent()->onEvent(event);
	} else if (event->getType() == MouseEvent::MOUSE_RELEASED) {
		this->pushed = false;
		repaint();
		getParent()->onEvent(event);
	}
}
