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
ListBox::ListBox()
{
	this->selectedIndex =  -1;
	this->dataList = new LinkedList();
	this->itemEvent.type = ITEM_SELECTED;
	this->itemEvent.source = this;
}

/** デストラクタ */
ListBox::~ListBox()
{
	delete(this->dataList);
}

/** 選択項目を得る */
char *ListBox::getSelectedItem()
{
	return ((String *)dataList->get(this->selectedIndex))->getBytes();
}

/** index 番目を選択する */
void ListBox::select(int index)
{
	this->selectedIndex = index;
	repaint();
}

/** 項目を追加する */
void ListBox::add(char *item)
{
	dataList->add(new String(item));
}

/** index 番目の項目を削除する */
void ListBox::remove(int index)
{
	dataList->remove(index);
}

/** 再描画 */
void ListBox::onPaint(Graphics *g)
{
	int i, w = this->width, h = this->height;
	
	// 外枠
	if (focused == true && enabled == true) {
		g->setColor(0,128,255);
		g->drawRect(0, 0, w, h);
	} else {
		g->setColor(getBackground());
		g->drawRect(0, 0, w, h);
	}
	
	// 内枠
	g->setColor(~this->foreColor);
	g->fillRect(1, 1, w - 2, h - 2);
	g->setColor(this->foreColor);
	g->drawRect(1, 1, w - 2, h - 2);

	// 文字
	for (i = 0; i < dataList->getLength(); i++) {
		if (selectedIndex == i && enabled == true) {
			g->setColor(0, 128, 255);
			g->fillRect(3, (16 * i) + 3, w - 6, 16);
			g->setColor(~this->foreColor);
			g->drawText(((String *)dataList->get(i))->getBytes(), 4, (16 * i) + 6);
		} else {
			g->setColor(this->foreColor);
			g->drawText(((String *)dataList->get(i))->getBytes(), 4, (16 * i) + 6);
		}
	} 
}

/** イベント処理 */
void ListBox::onEvent(Event *event)
{
	// 非活性の時はイベントを受け付けない
	if (this->enabled == false) return;

	// キー押下
	if (event->type == KEY_PRESSED) {
		int keycode = ((KeyEvent *)event)->keycode;
		if (keycode == VKEY_UP) {
			if (this->selectedIndex > 0) {
				this->selectedIndex--;
				repaint();
				getParent()->onEvent(&this->itemEvent);
			}
		} else if (keycode == VKEY_DOWN) {
			if (this->selectedIndex < this->dataList->getLength() - 1) {
				this->selectedIndex++;
				repaint();
				getParent()->onEvent(&this->itemEvent);
			}
		} else if (keycode == VKEY_ENTER) {
			getParent()->onEvent(&this->itemEvent);
		}
	// マウス押下
	} else if (event->type == MOUSE_PRESSED) {
		int my = ((MouseEvent *)event)->y;
		//printf("y = %d,", my);
		select((my - 3) / 16);
		getParent()->onEvent(&this->itemEvent);
	// フォーカス状態変更
	} else if (event->type == FOCUS_IN || event->type == FOCUS_OUT) {
		repaint();
		getParent()->onEvent(&this->itemEvent);
	}
}
