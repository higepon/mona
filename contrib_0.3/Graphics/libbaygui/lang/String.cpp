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
	/**
	 文字列を設定する
	 @param s 文字列
	*/
	String& String::set(const char* str)
	{
		byte c1, c2, c3;
		int slen;
		
		if (this->bytes != NULL) {
			delete[] this->bytes;
			delete[] this->charArray;
		}
		if (str && *str) {
			this->len = 0;
			slen = strlen(str);
			this->bytes = new char[slen + 1];
			this->charArray = new wchar[slen + 1];
			strcpy(this->bytes, str);
			
#if SUPPORT_SJIS
			if (this->encoding == String::CP932) {
				// CP932 -> UCS-4
				for (int i = 0; i < slen; i++) {
					c1 = (byte)str[i];
					// 1byte character
					if (c1 < 0x80) {
						this->charArray[this->len++] = c1;
					} else if (c1 == 0x80 || c1 == 0xa0 || c1 >= 0xfd) {
						this->charArray[this->len++] = '?';
					} else if (0xa1 <= c1 && c1 <= 0xdf) {
						this->charArray[this->len++] = c1 + 0xfec0;
					// 2byte character
					} else {
						c2 = (byte)str[++i];
						if(c2 < 0x40 || c2 == 0x7f || c2 > 0xfc) {
							this->charArray[this->len++] = '?';
						}
						if (c1 >= 0xe0) c1 -= 0x40;
						if (c2 > 0x7e) c2--;
						this->charArray[this->len++] = table_cp932[(c1 - 0x81) * 188 + (c2 - 0x40)];
						if (this->charArray[this->len] == 0) this->charArray[this->len] = '?';
					}
				}
			} else {
#endif
				// UTF-8 -> UCS-4
				for (int i = 0; i < slen; i++) {
					// 1st byte
					if (str[i] == 0) {
						break;
					} else {
						c1 = (byte)str[i];
					}
					// 0aaa bbbb - > 0aaa bbbb (0x20-0x7F)
					if (c1 <= 0x7F) {
						this->charArray[this->len++] = c1;
					// 110a aabb 10bb cccc -> 0000 0aaa bbbb cccc (0xC280-0xDFBF)
					} else if (0xC2 <= c1 && c1 <= 0xDF) {
						// 2nd byte
						if (str[i] == slen - 1) {
							break;
						} else {
							c2 = (byte)str[++i];
						}
						this->charArray[this->len++] = ((c1 & 0x1F) << 6) | (c2 & 0x3F);
					// 1110 aaaa 10bb bbcc 10cc dddd -> aaaa bbbb cccc dddd (0xE0A080-0xEFBFBF)
					} else if (0xE0 <= c1 && c1 <= 0xEF) {
						// 2nd byte
						if (str[i] == slen - 1) {
							break;
						} else {
							c2 = (byte)str[++i];
						}
						// 3rd byte
						if (str[i] == slen - 1) {
							break;
						} else {
							c3 = (byte)str[++i];
						}
						this->charArray[this->len++] = ((c1 & 0xF) << 12) | ((c2 & 0x3F) << 6) | (c3 & 0x3F);
					}
				}
#if SUPPORT_SJIS
			}
#endif
		} else {
			this->bytes  = NULL;
			this->charArray = NULL;
			this->len = 0;
		}
		return *this;
	}
	
	/** デフォルトコンストラクタ */
	String::String()
	{
#if SUPPORT_SJIS
		this->encoding = UTF_8;
#endif
		this->bytes = NULL;
		this->len = 0;
	}
	
	/**
	 コピーコンストラクタ
	 @param str 文字列
	*/
	String::String(const char *str)
	{
#if SUPPORT_SJIS
		this->encoding = UTF_8;
#endif
		this->bytes = NULL;
		this->len = 0;
		set(str);
	}
	
#if SUPPORT_SJIS
	/**
	 コピーコンストラクタ
	 @param str 文字列
	 @param encoding エンコーディング
	*/
	String::String(const char *str, int encoding)
	{
		this->encoding = encoding;
			this->bytes = NULL;
			this->charArray = NULL;
			this->len = 0;
		set(str);
	}
#endif
	
	/** デストラクタ */
	String::~String()
	{
		delete[] this->bytes;
		delete[] this->charArray;
	}
	
	/** ワイド文字数を返す (wstrlen相当) */
	int String::length()
	{
		return this->len;
	}
	
	/** 内部文字列 (byte配列) を返す */
	char* String::getBytes()
	{
		return this->bytes;
	}
	
	/** ワイド文字列を返す */
	wchar* String::toCharArray()
	{
		return this->charArray;
	}
	
	/** i番目のワイド文字を得る */
	wchar String::charAt(int i)
	{
		return (i < this->len) ? this->charArray[i] : 0;
	}
	
	/**
	 指定されたオブジェクトと等しいかどうかを得る
	 @param obj 比較対象のオブジェクト
	 */
	bool String::equals(Object* obj)
	{
		return equals(((String *)obj)->getBytes());
	}
	
	/**
	 指定された文字列と等しいかどうかチェックする
	 @param str 文字列
	 */
	bool String::equals(const char* str)
	{
		return (str && *str && strcmp(this->bytes, str) == 0) ? true : false;
	}
	
	/**
	 指定された文字列で始まるかどうかチェックする
	 @param str 文字列
	 @return 始まっていればtrue、そうでなければfalse
	*/
	bool String::startsWith(const char* str)
	{
		if (str == 0 || this->len == 0 || (int)strlen(str) > (int)strlen(this->bytes)) return false;
		for (int i = 0; i < (int)strlen(str); i++) {
			if (str[i] != this->bytes[i]) return false;
		}
		return true;
	}
	
	/**
	 指定された文字列で終っているかどうかチェックする
	 @param str 文字列
	 @return 終っていればtrue、そうでなければfalse
	*/
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
