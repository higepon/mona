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

#if !defined(_LINKEDLIST_H_INCLUDED_)
#define _LINKEDLIST_H_INCLUDED_

/**
 双方向リスト
*/
class LinkedList : public Object {
protected:
	/** リストの長さ */
	int dataListLength;
	
	virtual LinkedItem *_get(int index);
	virtual LinkedItem *getLinkedItem(Object *o);
	virtual void _add(LinkedItem *item);
	virtual void _remove(LinkedItem *item);
	virtual void _sort(LinkedItem *item);
	
public:
	/** 最初 */
	LinkedItem *firstItem;
	/** 終わり */
	LinkedItem *endItem;
	
	LinkedList::LinkedList();
	virtual LinkedList::~LinkedList();
	virtual const char *className() {return "baygui.LinkedList";}
	virtual int getLength();
	virtual Object *get(int index);
	virtual Object *getFirst();
	virtual Object *getLast();
	virtual void add(Object *o);
	virtual void remove(int index);
	virtual void remove(Object *o);
	virtual void removeAll();
	virtual void sort(Object *o);
};

#endif // _LINKEDLIST_H_INCLUDED_
