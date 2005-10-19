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

import java.util.Vector;
import java.awt.event.FocusEvent;
import java.awt.event.ItemEvent;
import java.awt.event.KeyEvent;
import java.awt.event.MouseEvent;

/**
 リストボックスクラス
*/
public class ListBox extends Component {
	/** 選択位置 */
	private int selectedIndex;
	/** 表示開始位置 */
	private int offsetIndex;
	/** 表示可能行数 */
	private int visibleRows;
	/** データリスト */
	private Vector dataList;
	/** 選択イベント */
	private ItemEvent itemEvent;

	/** コンストラクタ */
	public ListBox() {
		this.selectedIndex = -1;
		this.offsetIndex = 0;
		this.visibleRows = 0;
		this.dataList = new Vector();
		this.itemEvent = new ItemEvent(ItemEvent.ITEM_SELECTED, this);
		setBackground(Color.white);
	}
	
	/** 選択位置を得る */
	public int getSelectedIndex() { return this.selectedIndex; }
	
	/** 選択項目を得る */
	public String getSelectedItem() {
		return dataList.get(this.selectedIndex).toString();
	}
	
	/** index 番目を選択する */
	public void select(int index) {
		if (0 <= index && index < this.dataList.size()) {
			this.selectedIndex = index;
			repaint();
		}
	}
	
	/** 項目を追加する */
	public void add(String item) {
		this.dataList.add(item);
	}
	
	/** index 番目の項目を削除する */
	public void remove(int index) {
		this.dataList.remove(index);
	}
	
	/** 描画ハンドラ */
	public void paint(Graphics g) {
		int w = getWidth();
		int h = getHeight();
		
		this.visibleRows = (h - 2) / 16;
		
		// 外枠
		if (getFocused() == true && getEnabled() == true) {
			g.setColor(0,128,255);
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
		int I = this.dataList.size();
		for (int i = 0; i < visibleRows; i++) {
			if ((this.offsetIndex + i) < I) {
				if (selectedIndex == i && getEnabled() == true) {
					g.setColor(0, 128, 255);
					g.fillRect(3, (16 * i) + 3, w - 6, 16);
					g.setColor(getBackground());
					g.drawString((String)this.dataList.get(this.offsetIndex + i), 4, (16 * i) + 6);
				} else {
					g.setColor(getForeground());
					g.drawString((String)this.dataList.get(this.offsetIndex + i), 4, (16 * i) + 6);
				}
			}
		}
	}
	
	/** イベントハンドラ */
	public void processEvent(AWTEvent event) {
		// 非活性の時はイベントを受け付けない
		if (getEnabled() == false) return;

		// キー押下
		if (event.getType() == KeyEvent.KEY_PRESSED) {
			int keycode = ((KeyEvent)event).getKeycode();
			if (keycode == KeyEvent.VKEY_UP) {
				if (this.selectedIndex > 0) {
					this.selectedIndex--;
					repaint();
					if (getParent() != null) {
						getParent().processEvent(this.itemEvent);
					}
				}
			} else if (keycode == KeyEvent.VKEY_DOWN) {
				if (this.selectedIndex < this.dataList.size() - 1 && 
					this.selectedIndex < ((getHeight() - 22) / 16))
				{
					this.selectedIndex++;
					repaint();
					if (getParent() != null) {
						getParent().processEvent(this.itemEvent);
					}
				}
			} else if (keycode == KeyEvent.VKEY_ENTER) {
				getParent().processEvent(this.itemEvent);
			}
		// マウス押下
		} else if (event.getType() == MouseEvent.MOUSE_PRESSED) {
			int my = ((MouseEvent)event).getY();
			//printf("y = %d,", my);
			select((my - 3) / 16);
			if (getParent() != null) {
				getParent().processEvent(this.itemEvent);
			}
		// フォーカス状態変更
		} else if (event.getType() == FocusEvent.FOCUS_IN || event.getType() == FocusEvent.FOCUS_OUT) {
			repaint();
			if (getParent() != null) {
				getParent().processEvent(this.itemEvent);
			}
		}
	}
}
