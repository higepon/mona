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
private:
	/** キーコード */
	int keycode;
	/** 修飾キー */
	int modifiers;
	
public:
	/** デフォルトコンストラクタ */
	KeyEvent() {
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
	KeyEvent(int type, Control *source, int keycode, int modifiers) {
		setType(type);
		setSource(source);
		this->keycode = keycode;
		this->modifiers = modifiers;
	}
	
	/** デストラクタ */
	virtual ~KeyEvent() {}
	
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
