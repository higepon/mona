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
	/** 
	 このクラスは、ハッシュテーブルを実装するためのもので、キーを値にマップします。
	 null オブジェクト以外であれば、どのオブジェクトでもキーや値に使用することができます。
	*/
	class Hashtable : public Object {
	private:
		/** キー一覧 */
		Vector keyList;
		/** 値一覧 */
		Vector valueList;
		
	public:
		/** デフォルトの容量で新しい空のハッシュテーブルを生成します。 */
		Hashtable();
		
		/** デストラクタ */
		virtual ~Hashtable();
		
		/** 値にマップされているキーが、ハッシュテーブルにあるかどうかを判定します。 */
		inline bool isEmpty() { return (this->keyList.size() == 0) ? true : false; }
		
		/** ハッシュテーブルにあるキーのリストを返します。 */
		inline Object** keys() { return this->keyList.elements(); }
		
		/** ハッシュテーブルにある値のリストを返します。 */
		inline Object** elements() { return this->valueList.elements(); }
		
		/**
		 指定されたキーにマップされている、ハッシュテーブルの値を返します。
		 @param key ハッシュテーブルのキー
		 @return 指定されたキーにマップされているハッシュテーブルの値。指定されたキーにマップされている値がない場合は null
		*/
		Object* get(Object* key);
		
		/**
		 ハッシュテーブルにおいて、指定された key を、指定された value にマップします。
		 @param key ハッシュテーブルのキー
		 @param value 値
		*/
		void put(Object* key, Object* value);

		/**
		 キー (およびそれに対応する値) をハッシュテーブルから削除します。
		 @param key 削除するキー
		*/
		void remove(Object* key);

		/** すべての要素を削除する */
		void clear();
		
		/** 要素数を得る */
		int size() { return this->keyList.size(); }
	};
}

#endif // _HASHTABLE_H_INCLUDED_
