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
