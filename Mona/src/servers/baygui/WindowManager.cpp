/*
Comyright (c) 2004 bayside
All rights reserved.

Redistribution and use in source and binarmy forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above comyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binarmy form must reproduce the above comyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. The name of the author mamy not be used to endorse or promote products
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

#include <baygui.h>
#include "WindowManager.h"

/** インスタンス */
WindowManager *WindowManager::instance = NULL;

/** コンストラクタ */
WindowManager::WindowManager()
{
	isRunning = false;
	_x = _y = modifiers = 0;
	_width = _g->getWidth();
	_height = _g->getHeight();
	_g->translate(0, 0);
	_g->setClip(0, 0, _width, _height);
}

/** デストラクタ */
WindowManager::~WindowManager()
{
	// 全てのウィンドウを殺す
	for (int i = 0; i < _controlList->getLength(); i++) {
		Control *control = (Control *)_controlList->get(i);
		// 削除メッセージを投げる
		if (MonAPI::Message::send(control->getHandle(), MSG_GUISERVER_REMOVE, 0, 0, 0, NULL)) {
			//printf("WindowManager->Window: MSG_GUISERVER_REMOVE failed %d\n", controlList[i]->getHandle());
		} else {
			//printf("WindowManager->Window: MSG_GUISERVER_REMOVE sended %d\n", controlList[i]->getHandle());
		}
	}
	
	// キーサーバーから自分自身の登録を抹消する
	monapi_register_to_server(ID_KEYBOARD_SERVER, MONAPI_FALSE);

	// マウスサーバーから自分自身の登録を抹消する
	monapi_register_to_server(ID_MOUSE_SERVER, MONAPI_FALSE);

	// シェルサーバーに終了メッセージを投げる
	if (MonAPI::Message::send(shellsvrID, MSG_PROCESS_TERMINATED, _handle, 0, 0, NULL)) {
		//printf("baygui: ShellServer unregist error %d\n", _handle);
	} else {
		//printf("baygui: ShellServer unregistered %d\n", _handle);
	}
}

/** インスタンスを得る */
WindowManager *WindowManager::getInstance() {
	if (instance == NULL) {
		instance = new WindowManager();
	}
	return instance;
}

/**
 * ハードウェア固有のキーコードをBayGUI用に変換する
 */
int WindowManager::getKeycode(int keycode, int mod, int charcode)
{
	int key = 0;
	
	// 修飾キーの判別
	if ((mod & KEY_MODIFIER_DOWN) == KEY_MODIFIER_DOWN) {
		if ((mod & KEY_MODIFIER_SHIFT) == KEY_MODIFIER_SHIFT) {
			modifiers = VKEY_LSHIFT;
		} else if ((mod & KEY_MODIFIER_ALT) == KEY_MODIFIER_ALT) {
			modifiers = VKEY_ALT;
		} else if ((mod & KEY_MODIFIER_CTRL) == KEY_MODIFIER_CTRL) {
			modifiers = VKEY_CTRL;
		}
	}
	if ((mod & KEY_MODIFIER_UP) == KEY_MODIFIER_UP) {
		modifiers = 0;
	}
	
	// それ以外のキーの判別
	if (keycode == 33) {
		key = VKEY_PGUP;
	} else if (keycode == 34) {
		key = VKEY_PGDOWN;
	} else if (keycode == 36) {
		key = VKEY_HOME;
	} else if (keycode == 35) {
		key = VKEY_END;
	} else if (keycode == 38 || keycode == 104) {
		key = VKEY_UP;
	} else if (keycode == 40 || keycode == 98) {
		key = VKEY_DOWN;
	} else if (keycode == 37 || keycode == 100) {
		key = VKEY_LEFT;
	} else if (keycode == 39 || keycode == 102) {
		key = VKEY_RIGHT;
	} else if (keycode == 45) {
		key = VKEY_INSERT;
	} else if (keycode == 13) {
		key = VKEY_ENTER;
	} else if (VKEY_ENTER == 9) {
		key = VKEY_TAB;
	} else if (keycode == 8) {
		key = VKEY_BACKSPACE;
	} else if (keycode == 46) {
		key = VKEY_DELETE;
	} else {
		key = charcode;
	}
	
	return key;
}

