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

Container::Container()
{
	this->controlList = new LinkedList();
}

Container::~Container() {
	this->controlList->removeAll();
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
