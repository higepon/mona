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
IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#if !defined(_EVENT_H_INCLUDED_)
#define _EVENT_H_INCLUDED_

class Control;

/**
 イベント基底クラス
*/
class Event : public Object {
public:
	//
	// イベントタイプ一覧
	//
	/** キー押下 */
	static const int KEY_PRESSED     = 100;
	/** キーリリース */
	static const int KEY_RELEASED    = 101;
	/** マウスクリック */
	static const int MOUSE_PRESSED   = 200;
	/** マウスリリース */
	static const int MOUSE_RELEASED  = 201;
	/** マウスドラッグ */
	static const int MOUSE_DRAGGED   = 202;
	/** マウス移動 */
	static const int MOUSE_MOVED     = 203;
	/** フォーカスイン */
	static const int FOCUS_IN        = 300;
	/** フォーカスアウト */
	static const int FOCUS_OUT       = 301;
	/** タイマー */
	static const int TIMER           = 302;
	/** テキスト確定 */
	static const int TEXT_CHANGED    = 303;
	/** 項目選択 */
	static const int ITEM_SELECTED   = 304;
	/** ブロック増加 */
	static const int BLOCK_INCLEMENT = 305;
	/** ブロック減少 */
	static const int BLOCK_DECLEMENT = 306;
	/** カスタムイベント */
	static const int CUSTOM_EVENT    = 400;
	
	//
	// キーコード
	//
	/** Escape */
	static const int VKEY_ESC       =  0;
	/** BackSpace */
	static const int VKEY_BACKSPACE = 14;
	/** Tab */
	static const int VKEY_TAB       = 15;
	/** Enter */
	static const int VKEY_ENTER     = 28;
	/** PageUp */
	static const int VKEY_PGUP      = 354;
	/** PageDown */
	static const int VKEY_PGDOWN    = 355;
	/** Insert */
	static const int VKEY_INSERT    = 356;
	/** Delete */
	static const int VKEY_DELETE    = 357;
	/** ↑ */
	static const int VKEY_UP        = 358;
	/** ← */
	static const int VKEY_LEFT      = 359;
	/** →  */
	static const int VKEY_RIGHT     = 360;
	/** ↓ */
	static const int VKEY_DOWN      = 361;
	/** Home */
	static const int VKEY_HOME      = 362;
	/** End */
	static const int VKEY_END       = 363;
	/** F11 */
	static const int VKEY_F11       = 382;
	/** F12 */
	static const int VKEY_F12       = 383;
	/** F1 */
	static const int VKEY_F1        = 398;
	/** F2 */
	static const int VKEY_F2        = 399;
	/** F3 */
	static const int VKEY_F3        = 400;
	/** F4 */
	static const int VKEY_F4        = 401;
	/** F5 */
	static const int VKEY_F5        = 402;
	/** F6 */
	static const int VKEY_F6        = 403;
	/** F7 */
	static const int VKEY_F7        = 404;
	/** F8 */
	static const int VKEY_F8        = 405;
	/** F9 */
	static const int VKEY_F9        = 406;
	/** F10 */
	static const int VKEY_F10       = 407;
	/** 左シフト */
	static const int VKEY_LSHIFT    = 412;
	/** Alt */
	static const int VKEY_ALT       = 415;
	/** Ctrl */
	static const int VKEY_CTRL      = 416;
	/** 右メニュー */
	static const int VKEY_RMENU     = 424;
	/** 左メニュー */
	static const int VKEY_LMENU     = 425;
	/** 右シフト */
	static const int VKEY_RSHIFT    = 427;
	
private:
	/** イベントタイプ */
	int type;
	/** イベント発生元 */
	Control *source;

#ifdef MONA
public:
	/** ヘッダー */
	unsigned int header;
	/** 引数(1) */
	unsigned int arg1;
	/** 引数(2) */
	unsigned int arg2;
	/** 引数(3) */
	unsigned int arg3;
	/** メッセージ発生元 */
	unsigned int from;
	/** 文字列 */
	char str[128];
	/** 文字列の長さ */
	int length;
#endif

public:
	/** デフォルトコンストラクタ */
	Event() {
		this->type = CUSTOM_EVENT;
		this->source = NULL;
	}
	
	/**
	 コンストラクタ
	 @param type イベントタイプ
	 @param source イベント発生元部品
	 */
	Event(int type, Control *source) {
		this->type = type;
		this->source = source;
	}
	
	/** デストラクタ */
	virtual ~Event() {}
	
	/** イベントタイプを得る */
	inline int getType() { return this->type; }
	
	/** イベントタイプを設定する */
	inline void setType(int type) { this->type = type; }
	
	/** イベントの発生部品を得る */
	inline Control* getSource() { return this->source; }
	
	/** イベントの発生部品を設定する */
	inline void setSource(Control *c) { this->source = c; }
};

#endif // _EVENT_H_INCLUDED_
