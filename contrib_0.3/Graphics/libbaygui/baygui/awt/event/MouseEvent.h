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

#if !defined(_MOUSEEVENT_H_INCLUDED_)
#define _MOUSEEVENT_H_INCLUDED_

namespace baygui {
	/**
	 マウスイベントクラス
	*/
	class MouseEvent : public Event {
	public:
		static const int BUTTON_LEFT = 1;
		static const int BUTTON_RIGHT = 2;
		static const int BUTTON_MIDDLE = 3;
		
	private:
		/** X座標 */
		int x;
		/** Y座標 */
		int y;
		/** ボタン種別 */
		dword button;
		
	public:
		/** デフォルトコンストラクタ */
		MouseEvent() {
			setType(MOUSE_PRESSED);
			setSource(NULL);
			this->x = this->y = 0;
			this->button = 0;
		}
		
		/**
		 コンストラクタ.
		 x, y はメインウィンドウの内部領域の左上に対する相対座標である。
		 絶対座標(ax,ay)は以下のように取得する。
		 <ul>
		 <li> ax = x + getMainWindow()->getRect()->getX() + Frame::getInsets()->left;
		 <li> ay = y + getMainWindow()->getRect()->getY() + Frame::getInsets()->top;
		 </ul>
		 @param type type イベントタイプ
		 @param source イベント発生元
		 @param x X座標（絶対座標）
		 @param y Y座標（絶対座標）
		*/
		MouseEvent(int type, Component* source, int x, int y)
		{
			setType(type);
			setSource(source);
			this->x = x;
			this->y = y;
		}
		
		/** デストラクタ */
		virtual ~MouseEvent() {}
		
		/** X座標を得る */
		inline int getX() { return this->x; }
		
		/** X座標を設定する */
		inline void setX(int x) { this->x = x; }
		
		/** Y座標を得る */
		inline int getY() { return this->y; }
		
		/** Y座標を設定する */
		inline void setY(int y) { this->y = y; }
		
		/** ボタン種別を得る */
		inline dword getButton() { return this->button; }
		
		/** ボタン種別を設定する */
		inline void setButton(dword button) { this->button = button; }
	};
}

#endif // _MOUSEEVENT_H_INCLUDED_
