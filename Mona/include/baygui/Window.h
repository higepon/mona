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
public:
	//
	// ウィンドウオフセット
	//
	enum {
		/** ウィンドウ上端 */
		INSETS_TOP       = 22,
		/** ウィンドウ下端 */
		INSETS_BOTTOM    = 6,
		/** ウィンドウ左端 */
		INSETS_LEFT      = 6,
		/** ウィンドウ右端 */
		INSETS_RIGHT     = 6,
	};
		
	//
	// ウィンドウの状態
	//
	enum {
		/** 通常時 */
		STATE_NORMAL     = 0,
		/** ウィンドウ移動中 */
		STATE_MOVING     = 1,
	};

private:
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
	/** オフセットX */
	int offsetX;
	/** オフセットY */
	int offsetY;
	/** オーバーラップウィンドウの状態 */
	unsigned int overlap;
	/** キーイベント */
	KeyEvent keyEvent;
	/** マウスイベント */
	MouseEvent mouseEvent;
	/** タイマーイベント */
	Event timerEvent;
	/** カスタムイベント */
	Event customEvent;
	/** 描画領域 */
	Graphics *_g;
	/** 描画バッファー */
	Image *_buffer;
	/** 内部描画領域 */
	Graphics *__g;
	/** 内部描画バッファー */
	Image *__buffer;
	/** 実行中フラグ */
	bool isRunning;
	
#ifdef MONA
protected:
	/** GUIサーバー上のウィンドウオブジェクト */
	guiserver_window *_window;
#endif

public:
	/** コンストラクタ */
	Window::Window();
	
	/** デストラクタ */
	virtual Window::~Window();

	/**
	 部品生成時ハンドラ.
	 Window::run()で呼ばれる。
	 独自の処理を付与したいときはWindow::onStart()を先に呼ぶこと。
	*/
	virtual void onStart();
	
	/**
	 部品破棄ハンドラ.
	 デストラクタ内で呼ばれる。
	 独自の処理を付与したいときはWindow::onExit()を後で呼ぶこと。
	 */
	virtual void onExit();

	/** ハンドルを得る */
	unsigned int getHandle();
	
	/** 描画オブジェクトを得る */
	virtual Graphics *getGraphics();
	
	/** 内部バッファーを得る */
	virtual Image *getBuffer();
	
	/** タイトルを得る */
	inline char *getTitle() { return this->title.getBytes(); }
	
	/**
	 タイトル設定
	 @param title タイトル
	 */
	virtual void setTitle(char *title);
	
	/**
	 表示状態を設定する
	 @param visible 表示状態 (true / false)
	 */
	virtual void setVisible(bool visible);
	
	/**
	 位置を変更する
	 @param x X座標
	 @param y Y座標
	*/
	virtual void setLocation(int x, int y);
	
	/**
	 タイマーをセットする
	 @param duration タイマーイベントが発動するまでの時間[ms]
	 */
	virtual void setTimer(int duration);
	
	/** 再描画 */
	virtual void repaint();
	
	/** 部品更新 */
	virtual void update();
	
	/** イベント処理 */
	virtual void postEvent(Event *event);
	
	/** アプリケーションループを抜ける */
	virtual void stop();
	
	/** アプリケーションループ */
	virtual void run();
};

#endif // _WINDOW_H_INCLUDED_
