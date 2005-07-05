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

LinkedList::LinkedList()
{
	this->dataListLength = 0;
	this->firstItem = NULL;
	this->endItem = NULL;
	this->autoDelete = true;
}

LinkedList::LinkedList(bool autoDelete)
{
	this->dataListLength = 0;
	this->firstItem = NULL;
	this->endItem = NULL;
	this->autoDelete = autoDelete;
}

LinkedList::~LinkedList()
{
	removeAll();
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
		if (this->autoDelete == true) {
			delete(item);
		}
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
		if (this->autoDelete == true) {
			delete(item);
		}
		dataListLength--;
		return item->data;
	}
}

void LinkedList::removeAll()
{
	while (this->firstItem != NULL) {
		Object *obj = remove(0);
		if (this->autoDelete == true) {
			delete(obj);
		}
	}
	this->dataListLength = 0;
}
