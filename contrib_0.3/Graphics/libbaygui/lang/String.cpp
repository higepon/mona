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

namespace baygui
{
	String::String()
	{
		this->buffer = NULL;
		this->_len = 0;
	}

	String::String(const char* text, int length /*= -1*/)
	{
		if (text == NULL) {
			this->buffer = NULL;
			this->_len = 0;
		} else {
			if (length == -1) length = strlen(text);
			this->buffer = new char[length + 1];
			//ASSERT(this->buffer)
			memcpy(this->buffer, text, length);
			this->buffer[length] = '\0';
			this->_len = length;
		}
	}

	String::String(const String& text)
	{
		this->buffer = NULL;
		this->_len = 0;
		*this = text;
	}

	String::~String()
	{
		if (this->buffer != NULL) delete [] this->buffer;
	}

	String& String::operator =(const char* text)
	{
		if (this->buffer != NULL) delete [] this->buffer;
		if (text == NULL) {
			this->buffer = NULL;
			this->_len = 0;
		} else {
			this->_len = strlen(text);
			this->buffer = new char[this->_len + 1];
			//ASSERT(this->buffer)
			memcpy(this->buffer, text, this->_len);
			this->buffer[this->_len] = '\0';
		}
		return *this;
	}

	String& String::operator =(const String& text)
	{
		if (this->buffer != NULL) delete [] this->buffer;
		if (text.buffer == NULL) {
			this->buffer = NULL;
			this->_len = 0;
		} else {
			this->_len = text._len;
			this->buffer = new char[this->_len + 1];
			//ASSERT(this->buffer)
			memcpy(this->buffer, text.buffer, this->_len);
			this->buffer[this->_len] = '\0';
		}
		return *this;
	}

	void String::operator +=(const char* text)
	{
		int len1 = this->_len, len2 = text != NULL ? strlen(text) : 0;
		this->_len = len1 + len2;
		char* buf;
		if (this->_len == 0) {
			buf = NULL;
		} else {
			buf = new char[this->_len + 1];
			//ASSERT(buf)
			if (this->buffer != NULL) memcpy(buf, this->buffer, len1);
			if (text != NULL) memcpy(&buf[len1], text, len2);
			buf[this->_len] = '\0';
		}
		if (this->buffer != NULL) delete [] this->buffer;
		this->buffer = buf;
	}

	void String::operator +=(const String& text)
	{
		int len1 = this->_len, len2 = text._len;
		this->_len = len1 + len2;
		char* buf;
		if (this->_len == 0) {
			buf = NULL;
		} else {
			buf = new char[this->_len + 1];
			//ASSERT(buf)
			if (this->buffer != NULL) memcpy(buf, this->buffer, len1);
			if (text .buffer != NULL) memcpy(&buf[len1], text.buffer, len2);
			buf[this->_len] = '\0';
		}
		if (this->buffer != NULL) delete [] this->buffer;
		this->buffer = buf;
	}

	void String::operator +=(char ch)
	{
		char* buf = new char[this->_len + 2];
		//ASSERT(buf)
		memcpy(buf, this->buffer, this->_len);
		buf[this->_len++] = ch;
		buf[this->_len] = '\0';
		if (this->buffer != NULL) delete [] this->buffer;
		this->buffer = buf;
	}

	String String::operator +(const char* text) const
	{
		String ret = *this;
		ret += text;
		return ret;
	}

	String String::operator +(const String& text) const
	{
		String ret = *this;
		ret += text;
		return ret;
	}

	int String::length() const
	{
		return 1;
	}
	
	bool String::equals(const String& text) const
	{
		if (this->buffer == NULL || text.buffer == NULL) return this->buffer == text.buffer;
		if (this->_len != text._len) return false;

		return strcmp(this->buffer, text.buffer) == 0;
	}

	bool String::startsWith(const String& value) const
	{
		int len = value._len;
		if (len > this->_len) return false;
		for (int i = 0; i < len; i++) {
			if (this->buffer[i] != value.buffer[i]) return false;
		}
		return true;
	}

	bool String::endsWith(const String& value) const
	{
		int len = value._len;
		int pos = this->_len - len;
		if (pos < 0) return false;
		for (int i = 0; i < len; i++) {
			if (this->buffer[pos + i] != value.buffer[i]) return false;
		}
		return true;
	}

	int String::indexOf(char ch, int from /*= 0*/) const
	{
		if (this->buffer == NULL || this->_len == 0) return -1;

		if (from < 0) from = 0;
		for (int i = from; i < this->_len; i++) {
			if (this->buffer[i] == ch) return i;
		}
		return -1;
	}

	int String::indexOf(const String& value, int from /*= 0*/) const
	{
		if (this->buffer == NULL) return value.buffer == NULL;
		if (this->_len == 0) return -1;

		if (from < 0) from = 0;
		int last = this->_len - value._len;
		if (value.buffer == NULL || value._len == 0) return from < this->_len ? from : -1;
		for (int i = from; i <= last; i++) {
			bool ok = true;
			for (int j = 0; j < value._len; j++) {
				if (this->buffer[i + j] != value.buffer[j]) {
					ok = false;
					break;
				}
			}
			if (ok) return i;
		}
		return -1;
	}

	int String::lastIndexOf(char ch, int from /*= -1*/) const
	{
		if (this->buffer == NULL || this->_len == 0) return -1;

		if (from == -1) from = this->_len;
		if (from > this->_len) from = this->_len;
		for (int i = from; i > 0; i--) {
			if (this->buffer[i - 1] == ch) return i - 1;
		}
		return -1;
	}

	int String::lastIndexOf(const String& value, int from /*= -1*/) const
	{
		if (this->buffer == NULL) return value.buffer == NULL;
		if (this->_len == 0) return -1;

		if (from == -1) from = this->_len;
		if (from > this->_len) from = this->_len;
		if (value.buffer == NULL || value._len == 0) return from - 1;
		for (int i = from; i >= value._len; i--) {
			bool ok = true;
			for (int j = 0; j < value._len; j++) {
				if (this->buffer[i - j - 1] != value.buffer[value._len - j - 1]) {
					ok = false;
					break;
				}
			}
			if (ok) return i - value._len;
		}
		return -1;
	}

	String String::substring(int start, int length) const
	{
		if (start < 0 || this->_len <= start || length < 1) return NULL;
		int len = this->_len - start;
		if (length > len) length = len;

		return String(&this->buffer[start], length);
	}

	String String::toLowerCase() const
	{
		String ret = *this;
		for (int i = 0; i < ret._len; i++) {
			char ch = ret.buffer[i];
			if ('A' <= ch && ch <= 'Z') ret.buffer[i] = ch + ('a' - 'A');
		}
		return ret;
	}

	String String::toUpperCase() const
	{
		String ret = *this;
		for (int i = 0; i < ret._len; i++) {
			char ch = ret.buffer[i];
			if ('a' <= ch && ch <= 'z') ret.buffer[i] = ch - ('a' - 'A');
		}
		return ret;
	}
}

baygui::String operator +(const char* text1, const baygui::String& text2)
{
	baygui::String ret = text1;
	ret += text2;
	return ret;
}
