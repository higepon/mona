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

#if !defined(_WINDOW_H_INCLUDED_)
#define _WINDOW_H_INCLUDED_

/**
 ウィンドウクラス
*/
class Window : public Container {
private:
	/** 内部描画領域 */
	Graphics *__g;
	/** 内部描画バッファー */
	Image *__buffer;
	
protected:
	/** タイトル */
	String title;
	/** 修飾キー */
	int modifiers;
	/** ウィンドウの状態 */
	int state;
	/** 以前のマウスのX座標 */
	int preX;
	/** 以前のマウスのY座標 */
	int preY;
	/** オーバーラップウィンドウの状態 */
	unsigned int overlap;
	/** 実行中フラグ */
	bool isRunning;
	/** キーイベント */
	KeyEvent keyEvent;
	/** マウスイベント */
	MouseEvent mouseEvent;
	/** タイマーイベント */
	Event timerEvent;
	/** カスタムイベント */
	Event customEvent;

public:
	Window::Window();
	virtual Window::~Window();
	virtual void create();
	virtual void dispose();
	inline char  *getTitle() { return this->title.getBytes(); }
	virtual void setTitle(char *title);
	virtual void setTimer(int duration);
	virtual void postEvent(Event *event);
	virtual void repaint();
	virtual void update();
	virtual void run();
};

#endif // _WINDOW_H_INCLUDED_
