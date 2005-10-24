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

package java.awt;

import java.awt.event.KeyEvent;
import java.awt.event.MouseEvent;
import java.awt.event.TimerEvent;

/**
 ウィンドウクラス
*/
public class Window extends Container {
	/** 閉じるボタン (パレット) */
	private int close_palette[] = {
		0xff1c1a1c,
		0xff8c8e8c,
		0xffcccecc,
		0xffacaeac,
		0xffeceeec,
		0xff9c9e9c,
		0xffdcdedc,
		0xffbcbebc,
		0xfffcfefc,
	};

	/** 閉じるボタン (データ) */
	private byte close_data[] = {
		0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x2,
		0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x8,
		0x1,0x0,0x8,0x2,0x2,0x2,0x2,0x2,0x2,0x2,0x2,0x0,0x8,
		0x1,0x0,0x2,0x5,0x5,0x3,0x3,0x7,0x7,0x2,0x1,0x0,0x8,
		0x1,0x0,0x2,0x5,0x3,0x3,0x7,0x7,0x2,0x2,0x1,0x0,0x8,
		0x1,0x0,0x2,0x3,0x3,0x7,0x7,0x2,0x2,0x6,0x1,0x0,0x8,
		0x1,0x0,0x2,0x3,0x7,0x7,0x2,0x2,0x6,0x6,0x1,0x0,0x8,
		0x1,0x0,0x2,0x7,0x7,0x2,0x2,0x6,0x6,0x4,0x1,0x0,0x8,
		0x1,0x0,0x2,0x7,0x2,0x2,0x6,0x6,0x4,0x4,0x1,0x0,0x8,
		0x1,0x0,0x2,0x2,0x2,0x6,0x6,0x4,0x4,0x8,0x1,0x0,0x8,
		0x1,0x0,0x2,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x0,0x8,
		0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x8,
		0x2,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,
	};
	
	//
	// ウィンドウの状態
	//
	/** 通常時 */
	public static final int STATE_NORMAL = 0;
	/** ウィンドウ移動中 */
	public static final int STATE_MOVING = 1;
	
	/** 修飾キー */
	private int modifiers;
	/** ウィンドウの状態 */
	private int state;
	/** 以前のマウスのX座標 */
	private int preX;
	/** 以前のマウスのY座標 */
	private int preY;
	/** オフセットX */
	private int offsetX;
	/** オフセットY */
	private int offsetY;
	/** オーバーラップウィンドウの状態 */
	private int overlap;
	/** 実行中フラグ */
	private boolean isRunning;
	/** タイマーID */
	private int timerID;
	/** カスタムイベント */
	private AWTEvent customEvent;
	/** キーイベント */
	private KeyEvent keyEvent;
	/** マウスイベント */
	private MouseEvent mouseEvent;
	/** タイマーイベント */
	private TimerEvent timerEvent;
	/** タイトル */
	private String title;
	
	/** フチの状態 */
	protected int border;
	/** 配置位置 */
	protected Insets insets;
	/** 描画領域 */
	protected Graphics _g;
	/** 描画バッファー */
	protected Image _buffer;
	/** 内部描画領域 */
	protected Graphics __g;
	/** 内部描画バッファー */
	protected Image __buffer;
	/** GUIサーバーID */
	//protected int guisvrID;
	/** GUIサーバー上のウィンドウオブジェクト */
	//protected guiserver_window* _window;

	/** 閉じるボタンがクリックされたかどうか */
	public boolean getCloseButtonClicked(int px, int py) {
		return (4 <= px && px < 17 && 4 <= py && py < 17) ? true : false;
	}

	/** タイトルバーがクリックされたかどうか */
	public boolean getTitlebarClicked(int px, int py) {
		return (0 <= px && px < getWidth() && 0 <= py && py < getInsets().top) ? true : false;
	}

	private native boolean create(int[] buf, String title, int w, int h);
	private native void run();
	private native void exit();
	private native boolean doEvents();
	private native void invalidate();
	public void onKeyDown(int keycode, int mod) {}
	public void onKeyUp(int keycode, int mod) {}
	public void onKeyPress(char ch) {}
	public void onMouseDown(int button, int x, int y) {
		if (getCloseButtonClicked(x, y)) exit();
	}
	public void onMouseUp(int button, int x, int y) {}
	public void onMouseMove(int button, int x, int y) {}

	/** デフォルトコンストラクタ */
	public Window() {
		this.modifiers = this.state = this.preX = this.preY = this.offsetX = this.offsetY = this.overlap = 0;
		this.border = 0; /* フチなし */
		this.isRunning = false;
		this.timerID = 0;
		this._g = null;
		this.__g = null;
		this._buffer = null;
		this.__buffer = null;
		this.insets = new Insets();
		this.insets.top = 0;
		this.insets.bottom = 0;
		this.insets.left = 0;
		this.insets.right = 0;
		this.keyEvent = new KeyEvent(KeyEvent.KEY_PRESSED, this, 0, 0);
		this.mouseEvent = new MouseEvent(MouseEvent.MOUSE_PRESSED, this, 0, 0);
		this.timerEvent = new TimerEvent(TimerEvent.TIMER, this);
		this.customEvent = new AWTEvent(AWTEvent.CUSTOM_EVENT, this);
		this.title = "Window";
	}

