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
	selectedIndex =  0;
	_dataList = new LinkedList();
	_itemEvent = new Event(ITEM_SELECTED, this);
}

/** デストラクタ */
ListBox::~ListBox()
{
	delete(_dataList);
	delete(_itemEvent);
}

/** 選択位置を得る */
int ListBox::getSelectedIndex()
{
	return selectedIndex;
}

/** 選択項目を得る */
char *ListBox::getSelectedItem()
{
	return ((String *)_dataList->getItem(selectedIndex)->data)->toString();
}

/** index 番目を選択する */
void ListBox::select(int index)
{
	selectedIndex = index;
	if (firstpaint == true) {
		repaint();
	}
}

/** 項目を追加する */
void ListBox::add(char *item)
{
	_dataList->add(new LinkedItem(new String(item)));
}

/** index 番目の項目を削除する */
void ListBox::remove(int index)
{
	_dataList->remove(_dataList->getItem(index));
}

/** 再描画 */
void ListBox::repaint()
{
	int i;
	
	if (firstpaint == false)
		firstpaint = true;

	// 枠線
	if (focused == true) {
		_g->setColor(0,128,255);
		_g->drawRect(0, 0, width, height);
	} else {
		_g->setColor(200,200,200);
		_g->drawRect(0, 0, width, height);
	}
	_g->setColor(0,0,0);
	_g->drawRect(1, 1, width - 2, height - 2);
	_g->setColor(255,255,255);
	_g->fillRect(2, 2, width - 3, height - 3);

	// 文字
	int fh = FontManager::getInstance()->getHeight();
	for (i = 0; i < _dataList->getLength(); i++) {
		if (selectedIndex == i) {
			_g->setColor(0,128,255);
			_g->fillRect(3, 3 + (16 * i), width - 5, 17);
			_g->setColor(255,255,255);
			_g->drawText(((String *)_dataList->getItem(i)->data)->toString(), 
				4, 4 + (16 * i) + (16 - fh) / 2);
		} else {
			if (enabled == true) {
				_g->setColor(0,0,0);
			} else {
				_g->setColor(128,128,128);
			}
			_g->drawText(((String *)_dataList->getItem(i)->data)->toString(), 
				4, 4 + (16 * i) + (16 - fh) / 2);
		}
	} 
}

/** イベント処理 */
void ListBox::postEvent(Event *event)
{
	// キー押下
	if (event->type == KEY_PRESSED && enabled == true) {
		int keycode = ((KeyEvent *)event)->key;
		if (keycode == VKEY_UP || keycode == VKEY_UP_QEMU) {
			if (selectedIndex > 0) {
				selectedIndex--;
				if (firstpaint == true) {
					repaint();
					Control::postEvent(_itemEvent);
				}
			}
		} else if (keycode == VKEY_DOWN || keycode == VKEY_DOWN_QEMU) {
			if (selectedIndex < _dataList->getLength() - 1) {
				selectedIndex++;
				if (firstpaint == true) {
					repaint();
					Control::postEvent(_itemEvent);
				}
			}
		} else if (keycode == VKEY_ENTER) {
			Control::postEvent(_itemEvent);
		}
	// マウス押下
	} else if (event->type == MOUSE_PRESSED && enabled == true) {
		int my = ((MouseEvent *)event)->y;
		//printf("y = %d\n", my);
		select((my - 7) / 16);
		Control::postEvent(_itemEvent);
	// フォーカス状態変更
	} else if (event->type == FOCUS_IN || event->type == FOCUS_OUT) {
		if (firstpaint == true) {
			repaint();
			Control::postEvent(_focusEvent);
		}
	} else {
		Control::postEvent(event);
	}
}
