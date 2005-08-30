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

#if !defined(_EVENT_H_INCLUDED_)
#define _EVENT_H_INCLUDED_

namespace baygui {
	class Component;

	/**
	 イベント基底クラス
	*/
	class Event : public Object {
	public:
		//
		// イベントタイプ一覧
		//
		/** キー押下 */
		static const int KEY_PRESSED      = 100;
		/** キーリリース */
		static const int KEY_RELEASED     = 101;
		/** マウスクリック */
		static const int MOUSE_PRESSED    = 200;
		/** マウスリリース */
		static const int MOUSE_RELEASED   = 201;
		/** マウスドラッグ */
		static const int MOUSE_DRAGGED    = 202;
		/** マウス移動 */
		static const int MOUSE_MOVED      = 203;
		/** フォーカスイン */
		static const int FOCUS_IN         = 300;
		/** フォーカスアウト */
		static const int FOCUS_OUT        = 301;
		/** タイマー */
		static const int TIMER            = 302;
		/** テキスト確定 */
		static const int TEXT_CHANGED     = 303;
		/** 項目選択 */
		static const int ITEM_SELECTED    = 304;
		/** ブロック増加 */
		static const int BLOCK_INCLEMENT  = 305;
		/** ブロック減少 */
		static const int BLOCK_DECLEMENT  = 306;
		/** ボタン押下 */
		static const int ACTION_PERFORMED = 307;
		/** ウィンドウオープン */
		static const int WINDOW_OPENED    = 308;
		/** ウィンドウクローズ */
		static const int WINDOW_CLOSED    = 309;
		/** カスタムイベント */
		static const int CUSTOM_EVENT     = 400;
		
	private:
		/** イベントタイプ */
		int type;
		/** イベント発生元 */
		Component* source;
		/** イベントを消費したかどうか */
		bool consumed;

	public:
		//
		// Mona用イベント
		//
		/** ヘッダー */
		dword header;
		/** 引数(1) */
		dword arg1;
		/** 引数(2) */
		dword arg2;
		/** 引数(3) */
		dword arg3;
		/** メッセージ発生元 */
		dword from;
		/** 文字列 */
		char str[128];
		/** 文字列の長さ */
		int length;

	public:
		/** デフォルトコンストラクタ */
		Event() {
			this->type = CUSTOM_EVENT;
			this->source = NULL;
			this->consumed = false;
		}
		
		/**
		 コンストラクタ
		 @param type イベントタイプ
		 @param source イベント発生元部品
		 */
		Event(int type, Component* source) {
			this->type = type;
			this->source = source;
			this->consumed = false;
		}
		
		/** デストラクタ */
		virtual ~Event() {}
		
		/** イベントタイプを得る */
		inline int getType() { return this->type; }
		
		/** イベントタイプを設定する */
		inline void setType(int type) { this->type = type; }
		
		/** イベントの発生部品を得る */
		inline Component* getSource() { return this->source; }
		
		/** イベントの発生部品を設定する */
		inline void setSource(Component* c) { this->source = c; }
		
		/** イベントを消費したかどうかを得る */
		inline bool isConsumed() { return this->consumed; }
		
		/** イベントを消費する */
		inline void consume() { this->consumed = true; }
	};
}

#endif // _EVENT_H_INCLUDED_
