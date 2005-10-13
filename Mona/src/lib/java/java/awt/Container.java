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
import java.awt.event.KeyEvent;
import java.awt.event.MouseEvent;

/**
 コンテナークラス
*/
public class Container extends Component {
	/** 部品一覧 */
	private Vector componentList;

	/** コンストラクタ */
	public Container() {
		this.componentList = new Vector();
	}
	
	/**
	 活性部品を得る
	 @return 活性部品（なければnull）
	*/
	public Component getComponent() {
		// nullチェック
		int I = this.componentList.size();
		if (I == 0) return null;

		// 後ろからチェックしていく
		for (int i = I - 1; i >= 0; i--) {
			Component c = (Component)this.componentList.get(i);
			if (c.getFocused() == true) return c;
		}
		return null;
	}
	
	/**
	 部品を得る
	 @param x 指定するX座標
	 @param y 指定するY座標
	 */
	public Component getComponentAt(int x, int y) {
		// nullチェック
		int I = this.componentList.size();
		if (I == 0) return null;
		
		// 後ろからチェックしていく
		for (int i = I - 1; i >= 0; i--) {
			Component c = (Component)this.componentList.get(i);
			Rectangle bounds = c.getBounds();
			// マウスカーソルがある範囲に部品があるかどうかチェック
			if (bounds.x <= x && x <= bounds.x + bounds.width && 
				bounds.y <= y && y <= bounds.y + bounds.height)
			{
				return c;
			}
		}
		return null;
	}
	
	/**
	 指定した部品を追加する
	 @param component 指定する部品
	 */
	public void add(Component component) {
		component.setParent(this);
		component.addNotify();
		this.componentList.add(component);
	}
	
	/**
	 指定した部品を削除する
	 @param component 指定する部品
	 @return 削除された部品（なければnull）
	 */
	public void remove(Component component) {
		this.componentList.remove(component);
	}
	
	/** イベント処理 */
	public void dispatchEvent(AWTEvent event) {
		// 非活性の時はイベントを受け付けない
		if (getEnabled() == false) return;

		// 活性部品にキーイベントを投げる
		if (event.getType() == KeyEvent.KEY_PRESSED || event.getType() == KeyEvent.KEY_RELEASED) {
			Component component = getComponent();
			// 部品でイベントが起こった
			if (component != null) {
				event.setSource(component);
				component.processEvent(event);
			}
			// 部品以外でイベントが起こった
			processEvent(event);
		// マウスクリック
		} else if (event.getType() == MouseEvent.MOUSE_PRESSED) {
			MouseEvent me = (MouseEvent)event;
			// マウスイベントが起こった部品を探す
			Component component = getComponentAt(me.getX(), me.getY());
			// 部品でイベントが起こった
			if (component != null) {
				// イベントが起こった部品以外をフォーカスアウト状態にする
				int I = this.componentList.size();
				for (int i = 0; i < I; i++) {
					Component c = (Component)this.componentList.get(i);
					if (c != component) {
						c.setFocused(false);
					}
				}
				component.setFocused(true);
				event.setSource(component);
				Rectangle bounds = component.getBounds();
				me.setX(me.getX() - bounds.x);
				me.setY(me.getY() - bounds.y);
				//syscall_print("MOUSE_PRESSED,");
				component.processEvent(event);
			// 部品以外でイベントが起こった
			} else {
				// 部品をフォーカスアウト状態にする
				int I = this.componentList.size();
				for (int i = 0; i < I; i++) {
					Component c = (Component)this.componentList.get(i);
					c.setFocused(false);
				}
				processEvent(event);
			}
		// マウスクリック以外のマウスイベント
		} else if (event.getType() == MouseEvent.MOUSE_RELEASED || 
					event.getType() == MouseEvent.MOUSE_DRAGGED || 
					event.getType() == MouseEvent.MOUSE_MOVED)
		{
			MouseEvent me = (MouseEvent)event;
			// マウスイベントが起こった部品を探す
			Component component = getComponentAt(me.getX(), me.getY());
			// 部品でイベントが起こった
			if (component != null) {
				event.setSource(component);
				Rectangle bounds = component.getBounds();
				me.setX(me.getX() - bounds.x);
				me.setY(me.getY() - bounds.y);
				component.processEvent(event);
			// 部品以外でイベントが起こった
			} else {
				processEvent(event);
			}
		} else {
			processEvent(event);
		}
	}
	
	/** 再描画 */
	public void repaint() {
		if (getBuffer() == null) return;
		
		paint(getGraphics());

		// 自分の領域を更新する
		update();

		// 子部品を再描画する
		int I = this.componentList.size();
		for(int i = 0; i < I; i++) {
			Component component = (Component)this.componentList.get(i);
			component.repaint();
		}
	}
}