	/**
	 * コピーコンストラクタ
	 * @param title タイトル
	*/
	public Window(String title) {
		this();
		this.title = title;
	}
	
	/**
	 部品生成時ハンドラ.
	 Window.run()で呼ばれる。
	 独自の処理を付与したいときはWindow.addNotify()を先に呼ぶこと。
	*/
	public void addNotify() {
		if (this._buffer != null) return;
		
		/* 描画バッファー、描画オブジェクトの生成 */
		int w = getWidth() - getInsets().left - getInsets().right;
		int h = getHeight() - getInsets().top - getInsets().bottom;
		this._buffer = new Image(w, h);
		this._g = new Graphics(this._buffer);
		this.__buffer = new Image(getWidth(), getHeight());
		this.__g = new Graphics(this.__buffer);
		
		/* ネイティブメソッド */
		create(this.__buffer.getBuffer(), getTitle(), getWidth(), getHeight());
		
		/* ウィンドウをアクティブにする */
		setFocused(true);
	}
	
	/**
	 部品破棄ハンドラ.
	 デストラクタ内で呼ばれる。
	 独自の処理を付与したいときはWindow.removeNotify()を後で呼ぶこと。
	 */
	public void removeNotify() {
		setVisible(false);
	}

	/** ハンドルを得る */
	//public int getHandle() { return (this._window != null) ? this._window.Handle : 0; }
	
	/** 配置位置を得る */
	public Insets getInsets() {
		return this.insets;
	}
	
	/** タイトルを得る */
	public String getTitle() { return this.title; }
	
	/**
	 タイトル設定
	 @param title タイトル
	 */
	public void setTitle(String title) {
		this.title = title;
	}
	
	/** 描画オブジェクトを得る */
	public Graphics getGraphics() {
		if (this.border == 0) {
			return this.__g;
		} else {
			return this._g;
		}
	}
	
	/** 内部バッファーを得る */
	public Image getBuffer() {
		return this._buffer;
	}
	
	/**
	 表示状態を設定する
	 @param visible 表示状態 (true / false)
	 */
	public void setVisible(boolean visible) {
		super.setVisible(visible);
		update();
	}
	
	/**
	 位置を変更する
	 @param x X座標
	 @param y Y座標
	*/
	public void setLocation(int x, int y) {
		super.setLocation(x, y);
		update();
	}
	
	/**
	 タイマーをセットする
	 @param duration タイマーイベントが発動するまでの時間[ms]
	 */
	public void setTimer(int duration) {
		/* 非活性のときはタイマーを発生させない */
		if (getEnabled() == false) return;
	}
	
	/** 再描画 */
	/*public void repaint() {
		if (this.__g == null) return;
		super.repaint();
	}*/
	/** 再描画 */
	public void repaint() {
		if (this.__g == null) return;
		
		if (this.border == 1) {
			/* 矩形ウィンドウ */
			int w = getWidth();
			int h = getHeight();
			
			/* 外枠 */
			__g.setColor(Color.lightGray);
			__g.fillRect(0, 0, w, h);
			__g.setColor(Color.black);
			__g.drawRect(0, 0, w, h);
			
			/* 内枠 */
			__g.setColor(Color.black);
			__g.drawRect(5, 21, w - 10, h - 26);
			
			/* 枠 */
			__g.setColor(Color.white);
			__g.drawLine(1, 1, w - 2, 1);
			__g.drawLine(1, 1, 1, h - 2);
			__g.drawLine(w - 5, 21, w - 5, h - 5);
			__g.drawLine(5, h - 5, w - 5, h - 5);
			__g.setColor(Color.gray);
			__g.drawLine(w - 2, 2, w - 2, h - 2);
			__g.drawLine(2, h - 2, w - 2, h - 2);
			__g.drawLine(4, 20, w - 6, 20);
			__g.drawLine(4, 20, 4, h - 6);
			
			if (getFocused() == true) {
				/* タイトルバー */
				for (int i = 4; i <= 14; i = i + 2) {
					__g.setColor(Color.gray);
					__g.drawLine(20, i, w - 7, i);
					__g.setColor(Color.white);
					__g.drawLine(21, i + 1, w - 6, i + 1);
				}
				
				/* 閉じるボタン */
				for (int i = 0; i < 13; i++) {
					for (int j = 0; j < 13; j++) {
						__g.drawPixel(j + 4, i + 4, close_palette[close_data[i * 13 + j] & 0xFF]);
					}
				}
			}

			/* タイトル */
			int fw = getFontMetrics().getWidth(getTitle());
			int fh = getFontMetrics().getHeight(getTitle());
			__g.setColor(Color.lightGray);
			__g.fillRect(((w - fw) / 2) - 4, 2, fw + 8, getInsets().top - 4);
			if (getFocused() == true) {
				__g.setColor(Color.black);
			} else {
				__g.setColor(Color.gray);
			}
			__g.drawString(getTitle(), ((w - fw) / 2), ((getInsets().top - fh) / 2));
			
			/* 内側 */
			int[] temp_buffer = _buffer.getBuffer();
			int I = (getWidth() - getInsets().left - getInsets().right) * 
					(getHeight() - getInsets().top - getInsets().bottom);
			for (int i = 0; i < I; i++) {
				temp_buffer[i] = Color.lightGray;
			}
			paint(getGraphics());
			__g.drawImage(_buffer, getInsets().left, getInsets().top);
		} else {
			/* 内側 */
			int[] temp_buffer = __buffer.getBuffer();
			int I = (getWidth() - getInsets().left - getInsets().right) * 
					(getHeight() - getInsets().top - getInsets().bottom);
			for (int i = 0; i < I; i++) {
				temp_buffer[i] = Color.lightGray;
			}
			paint(getGraphics());
			__g.drawImage(__buffer, getInsets().left, getInsets().top);
		}
		
		//super.repaint();
		/* ネイティブメソッド */
		invalidate();
	}
	
