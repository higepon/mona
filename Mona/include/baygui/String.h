/*
Copyright (c) 2004 Tino, bayside
All rights reserved.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef __BAYGUI_STRING_H__
#define __BAYGUI_STRING_H__

typedef unsigned int wchar;

namespace baygui {
	/**
	 文字列ラッパークラス.
	 内部形式はUCS4 (4バイト) で保持する。
	 */
	class String : public Object
	{
	private:
		/** 内部文字列 */
		char* charArray;
		/** ワイド文字列 (UCS-4) */
		wchar* wcharArray;
		/** ワイド文字列の長さ */
		int len;
		
	public:
		/** コンストラクタ */
		inline String() : charArray(0), len(0) {}
		
		/**
		 コンストラクタ
		 @param s 文字列 (UTF-8)
		*/
		inline String(const char* s) : charArray(0), len(0) { set(s); }
		
		/** デストラクタ */
		inline ~String(){ delete[] charArray; delete[] wcharArray; }
		
		/** ワイド文字数を返す (wstrlen相当) */
		inline int length() const { return len; }
		
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
		 指定された文字列で始まるかどうかチェックする
		 @param s 文字列
		 @return 始まっていればtrue、そうでなければfalse
		*/
		inline bool startWith(const char* s)
		{
			if (s == 0 || len == 0 || (int)strlen(s) > (int)strlen(charArray)) return false;
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
			if (s == 0 || len == 0 || (int)strlen(s) > (int)strlen(charArray)) return false;
			for (int i = (int)strlen(s) - 1; i >= 0; i--) {
				if (s[i] != charArray[i]) return false;
			}
			return true;
		}
		
		/** 文字列を設定する */
		String& set(const char* s);
	};
}

#endif // __BAYGUI_STRING_H__
