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

#if !defined(_CONTROL_H_INCLUDED_)
#define _CONTROL_H_INCLUDED_

class Container;

/**
 コントロールクラス
*/
class Control : public Object {
protected:
	/** 親部品 */
	Container *parent;
	/** 活性・非活性 */
	bool enabled;
	/** フォーカス有無 */
	bool focused;
	/** アイコン化フラグ */
	bool iconified;
	/** 初回描画フラグ */
	bool firstpaint;
	/** x（絶対座標） */
	int x;
	/** y（絶対座標） */
	int y;
	/** 高さ */
	int height;
	/** 幅 */
	int width;
	/** 部品の大きさ */
	Rect *_rect;
	/** 描画領域 */
	Graphics *_g;
	/** フォーカスイベント */
	Event *_focusEvent;
	/** 背景色 */
	unsigned int backColor;
	/** 前景色 */
	unsigned int foreColor;
	/** フォント */
	Font *font;

public:
	Control::Control();
	virtual Control::~Control();
	virtual void onEvent(Event *event);
	virtual void onPaint(Graphics *g);
	virtual void postEvent(Event *event);
	virtual void repaint();
	virtual bool getEnabled();
	virtual bool getFocused();
	virtual bool getIconified();
	virtual Rect *getRect();
	virtual Container *getParent();
	virtual unsigned int getBackground();
	virtual unsigned int getForeground();
	virtual Font *getFont();
	virtual void setEnabled(bool enabled);
	virtual void setFocused(bool focused);
	virtual void setIconified(bool iconified);
	virtual void setRect(int x, int y, int width, int height);
	virtual void setParent(Container *parent);
	virtual void setBackground(unsigned int backColor);
	virtual void setForeground(unsigned int foreColor);
	virtual void setFont(Font *font);
};

#endif // _CONTROL_H_INCLUDED_
