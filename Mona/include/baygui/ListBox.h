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

#if !defined(_LISTBOX_H_INCLUDED_)
#define _LISTBOX_H_INCLUDED_

/**
 リストボックスクラス
*/
class ListBox : public Control {
private:
	/** 選択位置 */
	int selectedIndex;
	/** データリスト */
	LinkedList *dataList;
	/** 選択イベント */
	Event itemEvent;

public:
	/** コンストラクタ */
	ListBox::ListBox();
	
	/** デストラクタ */
	virtual ListBox::~ListBox();
	
	/** 選択位置を得る */
	inline  int getSelectedIndex() { return this->selectedIndex; }
	
	/** 選択項目を得る */
	virtual char *getSelectedItem();
	
	/** index 番目を選択する */
	virtual void select(int index);
	
	/** 項目を追加する */
	virtual void add(char *item);
	
	/** index 番目の項目を削除する */
	virtual void remove(int index);
	
	/** 描画ハンドラ */
	virtual void onPaint(Graphics *g);
	
	/** イベントハンドラ */
	virtual void onEvent(Event *event);
};

#endif // _LISTBOX_H_INCLUDED_
