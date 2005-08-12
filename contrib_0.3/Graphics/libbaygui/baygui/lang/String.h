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

#include "Object.h"

namespace baygui {
	/** 文字列クラス */
	class String : public Object {
	protected:
		/** 内部文字列 */
		char* buffer;
		/** 内部文字列長 */
		int _len;

	public:
		/** デフォルトコンストラクタ */
		String();
		
		/**
		 コピーコンストラクタ
		 @param text 文字列
		 @param length 文字列長（初期値＝与えられた文字列の長さ）
		*/
		String(const char* text, int length = -1);
		
		/**
		 コピーコンストラクタ
		 @param text 文字列
		*/
		String(const String& text);
		
		/** デストラクタ */
		virtual ~String();
		
		/** 'const char*'演算子 */
		operator const char*() const { return this->buffer; }
		
		/** '='演算子 */
		String& operator =(const char* text);
		
		/** '='演算子 */
		String& operator =(const String& text);
		
		/** '+='演算子 */
		void operator +=(const char* text);
		
		/** '+='演算子 */
		void operator +=(const String& text);
		
		/** '+='演算子 */
		void operator +=(char ch);
		
		/** '+'演算子 */
		String operator +(const char* text) const;
		
		/** '+'演算子 */
		String operator +(const String& text) const;
		
		/** 内部文字列を得る */
		char* getBytes() const { return this->buffer; }
		
		/** ワイド文字列の長さを得る */
		int length() const;
		
		/** 指定された順番のワイド文字を得る */
		unsigned int charAt(int index) const;
		
		/** ワイド文字列を得る */
		const unsigned int* toCharArray() const;
		
		/**
		 指定されたオブジェクトと等しいかどうかを得る
		 @param obj 比較対象のオブジェクト
		*/
		virtual bool equals(Object* obj) const { return equals((String *)obj); }
		
		/**
		 指定された文字列と等しいかどうかを得る
		 @param value 文字列
		*/
		bool equals(const String& value) const;
		
		/**
		 指定された文字列で始まっているかどうかを得る
		 @param value 文字列
		*/
		bool startsWith(const String& value) const;
		
		/**
		 指定された文字列で終わっているかどうかを得る
		 @param value 文字列
		*/
		bool endsWith(const String& value) const;
		
		/**
		 指定された文字が出現する先頭からの位置を得る
		 @param ch 文字
		*/
		int indexOf(char ch, int from = 0) const;
		
		/**
		 指定された文字列が出現する先頭からの位置を得る
		 @param value 文字列
		*/
		int indexOf(const String& value, int from = 0) const;
		
		/**
		 指定された文字列が出現する終端からの位置を得る
		 @param value 文字列
		*/
		int lastIndexOf(char ch, int from = -1) const;
		
		/**
		 指定された文字列が出現する終端からの位置を得る
		 @param value 文字列
		*/
		int lastIndexOf(const String& value, int from = -1) const;
		
		/**
		 部分文字列を得る
		 @param start 開始位置
		 @param length 長さ
		*/
		String substring(int start, int length) const;
		
		/** すべての文字を小文字に変換する */
		String toLowerCase() const;
		
		/** すべての文字を大文字に変換する */
		String toUpperCase() const;
	};
}

extern baygui::String operator +(const char* text1, const baygui::String& text2);

#endif  // _STRING_H_INCLUDED_