/**
 キーイベント
 @param keycode キーコード
 @param mod 修飾キー
 */
void WindowManager::onKeyPress(int keycode, int mod, int charcode)
{
	//printf("keycode = %d\n", keycode);
	// キーコード判別
	keycode = getKeycode(keycode, mod, charcode);
	
	// イベント発生
	if (keycode != 0) {
		Control *control = (Control *)_controlList->getLast();
		//KeyEvent *event = new KeyEvent(KEY_PRESSED, control, keycode, 0);
		//control->postEvent(event);
		//delete(event);
		// キーイベントを投げる
		if (MonAPI::Message::send(control->getHandle(), MSG_GUISERVER_ONKEYPRESS, keycode, this->modifiers, charcode, NULL)) {
			//printf("WindowManager->Window: MSG_GUISERVER_ONKEYPRESS failed %d\n", control->getHandle());
		} else {
			//printf("WindowManager->Window: MSG_GUISERVER_ONKEYPRESS sended %d\n", control->getHandle());
		}
	}
}

/**
 キーイベント
 @param keycode キーコード
 @param mod 修飾キー
 */
void WindowManager::onKeyRelease(int keycode, int mod, int charcode)
{
	//printf("keycode = %d\n", keycode);
	// キーコード判別
	keycode = getKeycode(keycode, mod, charcode);
	
	// イベント発生
	if (keycode != 0) {
		Control *control = (Control *)_controlList->getLast();
		//KeyEvent *event = new KeyEvent(KEY_RELEASED, control, keycode, 0);
		//control->postEvent(event);
		//delete(event);
		// キーイベントを投げる
		if (MonAPI::Message::send(control->getHandle(), MSG_GUISERVER_ONKEYRELEASE, keycode, this->modifiers, charcode, NULL)) {
			//printf("WindowManager->Window: MSG_GUISERVER_ONKEYRELEASE failed %d\n", control->getHandle());
		} else {
			//printf("WindowManager->Window: MSG_GUISERVER_ONKEYRELEASE sended %d\n", control->getHandle());
		}
	}
}

/**
 マウスイベント
 */
void WindowManager::onMousePress(int mx, int my)
{
	// モード設定
	state = STATE_NORMAL;
	Control *control = findChild(mx, my);
	
	// ウィンドウが一つもないときはイベントを送らない
	if (control == NULL) return;
	
	if (control->getFocused() == true) {
		Rect *rect = control->getRect();
		// ウィンドウを閉じる
		if (rect->x + 4 <= mx && mx <= rect->x + 4 + 13 && 
			rect->y + 5 <= my && my <= rect->y + 5 + 13)
		{
			// ランチャーは殺さない
			if (control->getHandle() != launcherID) {
				remove(control);
			}
			
		// アイコン化
		} else if (rect->x + rect->width - 16 <= mx && 
			mx <= rect->x + rect->width - 16 + 13 && 
			rect->y + 5 <= my && my <= rect->y + 5 + 13)
		{
			// 描画が必要かどうかチェックする領域
			Rect srect;
			srect.x = rect->x;
			srect.y = rect->y;
			srect.width = rect->width;
			srect.height = rect->height;
			
			// ウィンドウを非アイコン化
			if (control->getIconified() == true) {
				// __g->setLocked(false) -> postEvent(DEICONIFIED) -> iconified = false -> repaint()
				postIconifiedToWindow(false, control);
			// ウィンドウをアイコン化
			} else {
				// 背景を塗りつぶす
				restoreBackGround(control);
				
				// ウィンドウ再描画
				postRepaintToWindows(_controlList->getLength() - 1, &srect);
				
				// ウィンドウをアイコン化
				// __g->setLocked(true) -> postEvent(ICONIFIED) -> iconified = true -> repaint()
				postIconifiedToWindow(true, control);
			}
			
			// 再描画メッセージを投げる
			postRepaintToWindow(control);
			
		// クリックイベント発生
		} else if (rect->y + INSETS_TOP < my && control->getIconified() == false) {
			//MouseEvent *event = new MouseEvent(MOUSE_PRESSED, control, mx, my);
			//control->postEvent(event);
			//delete(event);
			// マウスイベントを投げる
			if (MonAPI::Message::send(control->getHandle(), MSG_GUISERVER_ONMOUSEPRESS, mx, my, 0, NULL)) {
				//printf("WindowManager->Window: MSG_GUISERVER_ONMOUSEPRESS failed %d\n", control->getHandle());
			} else {
				//printf("WindowManager->Window: MSG_GUISERVER_ONMOUSEPRESS sended %d\n", control->getHandle());
			}
			//printf("throw mouse_press event\n");
		}
	// アクティブウィンドウを切り替える
	} else {
		// ウィンドウ並び替え
		_controlList->sort(control);
		
		LinkedItem *item = _controlList->endItem->prev;
		if (item != NULL) {
			Control *prevControl = (Control *)item->data;
			
			// フォーカスアウトメッセージを投げる
			// __g->setLocked(true) -> postEvent(FOCUS_OUT) -> focused = false -> repaint()
			postFocusedToWindow(false, prevControl);
			
			// 再描画メッセージを投げる
			postRepaintToWindow(prevControl);
		}
		
		Control *c = (Control *)_controlList->getLast();
		
		// フォーカスインメッセージを投げる
		// __g->setLocked(false) -> postEvent(FOCUS_IN) -> focused = true -> repaint()
		postFocusedToWindow(true, c);
	}
}

