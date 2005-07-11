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
}

Container::~Container()
{
	this->controlList.removeAll();
}

Component* Container::getComponent()
{
	// NULLチェック
	int I = this->controlList.size();
	if (I == 0) return NULL;

	// 後ろからチェックしていく
	for (int i = I - 1; i >= 0; i--) {
		Component* c = (Component *)this->controlList.get(i);
		if (c->getFocused() == true) return c;
	}
	return NULL;
}

Component* Container::getComponentAt(int x, int y)
{
	// NULLチェック
	int I = this->controlList.size();
	if (I == 0) return NULL;
	
	// 後ろからチェックしていく
	for (int i = I - 1; i >= 0; i--) {
		Component* c = (Component *)this->controlList.get(i);
		Rectangle* bounds = c->getBounds();
		// マウスカーソルがある範囲に部品があるかどうかチェック
		if (bounds->x <= x && x <= bounds->x + bounds->width && 
			bounds->y <= y && y <= bounds->y + bounds->height)
		{
			return c;
		}
	}
	return NULL;
}

void Container::add(Component* control)
{
	control->setParent(this);
	control->addNotify();
	this->controlList.add(control);
}

void Container::remove(Component* control)
{
	this->controlList.remove(control);
}

void Container::postEvent(Event* event)
{
	// 非活性の時はイベントを受け付けない
	if (getEnabled() == false) return;

	// 活性部品にキーイベントを投げる
	if (event->getType() == KeyEvent::KEY_PRESSED || event->getType() == KeyEvent::KEY_RELEASED) {
		Component* control = getComponent();
		// 部品でイベントが起こった
		if (control != NULL) {
			event->setSource(control);
			control->onEvent(event);
		}
		// 部品以外でイベントが起こった
		onEvent(event);
	// マウスクリック
	} else if (event->getType() == MouseEvent::MOUSE_PRESSED) {
		MouseEvent* me = (MouseEvent *)event;
		// マウスイベントが起こった部品を探す
		Component* control = getComponentAt(me->getX(), me->getY());
		// 部品でイベントが起こった
		if (control != NULL) {
			// イベントが起こった部品以外をフォーカスアウト状態にする
			int I = this->controlList.size();
			for (int i = 0; i < I; i++) {
				Component* c = (Component *)this->controlList.get(i);
				if (c != control) {
					c->setFocused(false);
				}
			}
			control->setFocused(true);
			event->setSource(control);
			Rectangle* bounds = control->getBounds();
			me->setX(me->getX() - bounds->x);
			me->setY(me->getY() - bounds->y);
			//syscall_print("MOUSE_PRESSED,");
			control->onEvent(event);
		// 部品以外でイベントが起こった
		} else {
			// 部品をフォーカスアウト状態にする
			int I = this->controlList.size();
			for (int i = 0; i < I; i++) {
				Component* c = (Component *)this->controlList.get(i);
				c->setFocused(false);
			}
			//syscall_print("MOUSE_PRESSED,");
			onEvent(event);
		}
	// マウスリリース
	} else if (event->getType() == MouseEvent::MOUSE_RELEASED) {
		MouseEvent* me = (MouseEvent *)event;
		// マウスイベントが起こった部品を探す
		Component* control = getComponentAt(me->getX(), me->getY());
		// 部品でイベントが起こった
		if (control != NULL) {
			event->setSource(control);
			Rectangle* bounds = control->getBounds();
			me->setX(me->getX() - bounds->x);
			me->setY(me->getY() - bounds->y);
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
	int I = this->controlList.size();
	for(int i = 0; i < I; i++) {
		Component* control = (Component *)this->controlList.get(i);
		control->repaint();
	}
}
