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

#if !defined(_BUTTON_H_INCLUDED_)
#define _BUTTON_H_INCLUDED_

namespace baygui {
	/**
	 ボタンクラス
	*/
	class Button : public Component {
	private:
		/** ボタンが押されたかどうか */
		bool pushed;
		/** ボタンのラベル */
		String label;
		
	public:
		/** デフォルトコンストラクタ */
		Button();
		
		/**
		 コンストラクタ
		 @param label ラベル
		 */
		Button(char* label);
		
		/** デストラクタ */
		virtual ~Button();
		
		/**
		 ラベルを設定する
		 @param label ラベル
		 */
		virtual void setLabel(char* label);
		
		/** ボタンが押されたかどうかを得る */
		inline bool getPushed() { return this->pushed; }
		
		/** ラベルを得る */
		inline char* getLabel() { return this->label.getBytes(); }
		
		/** 描画ハンドラ */
		virtual void paint(Graphics* g);
		
		/** イベントハンドラ */
		virtual void processEvent(Event* event);
	};
}

#endif // _BUTTON_H_INCLUDED_