/**
 マウスイベント
 */
void WindowManager::onMouseDrag(int mx, int my)
{
	Control *control = (Control *)_controlList->getLast();

	// ウィンドウが一つもないときはイベントを送らない
	if (control == NULL) return;
	
	Rect *rect = control->getRect();
	if (state == STATE_NORMAL) {
		// 境界チェック
		if (mx <= 0) mx = 0;
		if (mx >= _width) mx = _width;
		if (my <= 0) my = 0;
		if (my >= _height) my = _height;
		
		// ウィンドウ移動開始
		if (rect->x <= mx && mx <= rect->x + rect->width && 
			rect->y <= my && my <= rect->y + INSETS_TOP)
		{
			// モード設定
			state = STATE_MOVING;
			
			// ドラッグ開始位置を記憶する
			dX = mx - rect->x;
			dY = my - rect->y;
			
			_g->setXORMode(true);
			_g->setColor(255,255,255);
			if (control->getIconified() == false) {
				_g->drawRect(rect->x, rect->y, rect->width - 1, rect->height - 1);
			} else {
				_g->drawRect(rect->x, rect->y, rect->width - 1, INSETS_TOP - 1);
			}
			_g->setXORMode(false);
			
			preX = rect->x;
			preY = rect->y;
			
			//printf("move window start: %d,%d\n", preX, preY);
		// ドラッグイベント発生
		} else if (control->getIconified() == false) {
			//MouseEvent *event = new MouseEvent(MOUSE_DRAGGED, control, mx, my);
			//control->postEvent(event);
			//delete(event);
			// マウスイベントを投げる
			//if (MonAPI::Message::send(control->getHandle(), MSG_GUISERVER_ONMOUSEDRAG, mx, my, 0, NULL)) {
				//printf("WindowManager->Window: MSG_GUISERVER_ONMOUSEDRAG failed %d\n", control->getHandle());
			//} else {
				//printf("WindowManager->Window: MSG_GUISERVER_ONMOUSEDRAG sended %d\n", control->getHandle());
			//}
			//printf("throw mouse_drag event\n");
		}
	// ウィンドウを移動する
	} else if (state == STATE_MOVING) {
		// 境界チェック
		if (mx - dX < 0) mx = dX;
		if (mx - dX + rect->width + 1 >= _width) mx = _width + dX - rect->width - 1;
		if (my - dY < 22) my = 22 + dY;
		if (my - dY + rect->height + 1 >= _height) my = _height + dY - rect->height - 1;
		
		_g->setXORMode(true);
		_g->setColor(255,255,255);
		if (control->getIconified() == false) {
			_g->drawRect(preX, preY, rect->width - 1, rect->height - 1);
			_g->drawRect(mx - dX, my - dY, rect->width - 1, rect->height - 1);
		} else {
			_g->drawRect(preX, preY, rect->width - 1, INSETS_TOP - 1);
			_g->drawRect(mx - dX, my - dY, rect->width - 1, INSETS_TOP - 1);
		}
		_g->setXORMode(false);
		
		preX = mx - dX;
		preY = my - dY;
	} else {
		// NOP
	}
}

