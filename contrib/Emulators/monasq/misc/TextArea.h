/*
Copyright (c) 2004 Hossy
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

#ifndef __INC_TEXTAREA_H__
#define __INC_TEXTAREA_H__

namespace monasq {

/**
 複数行対応テキストボックスクラス
 */
class TextArea : public TextField
{
public:
	TextArea(int buffer_size, bool draw_line);
	virtual ~TextArea();
	virtual void setText(const char* text);
	virtual char* getText();
	virtual void paint(Graphics* g);
	virtual void processEvent(Event* event);
	virtual void setBounds(int x, int y, int width, int height);
	virtual void SetOverwrite(bool is_overwrite) { _overwrite = is_overwrite; }
	virtual bool IsOverwrite() { return _overwrite; }

	// monasq 追加分
	virtual void linkScrollbar(Scrollbar* v_scrollbar);
	virtual void updateScrollbar(bool bMaxChange = false, bool bHeightChange = false);
	virtual int getVScroll();
	virtual void setVScroll(int y);
	virtual void insertText(const char* text);
	virtual void addText(const char* text);
	virtual void moveCaret(int x, int y);
	virtual int getCaretX() { return _ptr_x; }
	virtual int getCaretY() { return _ptr_y; }
	virtual int getCaretPos() { return XY2P(_ptr_x, _ptr_y); }
	virtual int getCaretLineStart() { return Y2P(_ptr_y); }
	virtual int getTextLength() { return _text_len; }


private:
	virtual void insertCharacter(char c);
	virtual void deleteCharacter();
	virtual void OffsetChange();
	virtual int Y2P(int y);
	virtual int XY2P(int x, int y);
	virtual void SetPos(int pos);
	virtual int LineCount();
	virtual void LineNoFormat(char* buff, int line, int size);
private:
	char* text;
	Event* _textEvent;
	int _buffer_size;
	int _text_len;
	int _ptr_x;
	int _ptr_y;
	int _offset_x;
	int _offset_y;
	int _max_line;
	int _max_col;
	bool _overwrite;
	bool _draw_line;
	
	// monasq 追加分
	int _keta;
	Scrollbar* _v_scrollbar;
};

}	// namespace monasq

#endif//__INC_TEXTAREA_H__
