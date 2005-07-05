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

typedef unsigned int wchar;

/**
 文字列ラッパークラス.
 内部形式はUCS-4 (4バイト) で保持する。
 */
class String : public Object
{
private:
	/** 内部文字列 */
	char* charArray;
	/** ワイド文字列 (UCS-4) */
	wchar* wcharArray;
	/** ワイド文字列の長さ */
	int wlen;
	
private:
	/** 文字列を設定する */
	String& set(const char* s);

	/** 数値を設定する */
	String& set(const int n);

public:
	/** コンストラクタ */
	inline String() : charArray(0), wlen(0) {}
	
	/**
	 コンストラクタ
	 @param s 文字列 (UTF-8)
	*/
	inline String(const char* s) : charArray(0), wlen(0) { set(s); }
	
	/**
	 コンストラクタ
	 @param n 数値
	*/
	inline String(int n) : charArray(0), wlen(0) { set(n); }
	
	/** デストラクタ */
	inline ~String(){ delete[] charArray; delete[] wcharArray; }
	
	/** ワイド文字数を返す (wstrlen相当) */
	inline int length() const { return wlen; }
	
	/** 内部文字列 (char配列) を返す */
	inline char* getBytes() const { return charArray; }
	
	/** ワイド文字列を返す */
	inline wchar* toCharArray() const { return wcharArray; }
	
	/** i番目のワイド文字を得る */
	inline wchar operator[](int i) const { return wcharArray[i]; }
	
	/**
	 "="演算子の多重定義.
	 String s = "hoge"; のように使うことができる。
	*/
	inline const String& operator=(const char* s){ set(s); return *this; }
	
	/**
	 指定された文字列と等しいかどうかチェックする
	 */
	inline bool equals(const char *s)
	{
		if (s == NULL) return false;
		if (strcmp(charArray, s) == 0) {
			return true;
		} else {
			return false;
		}
	}
	
	/**
	 指定された文字列で始まるかどうかチェックする
	 @param s 文字列
	 @return 始まっていればtrue、そうでなければfalse
	*/
	inline bool startsWith(const char* s)
	{
		if (s == 0 || wlen == 0 || (int)strlen(s) > (int)strlen(charArray)) return false;
		for (int i = 0; i < (int)strlen(s); i++) {
			if (s[i] != charArray[i]) return false;
		}
		return true;
	}
	
	/**
	 指定された文字列で終っているかどうかチェックする
	 @param s 文字列
	 @return 終っていればtrue、そうでなければfalse
	*/
	inline bool endsWith(const char*s)
	{
		if (s == 0 || wlen == 0 || (int)strlen(s) > (int)strlen(charArray)) return false;
		for (int i = 0; i < (int)strlen(s); i++) {
			if (s[(int)strlen(s) - i - 1] != 
				charArray[(int)strlen(charArray) - i - 1]) return false;
		}
		return true;
	}
};

#endif // _STRING_H_INCLUDED_