/**
 マウスイベント
 */
void WindowManager::onMouseRelease(int mx, int my)
{
	Control *control = (Control *)_controlList->getLast();
	
	// ウィンドウが一つもないときはイベントを送らない
	if (control == NULL) return;
	
	// ウィンドウ移動
	if (state == STATE_MOVING) {
		Rect *rect = control->getRect();
		// debug
		//printf("moved: %d, %d\n", (rect->x + mx + preX), (rect->y + my + preY));
		
		// フォーカスアウトメッセージを投げる
		// __g->setLocked(true) -> postEvent(FOCUS_OUT) -> focused = false -> repaint()
		postFocusedToWindow(false, control);
		
		// 背景を塗りつぶす
		restoreBackGround(control);
		
		// ４隅再描画
		restoreCorner();
		
		// 描画が必要かどうかチェックする領域
		Rect srect;
		srect.x = rect->x;
		srect.y = rect->y;
		srect.width = rect->width;
		srect.height = rect->height;
		
		// ウィンドウ移動
		control->setRect(
			preX,
			preY,
			rect->width,
			rect->height
		);
		
		// 領域変更メッセージを投げる
		if (MonAPI::Message::send(control->getHandle(), MSG_GUISERVER_SETRECT, 
			(preX << 16 | preY), (rect->width << 16 | rect->height), 0, NULL)) {
			//printf("WindowManager->Window: MSG_GUISERVER_SETRECT failed %d\n", control->getHandle());
		} else {
			//printf("WindowManager->Window: MSG_GUISERVER_SETRECT sended %d\n", control->getHandle());
		}
		
		// ウィンドウ再描画
		postRepaintToWindows(_controlList->getLength() - 1, &srect);
		
		// フォーカスインメッセージを投げる
		// __g->setLocked(false) -> postEvent(FOCUS_IN) -> focused = true -> repaint()
		postFocusedToWindow(true, control);
		
		//printf("move window end: %d,%d\n", preX, preY);
	} else  if (control->getIconified() == false) {
		//MouseEvent *event = new MouseEvent(MOUSE_RELEASED, control, mx, my);
		//control->postEvent(event);
		//delete(event);
		// マウスイベントを投げる
		if (MonAPI::Message::send(control->getHandle(), MSG_GUISERVER_ONMOUSERELEASE, mx, my, 0, NULL)) {
			//printf("WindowManager->Window: MSG_GUISERVER_ONMOUSERELEASE failed %d\n", control->getHandle());
		} else {
			//printf("WindowManager->Window: MSG_GUISERVER_ONMOUSERELEASE sended %d\n", control->getHandle());
		}
		//printf("throw mouse_up event\n");
	}

	// 状態を元に戻す
	state = STATE_NORMAL;
}

/**
 指定したウィンドウを追加する.
 @param control 指定するウィンドウ
 */
void WindowManager::add(Control *control)
{
	// NULLチェック
	if (control == NULL) return;
	
	// ウィンドウ追加
	_controlList->add(control);
	
	if (_controlList->endItem->prev != NULL) {
		Control *c = (Control *)_controlList->endItem->prev->data;
		// フォーカスアウトメッセージを投げる
		// __g->setLocked(true) -> postEvent(FOCUS_OUT) -> focused = false -> repaint()
		postFocusedToWindow(false, c);
		
		// 再描画メッセージを投げる
		postRepaintToWindow(c);
	}
	
	// フォーカスインメッセージを投げる
	//postFocusedToWindow(true, control);
	control->setFocused(true);
}

/**
 指定したウィンドウを削除する
 @param control 指定するウィンドウ
 */
