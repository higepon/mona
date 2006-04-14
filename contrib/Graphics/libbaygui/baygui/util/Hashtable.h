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

#if !defined(_HASHTABLE_H_INCLUDED_)
#define _HASHTABLE_H_INCLUDED_

namespace baygui {
	/** キーと値のペアを格納できるクラス */
	class Hashtable : public Object {
	private:
		/** キー一覧 */
		Vector keyList;
		/** 値一覧 */
		Vector valueList;
		
	public:
		/** デフォルトコンストラクタ */
		Hashtable();
		
		/** デストラクタ */
		virtual ~Hashtable();
		
		/** ハッシュテーブルが空かどうかを得る */
		inline bool isEmpty() { return (this->keyList.size() == 0) ? true : false; }
		
		/** キー一覧を得る */
		inline Object** keys() { return this->keyList.elements(); }
		
		/** 値一覧を得る */
		inline Object** elements() { return this->valueList.elements(); }
		
		/**
		 指定したキーに対応する値を得る
		 @param key キー
		 @return 対応する値がないときはNULL
		*/
		Object* get(Object* key);
		
		/**
		 キーと値の対を追加する
		 @param key キー
		 @param value 値
		*/
		void put(Object* key, Object* value);

		/** 指定されたキーを削除する */
		void remove(Object* key);

		/** すべての要素を削除する */
		void clear();
		
		/** 要素数を得る */
		int size() { return this->keyList.size(); }
	};
}

#endif // _HASHTABLE_H_INCLUDED_
