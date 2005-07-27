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

#include <string.h>
#include "String.h"

namespace baygui {
	String& String::set(const char* str)
	{
		unsigned char c1, c2, c3;
		int slen;
		
		if (this->bytes != 0/* NULL */) {
			delete [] this->bytes;
			delete [] this->charArray;
		}
		if (str && *str) {
			this->len = 0;
			slen = strlen(str);
			this->bytes = new char[slen + 1];
			this->charArray = new unsigned int[slen + 1];
			strcpy(this->bytes, str);
			
			if (this->encoding == String::UTF_8) {
				// UTF-8 -> UCS-4
				for (int i = 0; i < slen; i++) {
					// 1st unsigned char
					if (str[i] == 0) {
						break;
					} else {
						c1 = (unsigned char)str[i];
					}
					// 0aaa bbbb - > 0aaa bbbb (0x20-0x7F)
					if (c1 <= 0x7F) {
						this->charArray[this->len++] = c1;
					// 110a aabb 10bb cccc -> 0000 0aaa bbbb cccc (0xC280-0xDFBF)
					} else if (0xC2 <= c1 && c1 <= 0xDF) {
						// 2nd unsigned char
						if (str[i] == slen - 1) {
							break;
						} else {
							c2 = (unsigned char)str[++i];
						}
						this->charArray[this->len++] = ((c1 & 0x1F) << 6) | (c2 & 0x3F);
					// 1110 aaaa 10bb bbcc 10cc dddd -> aaaa bbbb cccc dddd (0xE0A080-0xEFBFBF)
					} else if (0xE0 <= c1 && c1 <= 0xEF) {
						// 2nd unsigned char
						if (str[i] == slen - 1) {
							break;
						} else {
							c2 = (unsigned char)str[++i];
						}
						// 3rd unsigned char
						if (str[i] == slen - 1) {
							break;
						} else {
							c3 = (unsigned char)str[++i];
						}
						this->charArray[this->len++] = ((c1 & 0xF) << 12) | ((c2 & 0x3F) << 6) | (c3 & 0x3F);
					}
				}
			}
			#if SUPPORT_SJIS
			if (this->encoding == String::CP932) {
				// CP932 -> UCS-4
				for (int i = 0; i < slen; i++) {
					c1 = (unsigned char)str[i];
					// 1unsigned char character
					if (c1 < 0x80) {
						this->charArray[this->len++] = c1;
					} else if (c1 == 0x80 || c1 == 0xa0 || c1 >= 0xfd) {
						this->charArray[this->len++] = '?';
					} else if (0xa1 <= c1 && c1 <= 0xdf) {
						this->charArray[this->len++] = c1 + 0xfec0;
					// 2unsigned char character
					} else {
						c2 = (unsigned char)str[++i];
						if(c2 < 0x40 || c2 == 0x7f || c2 > 0xfc) {
							this->charArray[this->len++] = '?';
						}
						if (c1 >= 0xe0) c1 -= 0x40;
						if (c2 > 0x7e) c2--;
						this->charArray[this->len++] = table_cp932[(c1 - 0x81) * 188 + (c2 - 0x40)];
						if (this->charArray[this->len] == 0) this->charArray[this->len] = '?';
					}
				}
			}
			#endif // SUPPORT_SJIS
		} else {
			this->bytes = 0/* NULL */;
			this->charArray = 0/* NULL */;
			this->len = 0;
		}
		return *this;
	}
	
	String::String()
	{
		this->encoding = String::UTF_8;
		this->bytes = 0/* NULL */;
		this->len = 0;
	}
	
	String::String(const char* str)
	{
		this->encoding = String::UTF_8;
		this->bytes = 0/* NULL */;
		this->len = 0;
		set(str);
	}
	
	String::String(const String& str)
	{
		this->encoding = String::UTF_8;
		this->bytes = 0/* NULL */;
		this->len = 0;
		set((const char*)str);
	}
	
	String::String(const char* str, int encoding)
	{
		this->encoding = encoding;
		this->bytes = 0/* NULL */;
		this->charArray = 0/* NULL */;
		this->len = 0;
		set(str);
	}
	
	String::~String()
	{
		delete [] this->bytes;
		delete [] this->charArray;
	}
	
	int String::length()
	{
		return this->len;
	}
	
	char* String::getBytes()
	{
		return this->bytes;
	}
	
	unsigned int* String::toCharArray()
	{
		return this->charArray;
	}
	
	unsigned int String::charAt(int i)
	{
		return (i < this->len) ? this->charArray[i] : 0;
	}
	
	bool String::equals(Object* obj)
	{
		return equals((String *)obj);
	}
	
	bool String::equals(String* str)
	{
		return equals(str->getBytes());
	}
	
	bool String::equals(const char* str)
	{
		return (str && *str && strcmp(this->bytes, str) == 0) ? true : false;
	}
	
	bool String::startsWith(String* str)
	{
		return startsWith(str->getBytes());
	}
	
	bool String::startsWith(const char* str)
	{
		if (str == 0 || this->len == 0 || (int)strlen(str) > (int)strlen(this->bytes)) return false;
		for (int i = 0; i < (int)strlen(str); i++) {
			if (str[i] != this->bytes[i]) return false;
		}
		return true;
	}
	
	bool String::endsWith(String* str)
	{
		return endsWith(str->getBytes());
	}
	
	bool String::endsWith(const char* str)
	{
		if (str == 0 || this->len == 0 || (int)strlen(str) > (int)strlen(this->bytes)) return false;
		for (int i = 0; i < (int)strlen(str); i++) {
			if (str[(int)strlen(str) - i - 1] != 
				this->bytes[(int)strlen(this->bytes) - i - 1]) return false;
		}
		return true;
	}
}