void WindowManager::remove(Control *control)
{
	// NULLチェック
	if (control == NULL) return;
	
	// 描画が必要かどうかチェックする領域
	Rect *rect = control->getRect();
	Rect srect;
	srect.x = rect->x;
	srect.y = rect->y;
	srect.width = rect->width;
	srect.height = rect->height;
	
	// 削除メッセージを投げる
	if (MonAPI::Message::send(control->getHandle(), MSG_GUISERVER_REMOVE, 0, 0, 0, NULL)) {
		//printf("WindowManager->Window: MSG_GUISERVER_REMOVE failed %d\n", control->getHandle());
	} else {
		//printf("WindowManager->Window: MSG_GUISERVER_REMOVE sended %d\n", control->getHandle());
	}
	
	// 背景を塗りつぶす
	restoreBackGround(control);
	
	// ４隅再描画
	restoreCorner();

	// ウィンドウ削除
	_controlList->remove(control);
	
	// NULLチェック
	if (_controlList->endItem == NULL) return;
	
	// フォーカスアウトメッセージを投げる
	// __g->setLocked(true) -> postEvent(FOCUS_OUT) -> focused = false -> repaint()
	postFocusedToWindows(false, _controlList->getLength() - 1);
	
	// 再描画メッセージを投げる
	postRepaintToWindows(_controlList->getLength() - 1, &srect);
	
	Control *c = (Control *)_controlList->getLast();
	
	// フォーカスインメッセージを投げる
	// __g->setLocked(false) -> postEvent(FOCUS_IN) -> focused = true -> repaint()
	postFocusedToWindow(true, c);
}

/**
 指定したウィンドウを非活性/活性化する.
 <ul>
 <li>enabledがかわる
 <li>非活性/活性化メッセージをウィンドウに投げる
 </ul>
 @param activated true/false
 */
void WindowManager::postEnabledToWindow(bool enabled, Control *control)
{
	// NULLチェック
	if (control == NULL) return;

	control->setEnabled(enabled);
	if (enabled == true) {
		MessageInfo info;
		if (MonAPI::Message::sendReceive(&info, control->getHandle(), MSG_GUISERVER_ENABLED, 0, 0, 0, NULL)) {
			//printf("WindowManager->Window: MSG_GUISERVER_ENABLED failed %d\n", control->getHandle());
		} else {
			//printf("WindowManager->Window: MSG_GUISERVER_ENABLED sended %d\n", control->getHandle());
		}
	} else {
		MessageInfo info;
		if (MonAPI::Message::sendReceive(&info, control->getHandle(), MSG_GUISERVER_DISABLED, 0, 0, 0, NULL)) {
			//printf("WindowManager->Window: MSG_GUISERVER_DISABLED failed %d\n", control->getHandle());
		} else {
			//printf("WindowManager->Window: MSG_GUISERVER_DISABLED sended %d\n", control->getHandle());
		}
	}
}

/**
 指定したウィンドウをフォーカスアウト/フォーカスインする.
 <ul>
 <li>focusedがかわる
 <li>フォーカスアウト/フォーカスインメッセージをウィンドウに投げる
 </ul>
 @param activated true/false
 */
void WindowManager::postFocusedToWindow(bool focused, Control *control)
{
	// NULLチェック
	if (control == NULL) return;

	control->setFocused(focused);
	if (focused == true) {
		MessageInfo info;
		if (MonAPI::Message::sendReceive(&info, control->getHandle(), MSG_GUISERVER_FOCUSED, 0, 0, 0, NULL)) {
			//printf("WindowManager->Window: MSG_GUISERVER_FOCUSED failed %d\n", control->getHandle());
		} else {
			//printf("WindowManager->Window: MSG_GUISERVER_FOCUSED sended %d\n", control->getHandle());
		}
	} else {
		MessageInfo info;
		if (MonAPI::Message::sendReceive(&info, control->getHandle(), MSG_GUISERVER_DEFOCUSED, 0, 0, 0, NULL)) {
			//printf("WindowManager->Window: MSG_GUISERVER_DEFOCUSED failed %d\n", control->getHandle());
		} else {
			//printf("WindowManager->Window: MSG_GUISERVER_DEFOCUSED sended %d\n", control->getHandle());
		}
	}
}

/**
 指定したウィンドウをフォーカスアウト/フォーカスインする.
 <ul>
 <li>focusedがかわる
 <li>フォーカスアウト/フォーカスインメッセージをウィンドウに投げる
 </ul>
 @param activated true/false
 @param length 0番目からlength番目までのウィンドウを対象にする
 */
void WindowManager::postFocusedToWindows(bool focused, int length)
{
	for (int i = 0; i < length; i++) {
		Control *control = (Control *)_controlList->get(i);
		postFocusedToWindow(focused, control);
	}
}

