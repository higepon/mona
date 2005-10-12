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

/**
 イベント基底クラス
*/
public class AWTEvent {
	//
	// イベントタイプ一覧
	//
	/** キー押下 */
	public static final int KEY_PRESSED      = 100;
	/** キーリリース */
	public static final int KEY_RELEASED     = 101;
	/** マウスクリック */
	public static final int MOUSE_PRESSED    = 200;
	/** マウスリリース */
	public static final int MOUSE_RELEASED   = 201;
	/** マウスドラッグ */
	public static final int MOUSE_DRAGGED    = 202;
	/** マウス移動 */
	public static final int MOUSE_MOVED      = 203;
	/** フォーカスイン */
	public static final int FOCUS_IN         = 300;
	/** フォーカスアウト */
	public static final int FOCUS_OUT        = 301;
	/** タイマー */
	public static final int TIMER            = 302;
	/** テキスト確定 */
	public static final int TEXT_CHANGED     = 303;
	/** 項目選択 */
	public static final int ITEM_SELECTED    = 304;
	/** ブロック増加 */
	public static final int BLOCK_INCLEMENT  = 305;
	/** ブロック減少 */
	public static final int BLOCK_DECLEMENT  = 306;
	/** ボタン押下 */
	public static final int ACTION_PERFORMED = 307;
	/** カスタムイベント */
	public static final int CUSTOM_EVENT     = 400;
	
	/** イベントタイプ */
	private int type;
	/** イベント発生元 */
	private Component source;

	//
	// Mona用イベント
	//
	/** ヘッダー */
	public int header;
	/** 引数(1) */
	public int arg1;
	/** 引数(2) */
	public int arg2;
	/** 引数(3) */
	public int arg3;
	/** メッセージ発生元 */
	public int from;
	/** 文字列 */
	public char str[]; //128
	/** 文字列の長さ */
	public int length;

	/** デフォルトコンストラクタ */
	public AWTEvent() {
		this.type = CUSTOM_EVENT;
		this.source = NULL;
	}
	
	/**
	 コンストラクタ
	 @param type イベントタイプ
	 @param source イベント発生元部品
	 */
	public AWTEvent(int type, Component source) {
		this.type = type;
		this.source = source;
	}
	
	/** イベントタイプを得る */
	public int getType() { return this.type; }
	
	/** イベントタイプを設定する */
	public void setType(int type) { this.type = type; }
	
	/** イベントの発生部品を得る */
	public Component getSource() { return this.source; }
	
	/** イベントの発生部品を設定する */
	public void setSource(Component c) { this.source = c; }
}
