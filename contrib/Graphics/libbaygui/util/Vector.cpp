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

#include "baygui.h"

namespace baygui {
	void Vector::initialize()
	{
		this->elementList = new Object*[defaultCapacity];
		this->_size = 0;
		this->_capacity = defaultCapacity;
	}

	Vector::Vector()
	{
		this->initialize();
	}

	Vector::~Vector()
	{
		delete [] this->elementList;
	}

	void Vector::add(Object* item)
	{
		if (this->_size == this->_capacity) {
			/* ‘«‚è‚È‚­‚È‚Á‚½‚ç—e—Ê‚ð”{‚É‘‚â‚· */
			this->_capacity <<= 1;
			Object** ptr = new Object*[this->_capacity];
			for (int i = 0; i < this->_size; i++) ptr[i] = this->elementList[i];
			delete [] this->elementList;
			this->elementList = ptr;
		}
		this->elementList[this->_size] = item;
		this->_size++;
	}

	int Vector::indexOf(Object* item)
	{
		for (int i = 0; i < this->_size; i++) {
			if (this->elementList[i]->equals(item)) return i;
		}
		return -1;
	}

	Object* Vector::remove(int index)
	{
		if (index < 0) return NULL;
		this->_size--;
		Object* obj = this->elementList[index];
		for (int i = index; i < this->_size; i++) {
			this->elementList[i] = this->elementList[i + 1];
		}
		return obj;
		//this->elementList[this->_size] = new Object();
	}

	void Vector::remove(Object* item)
	{
		int index = this->indexOf(item);
		if (index < 0) return;
		this->remove(index);
	}

	void Vector::removeAll()
	{
		delete [] this->elementList;
		this->initialize();
	}
}
