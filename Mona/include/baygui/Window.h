/*
Copyright (c) 2005 bayside

Permission is hereby granted, free of charge, to any person 
obtaining a copy of this software and associated documentation files 
(the "Software"), to deal in the Software without restriction, 
including without limitation the rights to use, copy, modify, merge, 
publish, distribute, sublicense, and/or sell copies of the Software, 
and to permit persons to whom the Software is furnished to do so, 
subject to the following conditions:

The above copyright notice and this permission notice shall be 
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY 
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
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
	/** ウィンドウ上端 */
	static const int INSETS_TOP       = 22;
	/** ウィンドウ下端 */
	static const int INSETS_BOTTOM    = 6;
	/** ウィンドウ左端 */
	static const int INSETS_LEFT      = 6;
	/** ウィンドウ右端 */
	static const int INSETS_RIGHT     = 6;
	
	//
	// ウィンドウの状態
	//
	/** 通常時 */
	static const int STATE_NORMAL     = 0;
	/** ウィンドウ移動中 */
	static const int STATE_MOVING     = 1;

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
	/** 実行中フラグ */
	bool isRunning;
	/** タイマーID */
	unsigned int timerID;
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
	
#ifdef MONA
protected:
	/** GUIサーバー上のウィンドウオブジェクト */
	guiserver_window *_window;
#endif

public:
	/** コンストラクタ */
	Window();
	
	/** デストラクタ */
	virtual ~Window();

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
#ifdef MONA
	inline unsigned int getHandle() { return (this->_window != NULL) ? this->_window->Handle : 0; }
#else
	inline unsigned int getHandle() { return 0; }
#endif
	
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
