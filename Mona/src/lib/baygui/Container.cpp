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
	if (_controlList->endItem == NULL) return NULL;

	// 後ろからチェックしていく
	LinkedItem *item = _controlList->endItem;
	Control *c = (Control *)item->data;
	if (c->getFocused() == true) return c;
	while (item->prev != NULL) {
		item = item->prev;
		c = (Control *)item->data;
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
	if (_controlList->endItem == NULL) return NULL;
	// 後ろからチェックしていく
	LinkedItem *item = _controlList->endItem;
	Control *c = (Control *)item->data;
	Rect *rect = c->getRect();
	// マウスカーソルがある範囲に部品があるかどうかチェック
	if (c->getIconified() == false &&
		rect->x <= x && x <= rect->x + rect->width && 
		rect->y <= y && y <= rect->y + rect->height)
	{
		return c;
	} else if (c->getIconified() == true &&
		rect->x <= x && x <= rect->x + rect->width && 
		rect->y <= y && y <= rect->y + INSETS_TOP)
	{
		return c;
	}
	while (item->prev != NULL) {
		item = item->prev;
		c = (Control *)item->data;
		rect = c->getRect();
		// マウスカーソルがある範囲に部品があるかどうかチェック
		if (c->getIconified() == false &&
			rect->x <= x && x <= rect->x + rect->width && 
			rect->y <= y && y <= rect->y + rect->height)
		{
			return c;
		} else if (c->getIconified() == true &&
			rect->x <= x && x <= rect->x + rect->width && 
			rect->y <= y && y <= rect->y + INSETS_TOP)
		{
			return c;
		}
	}
	return NULL;
}

/**
 部品をLinkedItemに変換する
 @param control 指定する部品
 @return 変換できなければNULL
 */
LinkedItem *Container::getLinkedItem(Control *control)
{
	// NULLチェック
	if (_controlList->endItem == NULL) return NULL;

	// 後ろからチェックしていく
	LinkedItem *item = _controlList->endItem;
	Control *c = (Control *)item->data;
	if (control == c) return item;
	while (item->prev != NULL) {
		item = item->prev;
		c = (Control *)item->data;
		if (control == c) return item;
	}
	return NULL;
}
