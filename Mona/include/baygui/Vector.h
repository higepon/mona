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
	void initialize()
	{
		//logprintf("%s:%s:%d\n", __FILE__, __FUNCTION__, __LINE__);
		this->elementList = new Object*[defaultCapacity];
		this->_size = 0;
		this->_capacity = defaultCapacity;
	}

public:
	/** コンストラクタ */
	Vector()
	{
		//logprintf("%s:%s:%d\n", __FILE__, __FUNCTION__, __LINE__);
		this->initialize();
	}
	
	/** デストラクタ */
	virtual ~Vector()
	{
		//logprintf("%s:%s:%d\n", __FILE__, __FUNCTION__, __LINE__);
		delete [] this->elementList;
	}
	
	/** 要素を追加する */
	void add(Object* item)
	{
		//logprintf("%s:%s:%d\n", __FILE__, __FUNCTION__, __LINE__);
		if (this->_size == this->_capacity) {
			// 足りなくなったら容量を倍に増やす
			this->_capacity <<= 1;
			Object** ptr = new Object*[this->_capacity];
			for (int i = 0; i < this->_size; i++) ptr[i] = this->elementList[i];
			delete [] this->elementList;
			this->elementList = ptr;
		}
		this->elementList[this->_size] = item;
		this->_size++;
	}
	
	/**
	 要素がはじめて出現する順番を得る
	 @return 一度も出現しなかった場合は-1
	*/
	int indexOf(Object* item)
	{
		//logprintf("%s:%s:%d\n", __FILE__, __FUNCTION__, __LINE__);
		for (int i = 0; i < this->_size; i++) {
			if (this->elementList[i]->equals(item)) return i;
		}
		return -1;
	}
	
	/** 指定した順番の要素を削除する */
	Object* remove(int index)
	{
		//logprintf("%s:%s:%d\n", __FILE__, __FUNCTION__, __LINE__);
		if (index < 0) return NULL;
		this->_size--;
		Object* obj = this->elementList[index];
		for (int i = index; i < this->_size; i++) {
			this->elementList[i] = this->elementList[i + 1];
		}
		return obj;
		//this->elementList[this->_size] = new Object();
	}
	
	/** 指定した要素を削除する */
	void remove(Object* item)
	{
		//logprintf("%s:%s:%d\n", __FILE__, __FUNCTION__, __LINE__);
		int index = this->indexOf(item);
		if (index < 0) return;
		this->remove(index);
	}
	
	/** すべての要素を削除する */
	void removeAll()
	{
		//logprintf("%s:%s:%d\n", __FILE__, __FUNCTION__, __LINE__);
		delete [] this->elementList;
		this->initialize();
	}
	
	/** 要素一覧を得る */
	Object** elements()
	{
		//logprintf("%s:%s:%d\n", __FILE__, __FUNCTION__, __LINE__);
		return this->elementList;
	}
	
	/** 指定した順番の要素を得る */
	Object* get(int index)
	{
		//logprintf("%s:%s:%d\n", __FILE__, __FUNCTION__, __LINE__);
		if (index < 0) return NULL;
		return this->elementList[index];
	}
	
	/** 要素数を得る */
	int size()
	{
		//logprintf("%s:%s:%d\n", __FILE__, __FUNCTION__, __LINE__);
		return this->_size;
	}
	
	/** 現在の容量を得る */
	int capacity()
	{
		//logprintf("%s:%s:%d\n", __FILE__, __FUNCTION__, __LINE__);
		return this->_capacity;
	}
};

#endif // _VECTOR_H_INCLUDED_
