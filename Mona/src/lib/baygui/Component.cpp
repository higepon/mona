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

Component::Component()
{
	this->parent = NULL;
	this->enabled = true;
	this->focused = false;
	this->visible = true;
	this->x = this->y = this->height = this->width = 10;
	this->backColor = Color::lightGray;
	this->foreColor = Color::black;
	this->fontStyle = Font::PLAIN;
	this->_g = NULL;
	this->_buffer = NULL;
	this->_metrics = new FontMetrics();
	this->focusEvent.setType(Event::FOCUS_IN);
	this->focusEvent.setSource(this);
}

Component::~Component()
{
	removeNotify();
}

void Component::addNotify()
{
	if (this->_buffer != NULL) return;

	// 内部バッファー、描画オブジェクトの生成
	this->_buffer = new Image(width, height);
	this->_g = new Graphics(this->_buffer);
}

void Component::removeNotify()
{
	delete(_buffer);
	delete(_g);
	delete(_metrics);
}

void Component::processEvent(Event* event)
{
}

void Component::paint(Graphics* g)
{
}

void Component::dispatchEvent(Event *event)
{
	processEvent(event);
	// 親部品にイベントを投げる
	if (getParent() != NULL) {
		getParent()->processEvent(event);
	}
}

void Component::repaint()
{
	if (this->_buffer == NULL) return;
	setFontStyle(this->fontStyle);
	paint(this->_g);
	update();
}

void Component::update()
{
	Component* c = getMainWindow();
	c->getGraphics()->drawImage(this->_buffer, this->x, this->y);
	c->update();
}

Component* Component::getMainWindow()
{
	return (this->parent == NULL) ? this : this->parent->getMainWindow();
}

void Component::setEnabled(bool enabled)
{
	this->enabled = enabled;
}

void Component::setFocused(bool focused)
{
	if (this->focused == true && focused == false) {
		//syscall_print("FOCUS_OUT,");
		this->focused = focused;
		this->focusEvent.setType(Event::FOCUS_OUT);
		dispatchEvent(&this->focusEvent);
	} else if (this->focused == false && focused == true) {
		//syscall_print("FOCUS_IN,");
		this->focused = focused;
		this->focusEvent.setType(Event::FOCUS_IN);
		dispatchEvent(&this->focusEvent);
	}
}

void Component::setVisible(bool visible)
{
	this->visible = visible;
}

void Component::setBounds(int x, int y, int width, int height)
{
	this->x = x;
	this->y = y;
	this->height = height;
	this->width = width;
	this->bounds.setBounds(x, y, width, height);
}

void Component::setLocation(int x, int y)
{
	if (this->x == x && this->y == y) return;
	
	this->x = x;
	this->y = y;
	this->bounds.setLocation(x, y);
}

void Component::setParent(Container *parent)
{
	this->parent = parent;
}

void Component::setBackground(dword backColor)
{
	this->backColor = backColor;
}

void Component::setForeground(dword foreColor)
{
	this->foreColor = foreColor;
}

void Component::setFontStyle(int style)
{
	if (this->_metrics != NULL) {
		_metrics->setFontStyle(style);
	}
	if (this->_g != NULL) {
		_g->setFontStyle(style);
	}
	this->fontStyle = style;
}
