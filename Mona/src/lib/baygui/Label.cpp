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

Label::Label(char* text)
{
	this->align = Label::LEFT;
	this->text = text;
}

Label::Label(char* text, int align)
{
	this->align = align;
	this->text = text;
}

Label::~Label()
{
}

void Label::setText(char* text)
{
	this->text = text;
	repaint();
}

void Label::paint(Graphics* g)
{
	int w = getWidth();
	int h = getHeight();
	
	// 塗りつぶし
	g->setColor(getBackground());
	g->fillRect(0, 0, w, h);

	// 文字
	int fw = getFontMetrics()->getWidth(getText());
	int fh = getFontMetrics()->getHeight(getText());
	if (getEnabled() == true) {
		g->setColor(getForeground());
	} else {
		g->setColor(Color::gray);
	}
	if (this->align == Label::RIGHT) {
		g->drawText(getText(), (w - fw), (h - fh) / 2);
	} else if (this->align == Label::CENTER) {
		g->drawText(getText(), (w - fw) / 2, (h - fh) / 2);
	} else {
		g->drawText(getText(), 0, (h - fh) / 2);
	}
}
