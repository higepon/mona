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
	enum {
		/** フォーカスイン */
		FOCUS_IN        = 300,
		/** フォーカスアウト */
		FOCUS_OUT       = 301,
		/** タイマー */
		TIMER           = 302,
		/** テキスト確定 */
		TEXT_CHANGED    = 303,
		/** 項目選択 */
		ITEM_SELECTED   = 304,
		/** ブロック増加 */
		BLOCK_INCLEMENT = 305,
		/** ブロック減少 */
		BLOCK_DECLEMENT = 306,
		/** カスタムイベント */
		CUSTOM_EVENT    = 400,
	};

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
	Event::Event() {
		this->type = CUSTOM_EVENT;
		this->source = NULL;
	}
	
	/**
	 コンストラクタ
	 @param type イベントタイプ
	 @param source イベント発生元部品
	 */
	Event::Event(int type, Control *source) {
		this->type = type;
		this->source = source;
	}
	
	/** デストラクタ */
	virtual Event::~Event() {}
	
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
