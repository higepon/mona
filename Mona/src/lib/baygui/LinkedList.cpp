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
LinkedList::LinkedList()
{
	this->dataListLength = 0;
	this->firstItem = NULL;
	this->endItem = NULL;
}

/** デストラクタ */
LinkedList::~LinkedList()
{
	removeAll();
}

/** ObjectをLinkedItemに変換する */
LinkedItem *LinkedList::getLinkedItem(Object *o)
{
	// NULLチェック
	if (o == NULL || this->firstItem == NULL) return NULL;
	
	for (int i = 0; i < getLength(); i++) {
		LinkedItem *item = _get(i);
		if (item->data == o) return item;
	}
	return NULL;
}

/** 指定した順番の項目を得る */
LinkedItem *LinkedList::_get(int index)
{
	// NULLチェック
	if (index >= dataListLength) return NULL;
	if (this->firstItem == NULL) return NULL;
	// 最初と最後チェック
	if (index == 0) return this->firstItem;
	if (index == dataListLength - 1) return this->endItem;
	// index回ループ
	LinkedItem *item = firstItem;
	for (int i = 0; i < index; i++) {
		item = item->next;
	}
	return item;
}

/** 指定した順番の項目を得る */
Object *LinkedList::get(int index)
{
	LinkedItem *item = _get(index);
	if (item != NULL) {
		return item->data;
	} else {
		return NULL;
	}
}

/** 一番最初の項目を得る */
Object *LinkedList::getFirst()
{
	if (this->firstItem != NULL) {
		return this->firstItem->data;
	} else {
		return NULL;
	}
}

/** 一番最後の項目を得る */
Object *LinkedList::getLast()
{
	if (this->endItem != NULL) {
		return this->endItem->data;
	} else {
		return NULL;
	}
}

/** 追加 */
void LinkedList::add(Object *o)
{
	LinkedItem *item = new LinkedItem(o);
	if (item == NULL) {
		return;
	} else if (this->firstItem == NULL) {
		this->firstItem = item;
		this->endItem = item;
	} else {
		LinkedItem *s = firstItem;
		while (s->next != NULL) {
			s = s->next;
		}
		item->prev = s;
		s->next = item;
		this->endItem = item;
	}
	this->dataListLength++;
}

/** 削除 */
void LinkedList::remove(int index)
{
	remove(get(index));
}

/** 削除 */
void LinkedList::remove(Object *o)
{
	LinkedItem *item = getLinkedItem(o);
	if (item == NULL) {
		return;
	} else if (item->next == NULL) {
		if (item->prev != NULL) {
			this->endItem = item->prev;
			this->endItem->next = NULL;
		} else {
			this->firstItem = NULL;
			this->endItem = NULL;
		}
		item->prev = NULL;
		item->next = NULL;
		delete(item);
	} else {
		if (item->prev != NULL) {
			item->prev->next = item->next;
			item->next->prev = item->prev;
		} else {
			this->firstItem = item->next;
			this->firstItem->prev = NULL;
		}
		item->prev = NULL;
		item->next = NULL;
		delete(item);
	}
	dataListLength--;
}

/** 全削除 */
void LinkedList::removeAll()
{
	while (this->firstItem != NULL) {
		remove(0);
	}
	this->dataListLength = 0;
}

#if 0
/** 指定した項目を一番最後に持っていく */
void LinkedList::sort(Object *o)
{
	LinkedItem *item = getLinkedItem(o);
	if (item == NULL) {
		return;
	} else if (item->next == NULL) {
		return;
	} else {
		if (item->prev != NULL) {
			item->prev->next = item->next;
			item->next->prev = item->prev;
		} else {
			this->firstItem = item->next;
			this->firstItem->prev = NULL;
		}
		item->prev = endItem;
		this->endItem->next = item;
		item->next = NULL;
		this->endItem = item;
	}
}
#endif