/**
 指定したウィンドウを非アイコン/アイコン化する.
 <ul>
 <li>iconifiedがかわる
 <li>非アイコン/アイコン化メッセージをウィンドウに投げる
 </ul>
 @param activated true/false
 */
void WindowManager::postIconifiedToWindow(bool iconified, Control *control)
{
	// NULLチェック
	if (control == NULL) return;

	control->setIconified(iconified);
	if (iconified == true) {
		MessageInfo info;
		if (MonAPI::Message::sendReceive(&info, control->getHandle(), MSG_GUISERVER_ICONIFIED, 0, 0, 0, NULL)) {
			//printf("WindowManager->Window: MSG_GUISERVER_ICONIFIED failed %d\n", control->getHandle());
		} else {
			//printf("WindowManager->Window: MSG_GUISERVER_ICONIFIED sended %d\n", control->getHandle());
		}
	} else {
		MessageInfo info;
		if (MonAPI::Message::sendReceive(&info, control->getHandle(), MSG_GUISERVER_DEICONIFIED, 0, 0, 0, NULL)) {
			//printf("WindowManager->Window: MSG_GUISERVER_DEICONIFIED failed %d\n", control->getHandle());
		} else {
			//printf("WindowManager->Window: MSG_GUISERVER_DEICONIFIED sended %d\n", control->getHandle());
		}
	}
}

/**
 指定したウィンドウを再描画する.
 <ul>
 <li>再描画メッセージをウィンドウに投げる
 </ul>
 */
void WindowManager::postRepaintToWindow(Control *control)
{
	// NULLチェック
	if (control == NULL) return;

	// 再描画メッセージを投げる
	MessageInfo info;
	if (MonAPI::Message::sendReceive(&info, control->getHandle(), MSG_GUISERVER_REPAINT, 0, 0, 0, NULL)) {
		//printf("WindowManager->Window: MSG_GUISERVER_REPAINT failed %d\n", control->getHandle());
	} else {
		//printf("WindowManager->Window: MSG_GUISERVER_REPAINT sended %d\n", control->getHandle());
	}
}

/**
 指定したウィンドウを再描画する.
 <ul>
 <li>再描画メッセージをウィンドウに投げる
 <li>drectに重なっていないウィンドウは描画対象外にする
 </ul>
 @param length 0番目からlength番目までのウィンドウを対象にする
 @param drect 描画が必要かチェックする領域
 */
void WindowManager::postRepaintToWindows(int length, Rect *drect)
{
	// 再描画メッセージを投げる
	for (int i = 0; i < length; i++) {
		Control *control = (Control *)_controlList->get(i);
		if (checkNeedsPaint(control->getRect(), drect) == true) {
			postRepaintToWindow(control);
		}
	}
}

/**
 指定したウィンドウの背景を復元する
 @param control NULLのときは画面全体を更新する
 */
void WindowManager::restoreBackGround(Control *control)
{
	#if 1
	if (control == NULL) {
		// 背景を塗りつぶす
		if (wallpaper->getData() != NULL) {
			_g->drawImage(wallpaper, 0, 0);
		} else {
			_g->setColor(128,128,255);
			_g->fillRect(0, 0, _width, _height);
		}
	} else {
		Rect *rect = control->getRect();
		// 背景を塗りつぶす
		if (wallpaper->getData() != NULL) {
			_g->drawImage(wallpaper, rect->x, rect->y, rect->width + 1, rect->height + 1);
		} else {
			_g->setColor(128,128,255);
			_g->fillRect(rect->x, rect->y, rect->width + 1, rect->height + 1);
		}
	}
	#else
	if (control == NULL) {
		_g->setColor(255,255,255);
		_g->fillRect(0, 0, _width, _height);
	} else {
		Rect *rect = control->getRect();
		_g->setColor(255,255,255);
		_g->fillRect(rect->x, rect->y, rect->width + 1, rect->height + 1);
	}
	#endif
}

