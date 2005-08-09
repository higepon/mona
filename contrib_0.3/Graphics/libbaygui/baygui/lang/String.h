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

#if !defined(_STRING_H_INCLUDED_)
#define _STRING_H_INCLUDED_

#if SUPPORT_SJIS
#include "../../../tools/code_table/cp932.h"
#endif

namespace baygui {
	/** 文字列クラス */
	class String : public Object {
#if SUPPORT_SJIS
	public:
		static const int UTF_8 = 0;
		static const int CP932 = 1;
#endif
		
	private:
#if SUPPORT_SJIS
		/** エンコーディング */
		int encoding;
#endif
		/** 内部文字列 */
		char* bytes;
		/** ワイド文字列 (UCS-4) */
		wchar* charArray;
		/** ワイド文字列の長さ */
		int len;
	
	private:
		/**
		 文字列を設定する
		 @param s 文字列
		*/
		String& set(const char* str);
		
	public:
		/** デフォルトコンストラクタ */
		String();
		
		/**
		 コピーコンストラクタ
		 @param str 文字列
		*/
		String(const char *str);
		
#if SUPPORT_SJIS
		/**
		 コピーコンストラクタ
		 @param str 文字列
		 @param encoding エンコーディング
		*/
		String(const char *str, int encoding);
#endif
		
		/** デストラクタ */
		~String();
		
		/** ワイド文字数を返す (wstrlen相当) */
		int length();
		
		/** 内部文字列 (byte配列) を返す */
		char* getBytes();
		
		/** ワイド文字列を返す */
		wchar* toCharArray();
		
		/** i番目のワイド文字を得る */
		wchar charAt(int i);
		
		/**
		 "="演算子の多重定義。<br>
		 String str = "hoge"; のように使うことができる。
		*/
		const String& operator=(const char* str)
		{
			set(str);
			return *this;
		}
		
		/**
		 指定されたオブジェクトと等しいかどうかを得る
		 @param obj 比較対象のオブジェクト
		 */
		bool equals(Object* obj);
		
		/**
		 指定された文字列と等しいかどうかチェックする
		 @param str 文字列
		 */
		bool equals(const char* str);
		
		/**
		 指定された文字列で始まるかどうかチェックする
		 @param str 文字列
		 @return 始まっていればtrue、そうでなければfalse
		*/
		bool startsWith(const char* str);
		
		/**
		 指定された文字列で終っているかどうかチェックする
		 @param str 文字列
		 @return 終っていればtrue、そうでなければfalse
		*/
		bool endsWith(const char* str);
	};
}

#endif // _STRING_H_INCLUDED_
