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
IN NO KEYEVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#if !defined(_KEYEVENT_H_INCLUDED_)
#define _KEYEVENT_H_INCLUDED_

/**
 キーイベントクラス
*/
class KeyEvent : public Event {
public:
	//
	// イベントタイプ一覧
	//
	enum {
		/** キー押下 */
		KEY_PRESSED     = 100,
		/** キーリリース */
		KEY_RELEASED    = 101,
	};

	//
	// 特殊キーコード一覧
	//
	enum {
		VKEY_ESC       =  0,
		VKEY_BACKSPACE = 14,
		VKEY_TAB       = 15,
		VKEY_ENTER     = 28,

		VKEY_PGUP      = 354,
		VKEY_PGDOWN    = 355,
		VKEY_INSERT    = 356,
		VKEY_DELETE    = 357,
		VKEY_UP        = 358,
		VKEY_LEFT      = 359,
		VKEY_RIGHT     = 360,
		VKEY_DOWN      = 361,
		VKEY_HOME      = 362,
		VKEY_END       = 363,

		VKEY_F11       = 382,
		VKEY_F12       = 383,
		VKEY_F1        = 398,
		VKEY_F2        = 399,
		VKEY_F3        = 400,
		VKEY_F4        = 401,
		VKEY_F5        = 402,
		VKEY_F6        = 403,
		VKEY_F7        = 404,
		VKEY_F8        = 405,
		VKEY_F9        = 406,
		VKEY_F10       = 407,
		VKEY_LSHIFT    = 412,
		VKEY_ALT       = 415,
		VKEY_CTRL      = 416,
		VKEY_RMENU     = 424,
		VKEY_LMENU     = 425,
		VKEY_RSHIFT    = 427,
	};

private:
	/** キーコード */
	int keycode;
	/** 修飾キー */
	int modifiers;
	
public:
	/** デフォルトコンストラクタ */
	KeyEvent::KeyEvent() {
		setType(KEY_PRESSED);
		setSource(NULL);
		this->keycode = 0;
		this->modifiers = 0;
	}
	
	/**
	 コンストラクタ
	 @param type type イベントタイプ
	 @param source イベント発生元
	 @param keycode キーコード
	 @param modifiers 修飾キー
	 */
	KeyEvent::KeyEvent(int type, Control *source, int keycode, int modifiers) {
		setType(type);
		setSource(source);
		this->keycode = keycode;
		this->modifiers = modifiers;
	}
	
	/** デストラクタ */
	virtual KeyEvent::~KeyEvent() {}
	
	/** キーコードを得る */
	inline int getKeycode() { return this->keycode; }
	
	/** キーコードを設定する */
	inline void setKeycode(int keycode) { this->keycode = keycode; }
	
	/** 修飾キーを得る */
	inline int getModifiers() { return this->modifiers; }
	
	/** 修飾キーを設定する */
	inline void setModifiers(int modifiers) { this->modifiers = modifiers; }
};

#endif // _KEYEVENT_H_INCLUDED_
