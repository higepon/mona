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

/** コンストラクタ */
Container::Container()
{
	_controlList = new LinkedList();
}

/** デストラクタ */
Container::~Container() {
	delete(_controlList);
}


/**
 部品を得る
 @return 活性部品（なければNULL）
*/
Control *Container::findChild()
{
	// NULLチェック
	if (_controlList->getLength() == 0) return NULL;

	// 後ろからチェックしていく
	for (int i = _controlList->getLength() - 1; i >= 0; i--) {
		Control *c = (Control *)_controlList->get(i);
		if (c->getFocused() == true) return c;
	}
	return NULL;
}

/**
 部品を得る
 @param x 指定するX座標
 @param y 指定するY座標
 */
Control *Container::findChild(int x, int y)
{
	// NULLチェック
	if (_controlList->getLength() == 0) return NULL;
	
	// 後ろからチェックしていく
	for (int i = _controlList->getLength() - 1; i >= 0; i--) {
		Control *c = (Control *)_controlList->get(i);
		Rect *rect = c->getRect();
		// マウスカーソルがある範囲に部品があるかどうかチェック
		if (rect->x <= x && x <= rect->x + rect->width && 
			rect->y <= y && y <= rect->y + rect->height)
		{
			return c;
		}
	}
	return NULL;
}

/**
 指定した部品を追加する
 @param control 指定する部品
 */
void Container::add(Control *control)
{
	control->setParent(this);
	control->create();
	_controlList->add(control);
}

/**
 指定した部品を削除する
 @param control 指定する部品
 @return 削除された部品（なければNULL）
 */
void Container::remove(Control *control)
{
	_controlList->remove(control);
}

/** イベント処理 */
void Container::postEvent(Event *event)
{
	// 非活性の時はイベントを受け付けない
	if (this->enabled == false) return;

	// 活性部品にキーイベントを投げる
	if (event->type == KEY_PRESSED || event->type == KEY_RELEASED) {
		Control *control = findChild();
		// 部品でイベントが起こった
		if (control != NULL) {
			event->source = control;
			control->onEvent(event);
		// 部品以外でイベントが起こった
		} else {
			onEvent(event);
		}
	// マウスクリック
	} else if (event->type == MOUSE_PRESSED) {
		MouseEvent *me = (MouseEvent *)event;
		// マウスイベントが起こった部品を探す
		Control *control = findChild(me->x, me->y);
		// 部品でイベントが起こった
		if (control != NULL) {
			// イベントが起こった部品以外をフォーカスアウト状態にする
			for (int i = 0; i < _controlList->getLength(); i++) {
				Control *c = (Control *)_controlList->get(i);
				if (c != control) {
					c->setFocused(false);
				}
			}
			control->setFocused(true);
			event->source = control;
			Rect *rect = control->getRect();
			me->x = me->x - rect->x;
			me->y = me->y - rect->y;
			//syscall_print("MOUSE_PRESSED,");
			control->onEvent(event);
		// 部品以外でイベントが起こった
		} else {
			// 部品をフォーカスアウト状態にする
			for (int i = 0; i < _controlList->getLength(); i++) {
				Control *c = (Control *)_controlList->get(i);
				c->setFocused(false);
			}
			//syscall_print("MOUSE_PRESSED,");
			onEvent(event);
		}
	// マウスリリース
	} else if (event->type == MOUSE_RELEASED) {
		MouseEvent *me = (MouseEvent *)event;
		// マウスイベントが起こった部品を探す
		Control *control = findChild(me->x, me->y);
		// 部品でイベントが起こった
		if (control != NULL) {
			event->source = control;
			Rect *rect = control->getRect();
			me->x = me->x - rect->x;
			me->y = me->y - rect->y;
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

/** 再描画 */
void Container::repaint()
{
	if (this->_buffer == NULL) return;
	
	onPaint(this->_g);

	// 自分の領域を更新する
	update();

	// 子部品を再描画する
	for(int i = 0; i < _controlList->getLength(); i++) {
		Control *control = (Control *)_controlList->get(i);
		control->repaint();
	}
}
