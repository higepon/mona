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

#if !defined(_LABEL_H_INCLUDED_)
#define _LABEL_H_INCLUDED_

/**
 ラベルクラス
*/
class Label : public Control {
private:
	/** 表示位置（左寄せ、中央寄せ、右寄せ）*/
	int align;
	/** 表示文字列 */
	String text;
	
public:
	/**
	 コンストラクタ.
	 描画位置は ALIGN_LEFT。
	 @param text ラベル
	 */
	Label::Label(char *text);
	
	/**
	 コンストラクタ
	 @param text ラベル
	 @param align 描画位置 (ALIGN_LEFT / ALIGN_CENTER / ALIGN_RIGHT)
	 */
	Label::Label(char *text, int align);
	
	/** デストラクタ */
	virtual Label::~Label();
	
	/**
	 テキスト設定
	 @param text
	 */
	virtual void setText(char *text);
	
	/** テキストを得る */
	inline  char *getText() { return this->text.getBytes(); }
	
	/** 描画ハンドラ */
	virtual void onPaint(Graphics *g);
};

#endif // _LABEL_H_INCLUDED_
