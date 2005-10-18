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

import java.awt.event.FocusEvent;

public class Component {
	/** 親部品 */
	private Container parent;
	/** 活性・非活性 */
	private boolean enabled;
	/** フォーカス有無 */
	private boolean focused;
	/** 表示・非表示 */
	private boolean visible;
	/** x（絶対座標） */
	private int x;
	/** y（絶対座標） */
	private int y;
	/** 高さ */
	private int height;
	/** 幅 */
	private int width;
	/** 背景色 */
	private int backColor;
	/** 前景色 */
	private int foreColor;
	/** フォントスタイル */
	private int fontStyle;
	/** 部品の大きさ */
	private Rectangle bounds;
	/** 描画領域 */
	private Graphics graphics;
	/** 描画バッファー */
	private Image image;
	/** フォント情報 */
	private FontMetrics metrics;

	/** フォーカスイベント */
	protected AWTEvent focusEvent;
	
	/** コンストラクタ */
	public Component() {
		this.parent = null;
		this.enabled = true;
		this.focused = false;
		this.visible = true;
		this.x = this.y = this.height = this.width = 10;
		this.backColor = Color.lightGray;
		this.foreColor = Color.black;
		this.fontStyle = Font.PLAIN;
		this.bounds = new Rectangle();
		this.graphics = null;
		this.image = null;
		this.metrics = new FontMetrics();
		this.focusEvent = new FocusEvent();
		this.focusEvent.setType(FocusEvent.FOCUS_IN);
		this.focusEvent.setSource(this);
	}

	/**
	 部品生成ハンドラ.
	 後でremoveNotify()呼ぶと再初期化できる。
	 */
	public void addNotify() {
		if (this.image != null) return;

		// 内部バッファー、描画オブジェクトの生成
		this.image = new Image(width, height);
		this.graphics = new Graphics(this.image);
	}

	/**
	 部品破棄ハンドラ.
	 後でaddNotify()を呼ぶと再初期化できる。
	*/
	public void removeNotify() {
		//delete(image);
		//delete(graphics);
		//delete(metrics);
	}

	/** イベントハンドラ */
	public void processEvent(AWTEvent event) {
	}

	/** 描画ハンドラ */
	public void paint(Graphics g) {
	}

	/** イベント処理 */
	public void dispatchEvent(AWTEvent event) {
		processEvent(event);
		// 親部品にイベントを投げる
		if (getParent() != null) {
			getParent().processEvent(event);
		}
	}

	/** 再描画 */
	public void repaint() {
		if (this.image == null) return;
		setFontStyle(this.fontStyle);
		paint(this.graphics);
		update();
	}

	/** 部品更新 */
	public void update() {
		update(getX(), getY(), getWidth(), getHeight());
	}

	/** 部品部分更新 */
	public void update(int x, int y, int w, int h) {
		/*Frame c = (Frame)getMainWindow();
		c.getGraphics().drawImage(this.image, getX(), getY());
		c.update(c.getX() + c.getInsets().left + x, c.getY() + c.getInsets().top + y, w, h);*/
	}

	/** 親部品を得る */
	public Container getParent() { return this.parent; }
	
	/** 活性・非活性を得る */
	public boolean getEnabled() { return this.enabled; }
	
	/** フォーカス状態を得る */
	public boolean getFocused() { return this.focused; }
	
	/** 表示・非表示を得る */
	public boolean getVisible() { return this.visible; }
	
	/** X座標を得る */
	public int getX() { return this.x; }
	
	/** Y座標を得る */
	public int getY() { return this.y; }
	
	/** 幅を得る */
	public int getWidth() { return this.width; }
	
	/** 高さを得る */
	public int getHeight() { return this.height; }
	
	/** 部品の大きさ */
	public Rectangle getBounds() { return this.bounds; }
	
	/** 背景色を得る */
	public int getBackground() { return this.backColor; }
	
	/** 前景色を得る */
	public int getForeground() { return this.foreColor; }
	
	/** フォントスタイルを得る */
	public int getFontStyle() { return this.fontStyle; }
	
	/** 描画オブジェクトを得る */
	public Graphics getGraphics() { return this.graphics; }
	
	/**
	 内部バッファーを得る.
	 VRAMのように直接描画したいときに用いる。
	 Graphicsクラスが持っているような便利なメソッドはない。
	 描画後update()を呼ぶことで実際の描画がされる。
	 */
	public Image getBuffer() { return this.image; }
	
	/** フォント情報を得る */
	public FontMetrics getFontMetrics() { return this.metrics; }
	
	/**
	 メインウィンドウを得る.
	 メインウィンドウとは全ての部品が乗っている一番下のウィンドウ。
	 メインウィンドウは各アプリケーションごとに必ず１つ存在している。
	 */
	public Component getMainWindow() {
		return (this.parent == null) ? this : this.parent.getMainWindow();
	}

	/**
	 活性状態を設定する
	 @param enabled 活性状態 (true / false)
	 */
	public void setEnabled(boolean enabled) {
		this.enabled = enabled;
	}

	/**
	 フォーカス状態を設定する.
	 このメソッドを呼ぶと該当部品にFOCUS_INまたはFOCUS_OUTイベントが投げられる。
	 @param focused フォーカス状態 (true / false)
	 */
	public void setFocused(boolean focused) {
		if (this.focused == true && focused == false) {
			//syscall_print("FOCUS_OUT,");
			this.focused = focused;
			this.focusEvent.setType(FocusEvent.FOCUS_OUT);
			dispatchEvent(this.focusEvent);
		} else if (this.focused == false && focused == true) {
			//syscall_print("FOCUS_IN,");
			this.focused = focused;
			this.focusEvent.setType(FocusEvent.FOCUS_IN);
			dispatchEvent(this.focusEvent);
		}
	}

	/**
	 表示状態を設定する
	 @param visible 表示状態 (true / false)
	 */
	public void setVisible(boolean visible) {
		this.visible = visible;
	}

	/**
	 大きさを設定する
	 @param x x座標
	 @param y y座標
	 @param width 幅
	 @param height 高さ
	*/
	public void setBounds(int x, int y, int width, int height) {
		this.x = x;
		this.y = y;
		this.height = height;
		this.width = width;
		this.bounds.setBounds(x, y, width, height);
	}

	/**
	 位置を変更する
	 @param x X座標
	 @param y Y座標
	*/
	public void setLocation(int x, int y) {
		if (this.x == x && this.y == y) return;
		
		this.x = x;
		this.y = y;
		this.bounds.setLocation(x, y);
	}

	/**
	 親部品を設定する
	 @param parent 親部品
	*/
	public void setParent(Container parent) {
		this.parent = parent;
	}

	/** 背景色を設定する */
	public void setBackground(int backColor) {
		this.backColor = backColor;
	}

	/** 前景色を設定する */
	public void setForeground(int foreColor) {
		this.foreColor = foreColor;
	}

	/**
	 フォントスタイル（通常、太字、斜字、固定幅）を設定する.
	 各スタイルを"|"で組み合わせることも可能。
	 @param style フォントスタイル (Font.PLAIN / Font.BOLD / Font.ITALIC / Font.FIXED )
	*/
	public void setFontStyle(int style) {
		if (this.metrics != null) {
			this.metrics.setFontStyle(style);
		}
		if (this.graphics != null) {
			this.graphics.setFontStyle(style);
		}
		this.fontStyle = style;
	}
}
