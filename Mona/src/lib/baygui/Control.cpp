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

Control::Control()
{
	this->enabled = true;
	this->focused = false;
	this->x = this->y = this->height = this->width = 0;
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
