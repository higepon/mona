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

#if !defined(_TEXTFIELD_H_INCLUDED_)
#define _TEXTFIELD_H_INCLUDED_

/**
 テキストボックスクラス
*/
class TextField : public Control {
private:
	int textPtr;
	int textLen;
	int offx;
	int offy;
	char text[MAX_TEXT_LEN];
	Event textEvent;
	
private:
	/** 1文字挿入する */
	virtual void insertCharacter(char c);
	
	/** 一文字削除する */
	virtual void deleteCharacter();

public:
	/** コンストラクタ */
	TextField::TextField();
	
	/** デストラクタ */
	virtual TextField::~TextField();
	
	/**
	 テキストを設定する
	 @param text
	 */
	virtual void setText(char *text);
	
	/** テキストを得る */
	inline  char *getText() { return this->text; }
	
	/** 描画ハンドラ */
	virtual void onPaint(Graphics *g);
	
	/** イベントハンドラ */
	virtual void onEvent(Event *event);
};

#endif // _TEXTFIELD_H_INCLUDED_
