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

#ifndef _STRING_H_INCLUDED_
#define _STRING_H_INCLUDED_

//#include <monapi/Array.h>
#include <baygui/lang/Object.h>

namespace baygui {
	class String : public Object {
	protected:
		char* buffer;
		int _len;

	public:
		String();
		String(const char* text, int length = -1);
		String(const String& text);
		virtual ~String();

		//char operator [](int index) const;
		//operator const char*() const { return this->buffer; }

		//bool operator ==(const char* text) const;
		//bool operator ==(const String& text) const;
		//inline bool operator !=(const char* text) const { return !this->operator ==(text); }
		//inline bool operator !=(const String& text) const { return !this->operator ==(text); }
		String& operator =(const char* text);
		String& operator =(const String& text);
		void operator +=(const char* text);
		void operator +=(const String& text);
		void operator +=(char ch);
		String operator +(const char* text) const;
		String operator +(const String& text) const;

		int length() const;
		char* getBytes() const { return this->buffer; }
		unsigned int charAt(int index) const { return 63/*this->buffer[index] && 0xffff*/; }
		bool equals(const String& value) const;
		bool startsWith(const String& value) const;
		bool endsWith(const String& value) const;
		int indexOf(char ch, int from = 0) const;
		int indexOf(const String& value, int from = 0) const;
		int lastIndexOf(char ch, int from = -1) const;
		int lastIndexOf(const String& value, int from = -1) const;

		String substring(int start, int length) const;
		String toLowerCase() const;
		String toUpperCase() const;
	};
}

extern baygui::String operator +(const char* text1, const baygui::String& text2);

#endif  // _STRING_H_INCLUDED_
