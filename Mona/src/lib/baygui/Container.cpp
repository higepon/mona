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

Container::Container()
{
	this->controlList = new LinkedList();
}

Container::~Container() {
	delete(this->controlList);
}

Control *Container::findChild()
{
	// NULLチェック
	if (this->controlList->getLength() == 0) return NULL;

	// 後ろからチェックしていく
	for (int i = this->controlList->getLength() - 1; i >= 0; i--) {
		Control *c = (Control *)this->controlList->get(i);
		if (c->getFocused() == true) return c;
	}
	return NULL;
}

Control *Container::findChild(int x, int y)
{
	// NULLチェック
	if (this->controlList->getLength() == 0) return NULL;
	
	// 後ろからチェックしていく
	for (int i = this->controlList->getLength() - 1; i >= 0; i--) {
		Control *c = (Control *)this->controlList->get(i);
		Rect *rect = c->getRect();
		// マウスカーソルがある範囲に部品があるかどうかチェック
		if (rect->getX() <= x && x <= rect->getX() + rect->getWidth() && 
			rect->getY() <= y && y <= rect->getY() + rect->getHeight())
		{
			return c;
		}
	}
	return NULL;
}

void Container::add(Control *control)
{
	control->setParent(this);
	control->onStart();
	this->controlList->add(control);
}

void Container::remove(Control *control)
{
	this->controlList->remove(control);
}

void Container::postEvent(Event *event)
{
	// 非活性の時はイベントを受け付けない
	if (getEnabled() == false) return;

	// 活性部品にキーイベントを投げる
	if (event->getType() == KeyEvent::KEY_PRESSED || event->getType() == KeyEvent::KEY_RELEASED) {
		Control *control = findChild();
		// 部品でイベントが起こった
		if (control != NULL) {
			event->setSource(control);
			control->onEvent(event);
		}
		// 部品以外でイベントが起こった
		onEvent(event);
	// マウスクリック
	} else if (event->getType() == MouseEvent::MOUSE_PRESSED) {
		MouseEvent *me = (MouseEvent *)event;
		// マウスイベントが起こった部品を探す
		Control *control = findChild(me->getX(), me->getY());
		// 部品でイベントが起こった
		if (control != NULL) {
			// イベントが起こった部品以外をフォーカスアウト状態にする
			for (int i = 0; i < this->controlList->getLength(); i++) {
				Control *c = (Control *)this->controlList->get(i);
				if (c != control) {
					c->setFocused(false);
				}
			}
			control->setFocused(true);
			event->setSource(control);
			Rect *rect = control->getRect();
			me->setX(me->getX() - rect->getX());
			me->setY(me->getY() - rect->getY());
			//syscall_print("MOUSE_PRESSED,");
			control->onEvent(event);
		// 部品以外でイベントが起こった
		} else {
			// 部品をフォーカスアウト状態にする
			for (int i = 0; i < this->controlList->getLength(); i++) {
				Control *c = (Control *)this->controlList->get(i);
				c->setFocused(false);
			}
			//syscall_print("MOUSE_PRESSED,");
			onEvent(event);
		}
	// マウスリリース
	} else if (event->getType() == MouseEvent::MOUSE_RELEASED) {
		MouseEvent *me = (MouseEvent *)event;
		// マウスイベントが起こった部品を探す
		Control *control = findChild(me->getX(), me->getY());
		// 部品でイベントが起こった
		if (control != NULL) {
			event->setSource(control);
			Rect *rect = control->getRect();
			me->setX(me->getX() - rect->getX());
			me->setY(me->getY() - rect->getY());
			//syscall_print("MOUSE_RELEASED,");
			control->onEvent(event);
		// 部品以外でイベントが起こった
		} else {
			//syscall_print("MOUSE_RELEASED,");
			onEvent(event);
		}
	} else {
		onEvent(event);
	}
}

void Container::repaint()
{
	if (getBuffer() == NULL) return;
	
	onPaint(getGraphics());

	// 自分の領域を更新する
	update();

	// 子部品を再描画する
	for(int i = 0; i < this->controlList->getLength(); i++) {
		Control *control = (Control *)this->controlList->get(i);
		control->repaint();
	}
}