/** メニューバー再描画 */
void WindowManager::restoreMenu()
{
	int i, j;
	
	_g->setColor(200,200,200);
	_g->fillRect(0,0,_width,20);
	_g->setColor(128,128,128);
	_g->drawLine(0,20,_width-1,20);
	_g->setColor(0,0,0);
	_g->drawLine(0,21,_width-1,21);

	// オレンジアイコン
#if defined(PEKOE)
	for (i = 0; i < 15; i++) {
		for (j = 0; j < 15; j++) {
			_g->drawPixel(19 + j, 4 + i, orangeIcon[i][j]);
		}
	}
#elif defined(MONA)
	for (i = 0; i < 15; i++) {
		for (j = 0; j < 16; j++) {
			_g->drawPixel(18 + j, 4 + i, monaIcon[i][j]);
		}
	}
#endif

	// メニュー
	int fh = FontMetrics::getInstance()->getHeight();
	_g->setColor(128,128,128);
	_g->drawText(WINDOWMANAGER_MENU_TITLE_JP, 45, 4 + (16 - fh) / 2);
}

/** ４隅再描画 */
void WindowManager::restoreCorner()
{
	_g->setColor(0,0,0);
	
	// 左上
	_g->drawLine(0,0,4,0);
	_g->drawLine(0,1,2,1);
	_g->drawLine(0,2,1,2);
	_g->drawLine(0,3,0,3);
	_g->drawLine(0,4,0,4);
	
	// 右上
	_g->drawLine(_width-5,0,_width-1,0);
	_g->drawLine(_width-3,1,_width-1,1);
	_g->drawLine(_width-2,2,_width-1,2);
	_g->drawLine(_width-1,3,_width-1,3);
	_g->drawLine(_width-1,4,_width-1,4);

	// 左下
	_g->drawLine(0,_height-5,0,_height-5);
	_g->drawLine(0,_height-4,0,_height-4);
	_g->drawLine(0,_height-3,1,_height-3);
	_g->drawLine(0,_height-2,2,_height-2);
	_g->drawLine(0,_height-1,4,_height-1);

	// 右下
	_g->drawLine(_width-5,_height-1,_width-1,_height-1);
	_g->drawLine(_width-3,_height-2,_width-1,_height-2);
	_g->drawLine(_width-2,_height-3,_width-1,_height-3);
	_g->drawLine(_width-1,_height-4,_width-1,_height-4);
	_g->drawLine(_width-1,_height-5,_width-1,_height-5);
}

/** 再描画 */
void WindowManager::repaint()
{
	if (isRunning == false)
		return;
	if (firstpaint == false)
		firstpaint = true;

	// 背景を塗りつぶす
	restoreBackGround(NULL);
	
	// メニューバー再描画
	restoreMenu();

	// ４隅再描画
	restoreCorner();
}

