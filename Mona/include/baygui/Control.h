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
	/** 表示・非表示 */
	bool visible;
	/** x（絶対座標） */
	int x;
	/** y（絶対座標） */
	int y;
	/** 高さ */
	int height;
	/** 幅 */
	int width;
	/** オフセットX */
	int offsetX;
	/** オフセットY */
	int offsetY;
	/** フォントスタイル */
	int fontStyle;
	/** 部品の大きさ */
	Rect rect;
	/** フォーカスイベント */
	Event focusEvent;
	/** 背景色 */
	unsigned int backColor;
	/** 前景色 */
	unsigned int foreColor;
	/** 描画領域 */
	Graphics *_g;
	/** フォント情報 */
	FontMetrics *_metrics;
	/** 描画バッファー */
	Image *_buffer;

public:
	Control::Control();
	virtual Control::~Control();
	virtual void create();
	virtual void dispose();
	virtual void onEvent(Event *event);
	virtual void onPaint(Graphics *g);
	virtual void postEvent(Event *event);
	virtual void repaint();
	virtual void update();
	/** 活性・非活性を得る */
	inline  bool getEnabled() { return this->enabled; }
	/** フォーカス状態を得る */
	inline  bool getFocused() { return this->focused; }
	/** 表示・非表示を得る */
	inline  bool getVisible() { return this->visible; }
	/** 部品の大きさ */
	inline  Rect *getRect() { return &this->rect; }
	/** 背景色を得る */
	inline  unsigned int getBackground() { return this->backColor; }
	/** 前景色を得る */
	inline  unsigned int getForeground() { return this->foreColor; }
	/** フォントスタイルを得る */
	inline  int getFontStyle() { return this->fontStyle; }
	/** 親部品を得る */
	inline  Container *getParent() { return this->parent; }
	/** 描画オブジェクトを得る */
	inline  Graphics *getGraphics() { return this->_g; }
	virtual Control *getMainWindow();
	virtual void setEnabled(bool enabled);
	virtual void setFocused(bool focused);
	virtual void setVisible(bool visible);
	virtual void setRect(int x, int y, int width, int height);
	virtual void setLocation(int x, int y);
	virtual void setParent(Container *parent);
	virtual void setBackground(unsigned int backColor);
	virtual void setForeground(unsigned int foreColor);
	virtual void setFontStyle(int style);
};

#endif // _CONTROL_H_INCLUDED_
