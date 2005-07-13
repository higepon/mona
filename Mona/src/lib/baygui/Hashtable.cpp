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
	Hashtable::Hashtable()
	{
	}

	Hashtable::~Hashtable()
	{
	}

	Object* Hashtable::get(Object* key)
	{
		return (key == NULL) ? NULL : this->valueList.get(this->keyList.indexOf(key));
	}

	void Hashtable::put(Object* key, Object* value)
	{
		if (key == NULL || value == NULL) return;
		int index = this->keyList.indexOf(key);
		if (index >= 0) {
			this->keyList.remove(index);
			this->valueList.remove(index);
		}
		this->keyList.add(key);
		this->valueList.add(value);
	}

	void Hashtable::remove(Object* key)
	{
		if (key == NULL) return;
		int index = this->keyList.indexOf(key);
		this->keyList.remove(index);
		this->valueList.remove(index);
	}

	void Hashtable::clear()
	{
		this->keyList.removeAll();
		this->valueList.removeAll();
	}
}
