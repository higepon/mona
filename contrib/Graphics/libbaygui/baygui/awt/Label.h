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

#if !defined(_LABEL_H_INCLUDED_)
#define _LABEL_H_INCLUDED_

namespace baygui {
	/**
	 ラベルクラス
	*/
	class Label : public Component {
	public:
		/** 左寄せ */
		static const int LEFT   = 1;
		/** 中央寄せ */
		static const int CENTER = 2;
		/** 右寄せ */
		static const int RIGHT  = 3;

	private:
		/** 表示位置（左寄せ、中央寄せ、右寄せ）*/
		int align;
		/** 表示文字列 */
		String text;
		
	public:
		/** デフォルトコンストラクタ */
		Label();
		
		/**
		 コンストラクタ.
		 描画位置は ALIGN_LEFT。
		 @param text ラベル
		 */
		Label(const String& text);
		
		/**
		 コンストラクタ
		 @param text ラベル
		 @param align 描画位置 (ALIGN_LEFT / ALIGN_CENTER / ALIGN_RIGHT)
		 */
		Label(const String& text, int align);
		
		/** デストラクタ */
		virtual ~Label();
		
		/**
		 テキスト設定
		 @param text
		 */
		virtual void setText(const String& text);
		
		/** テキストを得る */
		inline const char* getText() { return this->text.getBytes(); }
		
		/** 描画ハンドラ */
		virtual void paint(Graphics* g);
	};
}

#endif // _LABEL_H_INCLUDED_
