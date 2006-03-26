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

namespace baygui {
	/**
	 ウィンドウクラス
	*/
	class Window : public Container {
	public:
		//
		// ウィンドウの状態
		//
		/** 通常時 */
		static const int STATE_NORMAL = 0;
		/** ウィンドウ移動中 */
		static const int STATE_MOVING = 1;
		
	private:
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
		dword overlap;
		/** 実行中フラグ */
		bool isRunning;
		/** タイマーID */
		dword timerID;
		/** キーイベント */
		KeyEvent keyEvent;
		/** マウスイベント */
		MouseEvent mouseEvent;
		/** タイマーイベント */
		Event timerEvent;
		/** カスタムイベント */
		Event customEvent;

	protected:
		/** フチの状態 */
		int border;
		/** 配置位置 */
		Insets insets;
		/** 描画領域 */
		Graphics* _g;
		/** 描画バッファー */
		Image* _buffer;
		/** 内部描画領域 */
		Graphics* __g;
		/** 内部描画バッファー */
		Image* __buffer;
		/** GUIサーバーID */
		dword guisvrID;
		/** GUIサーバー上のウィンドウオブジェクト */
		guiserver_window* _window;
	
	protected:
		/** 閉じるボタンがクリックされたかどうか */
		virtual bool getCloseButtonClicked(int px, int py)
		{
			return false;
		}
		
		/** タイトルバーがクリックされたかどうか */
		virtual bool getTitlebarClicked(int px, int py)
		{
			return (0 <= px && px < getWidth() && 0 <= py && py < getHeight()) ? true : false;
		}

	public:
		/** コンストラクタ */
		Window();
		
		/** デストラクタ */
		virtual ~Window();

		/**
		 部品生成時ハンドラ.
		 Window::run()で呼ばれる。
		 独自の処理を付与したいときはWindow::addNotify()を先に呼ぶこと。
		*/
		virtual void addNotify();
		
		/**
		 部品破棄ハンドラ.
		 デストラクタ内で呼ばれる。
		 独自の処理を付与したいときはWindow::removeNotify()を後で呼ぶこと。
		 */
		virtual void removeNotify();

		/** ハンドルを得る */
		inline dword getHandle() { return (this->_window != NULL) ? this->_window->Handle : 0; }
		
		/** 配置位置を得る */
		virtual Insets* getInsets();
		
		/** 描画オブジェクトを得る */
		virtual Graphics* getGraphics();
		
		/** 内部バッファーを得る */
		virtual Image* getBuffer();
		
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
		
		/** 部品部分更新 */
		virtual void update(int x, int y, int w, int h);
		
		/** イベント処理 */
		virtual void dispatchEvent(Event* event);
        
	    /** アプリケーションループを抜ける */
		virtual void stop();
		
		/** アプリケーションループ */
		virtual void run();
	};
}

#endif // _WINDOW_H_INCLUDED_
