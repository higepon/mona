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

#ifndef __BAYGUI_EVENT_H__
#define __BAYGUI_EVENT_H__

namespace baygui
{
	///** イベントメッセージ */
	//enum MessageType { WM_MOUSEMOVE, WM_MOUSEDOWN, WM_MOUSEUP, WM_KEYDOWN, WM_KEYUP };
	/** イベントタイプ */
	enum EventType {
		KEY_PRESSED     = 100,
		KEY_RELEASED    = 101,
		MOUSE_PRESSED   = 200,
		MOUSE_RELEASED  = 201,
		MOUSE_DRAGGED   = 202,
		MOUSE_MOVED     = 203,
		FOCUS_IN        = 300,
		FOCUS_OUT       = 301,
		TIMER           = 302,
		TEXT_CHANGED    = 303,
		ITEM_SELECTED   = 304,
		BLOCK_INCLEMENT = 305,
		BLOCK_DECLEMENT = 306
	};

	/**
	 イベントクラス.
	 OSごとのメッセージもこれに同封して、OSの違いを吸収する。
	 */
	class Event
	{
	public:
		/** イベントタイプ */
		int type;
		/** イベント発生元 */
		_P<Object> source;
	#ifdef MONA
		int from;
		int header;
		unsigned int arg1;
		unsigned int arg2;
		unsigned int arg3;
	#endif
		
		Event() {}
		~Event() {}
	};
	
	/** マウスイベント */
	class MouseEvent : public Event
	{
	public:
		/** ボタンの種類 */
		int button;
		/** X座標 */
		int x;
		/** Y座標 */
		int y;
		
		MouseEvent() {}
		~MouseEvent() {}
	};
	
	/** キーイベント */
	class KeyEvent : public Event
	{
	public:
		/** キーコード */
		int keycode;
		/** 修飾キー */
		int modifiers;
		
		KeyEvent() {}
		~KeyEvent() {}
	};
}

#endif  // __BAYGUI_EVENT_H__
