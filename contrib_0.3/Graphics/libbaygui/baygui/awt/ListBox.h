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

#if !defined(_LISTBOX_H_INCLUDED_)
#define _LISTBOX_H_INCLUDED_

namespace baygui {
	/**
	 リストボックスクラス
	*/
	class ListBox : public Component {
	private:
		/** 選択位置 */
		int selectedIndex;
		/** 表示開始位置 */
		int offsetIndex;
		/** 表示可能行数 */
		int visibleRows;
		/** データリスト */
		Vector dataList;
		/** 選択イベント */
		Event itemEvent;
		/** ItemListener一覧 */
		Vector itemListenerList;

	public:
		/** コンストラクタ */
		ListBox();
		
		/** デストラクタ */
		virtual ~ListBox();
		
		/** 選択位置を得る */
		inline  int getSelectedIndex() { return this->selectedIndex; }
		
		/** 選択項目を得る */
		virtual char* getSelectedItem();
		
		/** index 番目を選択する */
		virtual void select(int index);
		
		/** 項目を追加する */
		virtual void add(const String& item);
		
		/** index 番目の項目を削除する */
		virtual void remove(int index);
		
		/** 描画ハンドラ */
		virtual void paint(Graphics* g);
		
		/** イベントハンドラ */
		virtual void processEvent(Event* event);
		
		/** ItemListenerを追加 */
		virtual void addItemListener(ItemListener* l);
		
		/** ItemListenerを追加 */
		virtual void removeItemListener(ItemListener* l);
		
		/** ItemListenerを追加 */
		virtual void processItemEvent(ItemEvent* e);
	};
}

#endif // _LISTBOX_H_INCLUDED_
