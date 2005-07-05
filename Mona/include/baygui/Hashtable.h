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

/** キーと値のペアを格納できるクラス */
class Hashtable : public Object {
private:
	/** キー一覧 */
	Vector keyList;
	/** 値一覧 */
	Vector valueList;
	
public:
	/** デフォルトコンストラクタ */
	Hashtable()
	{
		//logprintf("%s:%s:%d\n", __FILE__, __FUNCTION__, __LINE__);
		//this->keyList = new Vector();
		//this->valueList = new Vector();
	}
	
	/** デストラクタ */
	virtual ~Hashtable()
	{
		//logprintf("%s:%s:%d\n", __FILE__, __FUNCTION__, __LINE__);
	}
	
	/** ハッシュテーブルが空かどうかを得る */
	bool isEmpty()
	{
		//logprintf("%s:%s:%d\n", __FILE__, __FUNCTION__, __LINE__);
		if (this->keyList.size() == 0) {
			return true;
		} else {
			return false;
		}
	}
	
	/** キー一覧を得る */
	Object** keys()
	{
		//logprintf("%s:%s:%d\n", __FILE__, __FUNCTION__, __LINE__);
		return this->keyList.elements();
	}
	
	/** 値一覧を得る */
	Object** elements()
	{
		//logprintf("%s:%s:%d\n", __FILE__, __FUNCTION__, __LINE__);
		return this->valueList.elements();
	}
	
	/**
	 指定したキーに対応する値を得る
	 @param key キー
	 @return 対応する値がないときはNULL
	*/
	Object* get(Object* key)
	{
		//logprintf("%s:%s:%d\n", __FILE__, __FUNCTION__, __LINE__);
		if (key == NULL) return NULL;
		return this->valueList.get(this->keyList.indexOf(key));
	}
	
	/**
	 キーと値の対を追加する
	 @param key キー
	 @param value 値
	*/
	void put(Object* key, Object* value)
	{
		//logprintf("%s:%s:%d\n", __FILE__, __FUNCTION__, __LINE__);
		if (key == NULL || value == NULL) return;
		int index = this->keyList.indexOf(key);
		if (index >= 0) {
			this->keyList.remove(index);
			this->valueList.remove(index);
		}
		this->keyList.add(key);
		this->valueList.add(value);
	}

	/** 指定されたキーを削除する */
	void remove(Object* key)
	{
		//logprintf("%s:%s:%d\n", __FILE__, __FUNCTION__, __LINE__);
		if (key == NULL) return;
		int index = this->keyList.indexOf(key);
		this->keyList.remove(index);
		this->valueList.remove(index);
	}

	/** すべての要素を削除する */
	void clear()
	{
		//logprintf("%s:%s:%d\n", __FILE__, __FUNCTION__, __LINE__);
		this->keyList.removeAll();
		this->valueList.removeAll();
	}
	
	/** 要素数を得る */
	int size()
	{
		//logprintf("%s:%s:%d\n", __FILE__, __FUNCTION__, __LINE__);
		return this->keyList.size();
	}
};

#endif // _HASHTABLE_H_INCLUDED_
