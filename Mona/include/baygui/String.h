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

namespace baygui {
	/**
	 文字列ラッパークラス.
	 内部形式はUCS-4 (4バイト) で保持する。
	 */
	class String : public Object {
	private:
		/** 内部文字列 */
		char* bytes;
		/** ワイド文字列 (UCS-4) */
		wchar* wcharArray;
		/** ワイド文字列の長さ */
		int len;
		
	private:
		/** 文字列を設定する */
		String& set(const char *s);

		/** 数値を設定する */
		String& set(const int n);

	public:
		/** コンストラクタ */
		inline String() : bytes(0), len(0) {}
		
		/**
		 コンストラクタ
		 @param s 文字列 (UTF-8)
		*/
		inline String(const char *s) : bytes(0), len(0) { set(s); }
		
		/**
		 コンストラクタ
		 @param n 数値
		*/
		inline String(int n) : bytes(0), len(0) { set(n); }
		
		/** デストラクタ */
		inline ~String(){ delete[] bytes; delete[] wcharArray; }
		
		/** ワイド文字数を返す (wstrlen相当) */
		inline int length() const { return len; }
		
		/** 内部文字列 (char配列) を返す */
		inline char* getBytes() const { return bytes; }
		
		/** ワイド文字列を返す */
		inline wchar* toCharArray() const { return wcharArray; }
		
		/** i番目のワイド文字を得る */
		inline wchar charAt(int i) const { return wcharArray[i]; }
		
		/**
		 "="演算子の多重定義.
		 String s = "hoge"; のように使うことができる。
		*/
		inline const String& operator=(const char *s){ set(s); return *this; }
		
		/**
		 指定された文字列と等しいかどうかチェックする
		 */
		bool String::equals(Object* obj);
		
		/**
		 指定された文字列と等しいかどうかチェックする
		 */
		bool equals(const char *s);
		
		/**
		 指定された文字列で始まるかどうかチェックする
		 @param s 文字列
		 @return 始まっていればtrue、そうでなければfalse
		*/
		bool startsWith(const char *s);
		
		/**
		 指定された文字列で終っているかどうかチェックする
		 @param s 文字列
		 @return 終っていればtrue、そうでなければfalse
		*/
		bool endsWith(const char *s);
	};
}

#endif // _STRING_H_INCLUDED_
