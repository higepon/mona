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

LinkedList::LinkedList()
{
	this->dataListLength = 0;
	this->firstItem = NULL;
	this->endItem = NULL;
}

LinkedList::~LinkedList()
{
	while (this->firstItem != NULL) {
		// データそのものをdeleteすると他に持っている人が
		// いるときに困るためdeleteはしない
		remove(0);
	}
	this->dataListLength = 0;
}

LinkedItem *LinkedList::getLinkedItem(int index)
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

LinkedItem *LinkedList::getLinkedItem(Object *o)
{
	// NULLチェック
	if (o == NULL || this->firstItem == NULL) return NULL;
	
	for (int i = 0; i < getLength(); i++) {
		LinkedItem *item = getLinkedItem(i);
		if (item->data == o) return item;
	}
	return NULL;
}

Object *LinkedList::get(int index)
{
	LinkedItem *item = getLinkedItem(index);
	if (item != NULL) {
		return item->data;
	} else {
		return NULL;
	}
}

Object *LinkedList::getFirst()
{
	if (this->firstItem != NULL) {
		return this->firstItem->data;
	} else {
		return NULL;
	}
}

Object *LinkedList::getLast()
{
	if (this->endItem != NULL) {
		return this->endItem->data;
	} else {
		return NULL;
	}
}

void LinkedList::add(Object *obj)
{
	// NULLチェック
	if (obj == NULL) return;
	
	LinkedItem *item = new LinkedItem();
	item->data = obj;
	
	if (this->firstItem == NULL) {
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

Object *LinkedList::remove(int index)
{
	return remove(get(index));
}

Object *LinkedList::remove(Object *o)
{
	LinkedItem *item = getLinkedItem(o);
	if (item == NULL) {
		return NULL;
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
		//delete(item);
		dataListLength--;
		return item->data;
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
		//delete(item);
		dataListLength--;
		return item->data;
	}
}

void LinkedList::removeAll()
{
	while (this->firstItem != NULL) {
		Object *obj = remove(0);
		delete(obj);
	}
	this->dataListLength = 0;
}
