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
 双方向アイテム構造体（双方向リストで使用）
*/
struct LinkedItem {
	/** 内部データ */
	Object *data;
	/** 次のアイテムへのポインター */
	LinkedItem *next;
	/** 前のアイテムへのポインター */
	LinkedItem *prev;
};

/**
 双方向リスト
*/
class LinkedList : public Object {
private:
	/** リストの長さ */
	int dataListLength;
	/** 最初 */
	LinkedItem *firstItem;
	/** 終わり */
	LinkedItem *endItem;
	/** remove時にdeleteを行うかどうか */
	bool autoDelete;
	
private:
	/** ObjectをLinkedItemに変換する */
	LinkedItem *getLinkedItem(Object *o);
	
	/** 指定した順番の項目を得る */
	LinkedItem *getLinkedItem(int index);
	
public:
	/** デフォルトコンストラクタ */
	LinkedList();
	
	/**
	 コンストラクタ.
	 デフォルトで項目はremove時にdeleteされる。
	 deleteされることを望まない場合はautoDeleteにfalseを設定する。
	 @param autoDelete remove時にdeleteを行うかどうか（true / false）
	*/
	LinkedList(bool autoDelete);
	
	/** デストラクタ */
	virtual ~LinkedList();
	
	/** リストの長さを得る */
	inline int getLength() { return this->dataListLength; }
	
	/** 指定した順番の項目を得る */
	Object *get(int index);
	
	/** 一番最初の項目を得る */
	Object *getFirst();
	
	/** 一番最後の項目を得る */
	Object *getLast();
	
	/** 指定した項目を追加する */
	void add(Object *obj);
	
	/** 指定した順番の項目を削除する */
	Object *remove(int index);
	
	/** 指定した項目を削除する */
	Object *remove(Object *o);
	
	/** 全て削除する */
	void removeAll();
};

#endif // _LINKEDLIST_H_INCLUDED_
