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
TwoWayList::TwoWayList()
{
	firstItem = NULL;
	endItem = NULL;
}

/** デストラクタ */
TwoWayList::~TwoWayList()
{
	removeAll();
}

/** 追加 */
void TwoWayList::add(TwoWayItem *item)
{
	if (item == NULL) {
		return;
	} else if (firstItem == NULL) {
		firstItem = item;
		endItem = item;
	} else {
		TwoWayItem *s = firstItem;
		while (s->next != NULL) {
			s = s->next;
		}
		item->prev = s;
		s->next = item;
		endItem = item;
	}
}

/** 削除 */
void TwoWayList::remove(TwoWayItem *item)
{
	if (item == NULL) {
		return;
	} else if (item->next == NULL) {
		if (item->prev != NULL) {
			endItem = item->prev;
			endItem->next = NULL;
		} else {
			firstItem = NULL;
			endItem = NULL;
		}
		item->prev = NULL;
		item->next = NULL;
		delete(item);
	} else {
		if (item->prev != NULL) {
			item->prev->next = item->next;
			item->next->prev = item->prev;
		} else {
			firstItem = item->next;
			firstItem->prev = NULL;
		}
		item->prev = NULL;
		item->next = NULL;
		delete(item);
	}
}

/** 全削除 */
void TwoWayList::removeAll()
{
	while (endItem != NULL) {
		remove(endItem);
	}
}

/** 選択（選択されたものが一番最後にくるように並び替えられる */
void TwoWayList::select(TwoWayItem *item)
{
	if (item == NULL) {
		return;
	} else if (item->next == NULL) {
		return;
	} else {
		if (item->prev != NULL) {
			item->prev->next = item->next;
			item->next->prev = item->prev;
		} else {
			firstItem = item->next;
			firstItem->prev = NULL;
		}
		item->prev = endItem;
		endItem->next = item;
		item->next = NULL;
		endItem = item;
	}
}
