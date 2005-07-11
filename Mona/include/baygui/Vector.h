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

#if !defined(_VECTOR_H_INCLUDED_)
#define _VECTOR_H_INCLUDED_

namespace baygui {
	/** オブジェクトの可変長配列を格納できるクラス */
	class Vector : public Object {
	protected:
		/** デフォルト容量 */
		static const int defaultCapacity = 5;
		/** 要素一覧 */
		Object** elementList;
		/** 要素数 */
		int _size;
		/** 現在の容量 */
		int _capacity;

	private:
		/** 初期化 */
		void initialize();

	public:
		/** コンストラクタ */
		Vector();
		
		/** デストラクタ */
		virtual ~Vector();
		
		/** 要素を追加する */
		void add(Object* item);
		
		/**
		 要素がはじめて出現する順番を得る
		 @return 一度も出現しなかった場合は-1
		*/
		int indexOf(Object* item);
		
		/** 指定した順番の要素を削除する */
		Object* remove(int index);
		
		/** 指定した要素を削除する */
		void remove(Object* item);
		
		/** すべての要素を削除する */
		void removeAll();
		
		/** 要素一覧を得る */
		inline Object** elements() { return this->elementList; }
		
		/** 指定した順番の要素を得る */
		inline Object* get(int index) { return (index < 0) ? NULL : this->elementList[index]; }
		
		/** 要素数を得る */
		inline int size() { return this->_size; }
		
		/** 現在の容量を得る */
		inline int capacity() { return this->_capacity; }
	};
}

#endif // _VECTOR_H_INCLUDED_
