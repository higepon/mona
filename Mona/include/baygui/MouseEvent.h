/*
Copyright (c) 2004 bayside
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. The name of the author may not be used to endorse or promote products
   derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO MOUSEEVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#if !defined(_MOUSEEVENT_H_INCLUDED_)
#define _MOUSEEVENT_H_INCLUDED_

/**
 マウスイベントクラス
*/
class MouseEvent : public Event {
public:
	//
	// イベントタイプ一覧
	//
	enum {
		/** マウスクリック */
		MOUSE_PRESSED   = 200,
		/** マウスリリース */
		MOUSE_RELEASED  = 201,
		/** マウスドラッグ */
		MOUSE_DRAGGED   = 202,
		/** マウス移動 */
		MOUSE_MOVED     = 203,
	};

private:
	/** X座標 */
	int x;
	/** Y座標 */
	int y;
	/** ボタン種別 */
	unsigned int button;
	
public:
	/** デフォルトコンストラクタ */
	MouseEvent::MouseEvent() {
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
	 <li> ax = x + getMainWindow()->getRect()->getX() + Window::INSETS_LEFT;
	 <li> ay = y + getMainWindow()->getRect()->getY() + Window::INSETS_TOP;
	 </ul>
	 @param type type イベントタイプ
	 @param source イベント発生元
	 @param x X座標（絶対座標）
	 @param y Y座標（絶対座標）
	*/
	MouseEvent::MouseEvent(int type, Control *source, int x, int y)
	{
		setType(type);
		setSource(source);
		this->x = x;
		this->y = y;
	}
	
	/** デストラクタ */
	virtual MouseEvent::~MouseEvent() {}
	
	/** X座標を得る */
	inline int getX() { return this->x; }
	
	/** X座標を設定する */
	inline void setX(int x) { this->x = x; }
	
	/** Y座標を得る */
	inline int getY() { return this->y; }
	
	/** Y座標を設定する */
	inline void setY(int y) { this->y = y; }
	
	/** ボタン種別を得る */
	inline unsigned int getButton() { return this->button; }
	
	/** ボタン種別を設定する */
	inline void setButton(unsigned int button) { this->button = button; }
};

#endif // _MOUSEEVENT_H_INCLUDED_
