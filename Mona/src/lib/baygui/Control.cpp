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

Control::Control()
{
	this->enabled = true;
	this->focused = false;
	this->x = this->y = 0;
	this->height = this->width = 100;
	this->fontStyle = Font::PLAIN;
	this->_buffer = NULL;
	this->_g = NULL;
	this->_metrics = new FontMetrics();
	this->focusEvent.setType(Event::FOCUS_IN);
	this->focusEvent.setSource(this);
	this->backColor = Color::DEFAULT_BACKCOLOR;
	this->foreColor = Color::DEFAULT_FORECOLOR;
}

Control::~Control()
{
	onExit();
}

void Control::onStart()
{
	if (this->_buffer != NULL) return;

	// 内部バッファー、描画オブジェクトの生成
	this->_buffer = new Image(width, height);
	this->_g = new Graphics(this->_buffer);
}

void Control::onExit()
{
	delete(_buffer);
	delete(_g);
	delete(_metrics);
}

void Control::onEvent(Event *event)
{
}

void Control::onPaint(Graphics *g)
{
}

void Control::postEvent(Event *event)
{
	onEvent(event);
	// 親部品にイベントを投げる
	if (getParent() != NULL) {
		getParent()->onEvent(event);
	}
}

void Control::repaint()
{
	if (this->_buffer == NULL) return;
	setFontStyle(this->fontStyle);
	onPaint(this->_g);
	update();
}

void Control::update()
{
	Control *c = getMainWindow();
	c->getGraphics()->drawImage(this->_buffer, this->x, this->y);
	c->update();
}

Control *Control::getMainWindow()
{
	if (this->parent == NULL) {
		return this;
	} else {
		return this->parent->getMainWindow();
	}
}

void Control::setEnabled(bool enabled)
{
	this->enabled = enabled;
}

void Control::setFocused(bool focused)
{
	if (this->focused == true && focused == false) {
		//syscall_print("FOCUS_OUT,");
		this->focused = focused;
		this->focusEvent.setType(Event::FOCUS_OUT);
		postEvent(&this->focusEvent);
	} else if (this->focused == false && focused == true) {
		//syscall_print("FOCUS_IN,");
		this->focused = focused;
		this->focusEvent.setType(Event::FOCUS_IN);
		postEvent(&this->focusEvent);
	}
}

void Control::setVisible(bool visible)
{
	this->visible = visible;
}

void Control::setRect(int x, int y, int width, int height)
{
	this->x = x;
	this->y = y;
	this->height = height;
	this->width = width;
	this->rect.setLocation(x, y);
	this->rect.setSize(width, height);
}

void Control::setLocation(int x, int y)
{
	if (this->x == x && this->y == y) return;
	
	this->x = x;
	this->y = y;
	this->rect.setLocation(x, y);
}

void Control::setParent(Container *parent)
{
	this->parent = parent;
}

void Control::setBackground(unsigned int backColor)
{
	this->backColor = backColor;
}

void Control::setForeground(unsigned int foreColor)
{
	this->foreColor = foreColor;
}

void Control::setFontStyle(int style)
{
	if (this->_metrics != NULL) {
		_metrics->setFontStyle(style);
	}
	if (this->_g != NULL) {
		_g->setFontStyle(style);
	}
	this->fontStyle = style;
}
