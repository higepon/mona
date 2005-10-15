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

import java.awt.event.TextEvent;

/**
 テキストボックスクラス
*/
public class TextField extends Component {
	/** テキストフィールド文字列最大長 */
	private static final int MAX_TEXT_LEN  = 128;
	/** テキスト選択位置 */
	private int textPtr;
	/** テキストの長さ */
	private int textLen;
	/** オフセットX座標 */
	private int offx;
	/** オフセットY座標 */
	private int offy;
	/** テキスト */
	private char[] text;
	/** IMEイベント */
	private TextEvent textEvent;
	
	/** 1文字挿入する */
	private void insertCharacter(char c) {
	}
	
	/** 一文字削除する */
	private void deleteCharacter() {
	}

	/** コンストラクタ */
	public TextField() {
		this.text = new char [MAX_TEXT_LEN];
	}
	
	/**
	 テキストを設定する
	 @param text
	 */
	public void setText(String text) {
		for (int i = 0; i < this.textLen; i++) {
			this.text[i] = '\0';
		}
		int I = text.length() > this.textLen ? this.textLen : text.length();
		for (int i = 0; i < I; i++) {
			this.text[i] = text.charAt(i);
		}
		this.textLen = I;
	}
	
	/** テキストを得る */
	public String getText() { return new String(this.text, 0, this.textLen); }
	
	/** 描画ハンドラ */
	public void paint(Graphics g) {
	}
	
	/** イベントハンドラ */
	public void processEvent(AWTEvent event) {
	}
}
