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
import java.awt.event.KeyEvent;
import java.awt.event.TextEvent;

/**
 テキストボックスクラス
*/
public class TextField extends Component {
	/** テキスト選択位置 */
	private int dataPtr;
	/** テキストの長さ */
	private int dataLen;
	/** オフセットX座標 */
	private int offx;
	/** オフセットY座標 */
	private int offy;
	/** テキスト */
	private char[] data;
	/** IMEイベント */
	private TextEvent textEvent;
	/** バッファーデフォルト長 */
	private static final int BUFFER_SIZE = 16;
	
	/** コンストラクタ */
	public TextField() {
		this.dataPtr = -1;
		this.dataLen = 0;
		this.offx = 3;
		this.offy = 3;
		this.data = new char [BUFFER_SIZE];
		this.textEvent = new TextEvent(TextEvent.TEXT_CHANGED, this);
		setBackground(Color.white);
	}
	
	/** 1文字挿入する */
	private void insertCharacter(char c) {
		// 配列拡張
		if (this.dataLen == this.data.length) {
			char[] new_text = new char[this.data.length * 2];
			for (int i = 0; i < this.dataLen; i++) {
				new_text[i] = this.data[i];
			}
			this.data = new_text;
		}
		// 1文字挿入
		if (this.dataPtr >= 0) {
			for (int i = this.dataLen; i > this.dataPtr; i--) {
				this.data[i] = this.data[i - 1];
			}
		}
		this.data[++this.dataPtr] = c;
		this.dataLen++;
	}
	
	/** 一文字削除する */
	private void deleteCharacter() {
		int i = 0;
		for (i = this.dataPtr; i < this.dataLen - 1; i++) {
			this.data[i] = this.data[i + 1];
		}
		data[i] = '\0';
		this.dataPtr--;
		this.dataLen--;
	}
	
	/**
	 テキストを設定する
	 @param text
	 */
	public void setText(String text) {
		for (int i = 0; i < this.data.length; i++) {
			this.data[i] = '\0';
		}
		this.dataPtr = -1;
		this.dataLen = 0;
		for (int i = 0; i < text.length(); i++) {
			insertCharacter(text.charAt(i));
		}
	}
	
	/** テキストを得る */
	public String getText() { return new String(this.data, 0, this.dataLen); }
	
	/** 描画ハンドラ */
	public void paint(Graphics g) {
		int w = getWidth();
		int h = getHeight();

		// 外枠
		if (getFocused() == true && getEnabled() == true) {
			g.setColor(0, 128, 255);
			g.drawRect(0, 0, w, h);
		} else {
			if (getParent() != null) {
				g.setColor(getParent().getBackground());
			} else {
				g.setColor(Color.lightGray);
			}
			g.drawRect(0, 0, w, h);
		}
		
		// 内枠
		g.setColor(getBackground());
		g.fillRect(1, 1, w - 2, h - 2);
		g.setColor(getForeground());
		g.drawRect(1, 1, w - 2, h - 2);

		// 文字
		int fh = getFontMetrics().getHeight(getText());
		if (getEnabled() == true) {
			g.setColor(getForeground());
		} else {
			g.setColor(Color.gray);
		}
		g.drawString(getText(), this.offx, (h - fh) / 2);

		// キャレット
		if (getFocused() == true && getEnabled() == true) {
			int fw = 0;
			for (int i = 0; i <= this.dataPtr; i++) {
				fw += getFontMetrics().getCharWidth(this.data[i]);
			}
			g.drawLine(offx + fw, (h - fh) / 2, offx + fw, (h - fh) / 2 + fh);
		}
	}
	
	/** イベントハンドラ */
	public void processEvent(AWTEvent event) {
		// 非活性の時はイベントを受け付けない
		if (getEnabled() == false) return;
		
		// キー押下
		if (event.getType() == KeyEvent.KEY_PRESSED) {
			int keycode = ((KeyEvent)event).getKeycode();
			if (keycode == KeyEvent.VKEY_BACKSPACE) {
				if (this.dataPtr >= 0) {
					// バックスペース
					deleteCharacter();
					repaint();
				}
			} else if (keycode == KeyEvent.VKEY_LEFT) {
				// ←移動
				if (this.dataPtr >= 0) {
					this.dataPtr--;
					repaint();
				}
			} else if (keycode == KeyEvent.VKEY_RIGHT) {
				// →移動
				if (this.dataPtr < this.dataLen - 1) {
					this.dataPtr++;
					repaint();
				}
			} else if (keycode == KeyEvent.VKEY_ENTER) {
				// 確定
				if (getParent() != null) {
					getParent().processEvent(this.textEvent);
				}
				return;
			} else if (keycode < 128) {
				// 1文字挿入
				insertCharacter((char)keycode);
				repaint();
			}
		// フォーカス状態変更
		} else if (event.getType() == FocusEvent.FOCUS_IN || event.getType() == FocusEvent.FOCUS_OUT) {
			repaint();
			if (getParent() != null) {
				getParent().processEvent(this.focusEvent);
			}
		}
	}
}