/**
 スレッドスタート.
 <ul>
 <li>whileループに入るため、stop() が呼ばれる限り抜けることはない
 <li>イベントは各ウィンドウに通知される
 </ul>
*/
void WindowManager::service()
{
	MessageInfo info;
	bool isMouseClick = false;

	// すでに起動していたら実行しない
	if (isRunning == true) {
		return;
	} else {
		isRunning = true;
	}

	// スタートメッセージ
	monapi_call_mouse_set_cursor(0);
	//syscall_clear_screen();
	//syscall_set_cursor(0, 0);
	printf(WINDOWMANAGER_STARUP_MESSAGE);

	// 壁紙読み込み
	wallpaper = new Bitmap(WALLPAPER_NAME);

	// スレッドIDを得る
	_handle = MonAPI::System::getThreadID();

	// キーサーバーにキー情報をくれるように自分自身を登録するメッセージを送信
	if (monapi_register_to_server(ID_KEYBOARD_SERVER, MONAPI_TRUE) == MONAPI_FALSE) {
		isRunning = false;
	}
	
	// マウスサーバーにマウス情報をくれるように自分自身を登録するメッセージを送信
	if (monapi_register_to_server(ID_MOUSE_SERVER, MONAPI_TRUE) == MONAPI_FALSE) {
		isRunning = false;
	}

	// IMEサーバーを探す
	if ((imesvrID = MonAPI::Message::lookupMainThread(IMESERVER_NAME)) == THREAD_UNKNOWN) {
		isRunning = false;
	}
	
	// シェルサーバーを探す
	if ((shellsvrID = MonAPI::Message::lookupMainThread(SHELLSERVER_NAME)) == THREAD_UNKNOWN) {
		isRunning = false;
	}

	if (isRunning == true) {
		// 再描画
		repaint();
		// ランチャー
		monapi_call_process_execute_file(LAUNCHER_NAME, MONAPI_FALSE);
	}

	// draw mouse_cursor
	monapi_call_mouse_set_cursor(1);

	while (isRunning == true) {
		if(!MonAPI::Message::receive(&info)){
		//if(!MonAPI::Message::peek(&info, 0, PEEK_REMOVE)){
			switch(info.header){
			case MSG_KEY_VIRTUAL_CODE:
				if (info.arg2 & KEY_MODIFIER_DOWN) {
					// ESCで終了
					if (info.arg1 == 27) {
						isRunning = false;
						break;
					}
				}
				// ウィンドウが一つもないときはイベントを送らない
				if (_controlList->endItem != NULL) {
					if (info.arg2 & KEY_MODIFIER_DOWN) {
						onKeyPress(info.arg1, info.arg2, info.arg3);
					} else if (info.arg2 & KEY_MODIFIER_UP) {
						onKeyRelease(info.arg1, info.arg2, info.arg3);
					}
				}
				break;
			case MSG_MOUSE_INFO:
				monapi_call_mouse_set_cursor(0);
				// ウィンドウが一つもないときはイベントを送らない
				if (_controlList->endItem != NULL) {
					if(info.arg3 != 0){
						// press
						if(isMouseClick == false){
							isMouseClick = true;
							onMousePress(info.arg1, info.arg2);
						// drag
						}else{
							onMouseDrag(info.arg1, info.arg2);
						}
					}else{
						// release
						if(isMouseClick == true){
							isMouseClick = false;
							onMouseRelease(info.arg1, info.arg2);
						}
					}
				}
				monapi_call_mouse_set_cursor(1);
				break;
			case MSG_GUISERVER_ADD:
				{
					Control *control = new Control();
					control->setHandle(info.arg1);
					// ランチャーID登録
					if (_controlList->endItem == NULL) {
						launcherID = info.arg1;
					}
					int x = info.arg2 >> 16;
					int y = info.arg2 & 0xFFFF;
					int w = info.arg3 >> 16;
					int h = info.arg3 & 0xFFFF;
					control->setRect(x, y, w, h);
					//printf("Window->WindowManager MSG_GUISERVER_ADD received %d, %d, %d, %d, %d\n", info.arg1, x, y, w, h);
					add(control);
				}
				//MonAPI::Message::reply(&info);
				break;
			case MSG_GUISERVER_REMOVE:
				//printf("Window->WindowManager MSG_GUISERVER_REMOVE received %d\n", info.arg1);
				if (_controlList->endItem != NULL) {
					remove((Control *)_controlList->getLast());
				}
				break;
			case MSG_GUISERVER_STOP:
				//printf("Window->WindowManager MSG_GUISERVER_STOP received %d\n", info.arg1);
				isRunning = false;
				break;
			case MSG_GUISERVER_GETFONT:
				//printf("FontManager->WindowManager MSG_GUISERVER_GETFONT received\n");
				{
					MonAPI::Message::sendReceive(&info, imesvrID, MSG_IMESERVER_GETFONT, 0, 0, 0, NULL);
					MonAPI::Message::reply(&info, info.arg2, info.arg3);
					//monapi_cmemoryinfo *fpMemory = FontManager::getInstance()->fpMemory;
					//MonAPI::Message::reply(&info, fpMemory->Handle, fpMemory->Size);
				}
				break;
			case MSG_GUISERVER_RESTORE:
				// KUKURIを移植するのに必要
				//printf("Window->WindowManager MSG_GUISERVER_RESTORE received %d\n", info.arg1);
				if (_controlList->endItem != NULL) {
					restoreBackGround((Control *)_controlList->getLast());
				}
				MonAPI::Message::reply(&info);
				break;
			}
		}
	}
	
	// 画面クリア
	monapi_call_mouse_set_cursor(0);
	syscall_clear_screen();
	monapi_call_mouse_set_cursor(1);
	syscall_set_cursor(0, 0);
	printf(WINDOWMANAGER_SHUTDOWN_MESSAGE);
}

/** メイン */
int MonaMain(List<char*>* pekoe)
{
	WindowManager *manager = WindowManager::getInstance();
	manager->service();
	delete(manager);
	return 0;
}