	/** 部品更新 */
	public void update() {
		update(this.getX(), this.getY(), this.getWidth(), this.getHeight());
	}
	
	/** 部品部分更新 */
	public void update(int x, int y, int w, int h) {
		/* ネイティブメソッド */
		invalidate();
	}
	
	/** イベント処理 */
	public void dispatchEvent(AWTEvent event) {
		if (event.getType() >= MouseEvent.MOUSE_PRESSED && event.getType() <= MouseEvent.MOUSE_MOVED) {
			MouseEvent me = (MouseEvent)event;
			int px = me.getX() - getX();
			int py = me.getY() - getY();
			if (event.getType() == MouseEvent.MOUSE_PRESSED) {
				/* 閉じるボタンクリック */
				if (getCloseButtonClicked(px, py)) {
					stop();
					return;
				/* タイトルバークリック */
				} else if (getTitlebarClicked(px, py)) {
					this.state = STATE_MOVING;
					/* キャプチャー要求とウィンドウ移動用オブジェクト作成要求 */
					/*MessageInfo info;
					MonAPI.Message.sendReceive(null, this.guisvrID, MSG_GUISERVER_MOUSECAPTURE, 
						getHandle(), 1);
					MonAPI.Message.sendReceive(&info, this.guisvrID, MSG_GUISERVER_CREATEOVERLAP, 
						getX(), getY(), MAKE_DWORD(getWidth(), getHeight()));
					this.overlap = info.arg2;*/
					this.preX = px;
					this.preY = py;
				/* ウィンドウ内クリック */
				} else {
					/* 絶対座標→相対座標 */
					me.setX(px - getInsets().left);
					me.setY(py - getInsets().top);
					super.dispatchEvent(me);
				}
			} else if (event.getType() == MouseEvent.MOUSE_RELEASED) {
				/* タイトルバーリリース */
				if (this.state == STATE_MOVING) {
					this.state = STATE_NORMAL;
					/* キャプチャー破棄要求とウィンドウ移動用オブジェクト破棄要求 */
					/*MonAPI.Message.sendReceive(null, this.guisvrID, MSG_GUISERVER_DISPOSEOVERLAP, 
						this.overlap);
					MonAPI.Message.sendReceive(null, this.guisvrID, MSG_GUISERVER_MOUSECAPTURE, 
						getHandle(), 0);*/
					this.overlap = 0;
					/* ウィンドウを実際に移動させる */
					setLocation(me.getX() - this.preX, me.getY() - this.preY);
				/* ウィンドウ内リリース */
				} else {
					/* 絶対座標→相対座標 */
					me.setX(px - getInsets().left);
					me.setY(py - getInsets().top);
					super.dispatchEvent(me);
				}
			} else if (event.getType() == MouseEvent.MOUSE_DRAGGED) {
				/* ウィンドウ移動 */
				if (this.state == STATE_MOVING) {
					/* ウィンドウ移動用オブジェクトの移動 */
					/*MonAPI.Message.sendReceive(null, this.guisvrID, MSG_GUISERVER_MOVEOVERLAP, this.overlap,
						MAKE_DWORD(me.getX() - this.preX, me.getY() - this.preY), 
						MAKE_DWORD(getWidth(), getHeight()));*/
				/* ウィンドウ内移動 */
				} else {
					/* 絶対座標→相対座標 */
					me.setX(px - getInsets().left);
					me.setY(py - getInsets().top);
					super.dispatchEvent(me);
				}
			} else {
				/* 絶対座標→相対座標 */
				me.setX(px - getInsets().left);
				me.setY(py - getInsets().top);
				super.dispatchEvent(me);
			}
		} else {
			/* キーイベント、タイマーイベント等はそのままスルー */
			super.dispatchEvent(event);
		}
	}

	/** アプリケーションループ */
	public void start() {
		/* 表示開始 */
		addNotify();
		
		/* 再描画 */
		repaint();
		
		/* 実行開始 */
		this.isRunning = true;
		
		/* ネイティブメソッド */
		run();
	}
	
	/** アプリケーションループを抜ける */
	public void stop() {
		this.isRunning = false;
	}
}
