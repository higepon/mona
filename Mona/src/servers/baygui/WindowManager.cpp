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

/** オレンジアイコン */
static int orangeIcon [15][15] = {
	{0xcccecc,0xcccecc,0xcccecc,0xcccecc,0xcccecc,0xcccecc,0xcccecc,0xcccecc,0xcccecc,0xcccecc,0xcccecc,0xcccecc,0xcccecc,0xcccecc,0xcccecc},
	{0xcccecc,0xcccecc,0xcccecc,0xcccecc,0xcccecc,0xcccecc,0xcccecc,0xcccecc,0xcccecc,0xcccecc,0x40204,0x40204,0x40204,0xcccecc,0xcccecc},
	{0xcccecc,0xcccecc,0xcccecc,0xcccecc,0xcccecc,0x40204,0x40204,0x40204,0x40204,0x40204,0x4fe04,0x4fe04,0x4fe04,0x40204,0xcccecc},
	{0xcccecc,0xcccecc,0xcccecc,0x40204,0x40204,0xfc9a04,0xfc9a04,0x40204,0x4fe04,0x4fe04,0x40204,0x40204,0x40204,0xcccecc,0xcccecc},
	{0xcccecc,0xcccecc,0x40204,0xfc9a04,0xfc9a04,0xfc9a04,0x40204,0x40204,0x40204,0x40204,0xfc9a04,0xfc9a04,0x40204,0xcccecc,0xcccecc},
	{0xcccecc,0x40204,0xfc9a04,0xfc9a04,0xfc9a04,0xfc9a04,0xfc9a04,0xfc9a04,0xfc9a04,0xfc9a04,0xfc9a04,0xfc9a04,0xfc9a04,0x40204,0xcccecc},
	{0x40204,0xfc9a04,0xfc9a04,0xfc9a04,0xfc9a04,0xfc9a04,0xfc9a04,0xfc9a04,0xfc9a04,0xfc9a04,0xfc9a04,0xfc9a04,0xfc9a04,0xfc9a04,0x40204},
	{0x40204,0xfc9a04,0xfc9a04,0xfc9a04,0xfc9a04,0xfc9a04,0xfc9a04,0xfc9a04,0xfc9a04,0xfc9a04,0xfc9a04,0xfc9a04,0xfc9a04,0xfc9a04,0x40204},
	{0x40204,0xfc9a04,0xfc9a04,0xfc9a04,0xfc9a04,0xfc9a04,0xfc9a04,0xfc9a04,0xfc9a04,0xfc9a04,0xfc9a04,0xfc9a04,0xfc9a04,0xfc9a04,0x40204},
	{0x40204,0xfc9a04,0xfc9a04,0xfc9a04,0xfc9a04,0xfc9a04,0xfc9a04,0xfc9a04,0xfc9a04,0xfc9a04,0xfc9a04,0xfc9a04,0xfc9a04,0xfc9a04,0x40204},
	{0x40204,0xfc9a04,0xfc9a04,0xfc9a04,0xfc9a04,0xfc9a04,0xfc9a04,0xfc9a04,0xfc9a04,0xfc9a04,0xfc9a04,0xfc9a04,0xfc9a04,0xfc9a04,0x40204},
	{0xcccecc,0x40204,0xfc9a04,0xfc9a04,0xfc9a04,0xfc9a04,0xfc9a04,0xfc9a04,0xfc9a04,0xfc9a04,0xfc9a04,0xfc9a04,0xfc9a04,0x40204,0xcccecc},
	{0xcccecc,0xcccecc,0x40204,0xfc9a04,0xfc9a04,0xfc9a04,0xfc9a04,0xfc9a04,0xfc9a04,0xfc9a04,0xfc9a04,0xfc9a04,0x40204,0xcccecc,0xcccecc},
	{0xcccecc,0xcccecc,0xcccecc,0x40204,0x40204,0xfc9a04,0xfc9a04,0xfc9a04,0xfc9a04,0xfc9a04,0x40204,0x40204,0xcccecc,0xcccecc,0xcccecc},
	{0xcccecc,0xcccecc,0xcccecc,0xcccecc,0xcccecc,0x40204,0x40204,0x40204,0x40204,0x40204,0xcccecc,0xcccecc,0xcccecc,0xcccecc,0xcccecc},
};

/** STDOUTを横取りする */
static void StdoutMessageLoop() {
	MonAPI::Message::send(WindowManager::getInstance()->getThreadID(), MSG_SERVER_START_OK);
	for (MessageInfo info;;) {
		if (MonAPI::Message::receive(&info) != 0) continue;
		switch (info.header) {
			case MSG_PROCESS_STDOUT_DATA: {
				//info.str[127] = '\0';
				MonAPI::Message::reply(&info);
				break;
			}
		}
	}
}

/** インスタンス */
WindowManager *WindowManager::instance = NULL;

/** コンストラクタ */
WindowManager::WindowManager()
{
	MessageInfo info;

	isRunning = false;
	controlListPtr = x = y = 0;
	MonAPI::Screen screen;
	width = screen.getWidth();
	height = screen.getHeight();
	_g->setClip(0,0,width,height);

	// 壁紙読み込み
	wallpaper = new Bitmap("/SERVERS/MONAWALL.BM2");

	// スレッドIDを得る
	threadID = MonAPI::System::getThreadID();

	// キーサーバーを探す
	keysvrID = MonAPI::Message::lookupMainThread("KEYBDMNG.EX2");
	if (keysvrID == 0xFFFFFFFF) {
		//printf("Window: KeyServer not found %d\n", threadID);
	} else {
		//printf("Window: KeyServer found %d\n", threadID);
	}
	
	// キーサーバーにキー情報をくれるように自分自身を登録するメッセージを送信
	MonAPI::Message::create(&info, MSG_KEY_REGIST_TO_SERVER, threadID, 0, 0, NULL);
	if (MonAPI::Message::send(keysvrID, &info)) {
		//printf("Window: KeyServer regist error %d\n", threadID);
	} else {
		//printf("Window: KeyServer registered %d\n", threadID);
	}

	// マウスサーバーを探す
	mousesvrID = MonAPI::Message::lookupMainThread("MOUSE.EX2");
	if (mousesvrID == 0xFFFFFFFF) {
		//printf("Window: MouseServer not found %d\n", threadID);
	} else {
		//printf("Window: MouseServer found %d\n", threadID);
	}

	// マウスサーバーにマウス情報をくれるように自分自身を登録するメッセージを送信
	MonAPI::Message::create(&info, MSG_MOUSE_REGIST_TO_SERVER, threadID, 0, 0, NULL);
	if (MonAPI::Message::send(mousesvrID, &info)) {
		//printf("Window: MouseServer regist error %d\n", threadID);
	} else {
		//printf("Window: MouseServer registered %d\n", threadID);
	}
}

/** デストラクタ */
WindowManager::~WindowManager()
{
	// 全てのウィンドウを殺す
	int i;
	for (i = 0; i < controlListPtr; i++) {
		// 削除メッセージを投げる
		if (MonAPI::Message::send(controlList[i]->getThreadID(), MSG_GUISERVER_REMOVE, 0, 0, 0, NULL)) {
			//printf("WindowManager->Window: MSG_GUISERVER_REMOVE failed %d\n", controlList[i]->getThreadID());
		} else {
			//printf("WindowManager->Window: MSG_GUISERVER_REMOVE sended %d\n", controlList[i]->getThreadID());
		}
	}
	
	// キーサーバーから自分自身の登録を抹消する
	if (MonAPI::Message::send(keysvrID, MSG_KEY_UNREGIST_FROM_SERVER, threadID)) {
		//printf("baygui: KeyServer unregist error %d\n", threadID);
	} else {
		//printf("baygui: KeyServer unregistered %d\n", threadID);
	}

	// マウスサーバーから自分自身の登録を抹消する
	if (MonAPI::Message::send(mousesvrID, MSG_MOUSE_UNREGIST_FROM_SERVER, threadID)) {
		//printf("baygui: MouseServer unregist error %d\n", threadID);
	} else {
		//printf("baygui: MouseServer unregistered %d\n", threadID);
	}

	// シェル出力を元に戻す
	#if 0
	if (procsvrID == THREAD_UNKNOWN) {
		//printf("baygui: MSG_PROCESS_UNGRAB_STDOUT failed\n");
	} else {
		//printf("baygui: MSG_PROCESS_UNGRAB_STDOUT succeed\n");
		MonAPI::Message::sendReceive(NULL, procsvrID + 1, MSG_PROCESS_UNGRAB_STDOUT, stdoutID);
		syscall_kill_thread(stdoutID);
	}
	#endif
}

/** インスタンスを得る */
WindowManager *WindowManager::getInstance() {
	if (instance == NULL) {
		instance = new WindowManager();
	}
	return instance;
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
	if (keycode == 33) {
		keycode = VKEY_PGUP;
	} else if (keycode == 34) {
		keycode = VKEY_PGDOWN;
	} else if (keycode == 36) {
		keycode = VKEY_HOME;
	} else if (keycode == 35) {
		keycode = VKEY_END;
	} else if (keycode == 38) {
		keycode = VKEY_UP;
	} else if (keycode == 40) {
		keycode = VKEY_DOWN;
	} else if (keycode == 37) {
		keycode = VKEY_LEFT;
	} else if (keycode == 39) {
		keycode = VKEY_RIGHT;
	} else if (keycode == 45) {
		keycode = VKEY_INSERT;
	} else if (keycode == 13) {
		keycode = VKEY_ENTER;
	} else if (VKEY_ENTER == 9) {
		keycode = VKEY_TAB;
	} else if (keycode == 8) {
		keycode = VKEY_BACKSPACE;
	} else if (keycode == 46) {
		keycode = VKEY_DELETE;
	} else if (keycode == 104) {
		keycode = VKEY_UP_QEMU;
	} else if (keycode == 98) {
		keycode = VKEY_DOWN_QEMU;
	} else if (keycode == 100) {
		keycode = VKEY_LEFT_QEMU;
	} else if (keycode == 102) {
		keycode = VKEY_RIGHT_QEMU;
	} else {
		keycode = charcode;
	}
	
	// イベント発生
	if (keycode != 0) {
		Control *control = (Control *)getActiveControl();
		//KeyEvent *event = new KeyEvent(KEY_PRESSED, control, keycode, 0);
		//getActiveControl()->postEvent(event);
		//delete(event);
		// キーイベントを投げる
		if (MonAPI::Message::send(control->getThreadID(), MSG_GUISERVER_ONKEYPRESS, keycode, 0, 0, NULL)) {
			//printf("WindowManager->Window: MSG_GUISERVER_ONKEYPRESS failed %d\n", control->getThreadID());
		} else {
			//printf("WindowManager->Window: MSG_GUISERVER_ONKEYPRESS sended %d\n", control->getThreadID());
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
}

/**
 マウスイベント
 */
void WindowManager::onMousePress(int mx, int my)
{
	MessageInfo info;
	
	// モード設定
	state = STATE_NORMAL;
	// ウィンドウを切り替える
	if (setActiveControl(mx, my) == false) {
		Control *control = getActiveControl(mx, my);
		// ウィンドウが一つもないときはイベントを送らない
		if (control != NULL) {
			Rect *rect = control->getRect();
			// ウィンドウを閉じる
			if (rect->x + 4 <= mx && mx <= rect->x + 4 + 13 && 
				rect->y + 5 <= my && my <= rect->y + 5 + 13)
			{
				remove(getActiveControl());
				//printf("close window: %x\n", getActiveControl());
			// アイコン化
			} else if (rect->x + rect->width - 16 <= mx && 
				mx <= rect->x + rect->width - 16 + 13 && 
				rect->y + 5 <= my && my <= rect->y + 5 + 13)
			{
				// 背景を塗りつぶす
				if (wallpaper->getData() != NULL) {
					_g->drawImage(wallpaper, rect->x, rect->y, rect->width + 1, rect->height + 1);
				} else {
					_g->setColor(128,128,255);
					_g->fillRect(rect->x, rect->y, rect->width + 1, rect->height + 1);
				}
				// ウィンドウをアイコン化
				if (control->getIconified() == true) {
					control->setIconified(false);
					// 非アイコン化メッセージを投げる
					if (MonAPI::Message::send(control->getThreadID(), MSG_GUISERVER_DEICONIFIED, 0, 0, NULL)) {
						//printf("WindowManager->Window: MSG_GUISERVER_DEICONIFIED failed %d\n", control->getThreadID());
					} else {
						//printf("WindowManager->Window: MSG_GUISERVER_DEICONIFIED sended %d\n", control->getThreadID());
					}
				} else {
					control->setIconified(true);
					// アイコン化メッセージを投げる
					if (MonAPI::Message::send(control->getThreadID(), MSG_GUISERVER_ICONIFIED, 0, 0, 0, NULL)) {
						//printf("WindowManager->Window: MSG_GUISERVER_ICONIFIED failed %d\n", control->getThreadID());
					} else {
						//printf("WindowManager->Window: MSG_GUISERVER_ICONIFIED sended %d\n", control->getThreadID());
					}
				}
				// ウィンドウ再描画
				for (int i = 0; i < controlListPtr; i++) {
					//controlList[i]->repaint();
					// 再描画メッセージを投げる
					if (MonAPI::Message::sendReceive(&info, controlList[i]->getThreadID(), MSG_GUISERVER_REPAINT, 0, 0, 0, NULL)) {
						//printf("WindowManager->Window: MSG_GUISERVER_REPAINT failed %d\n", controlList[i]->getThreadID());
					} else {
						//printf("WindowManager->Window: MSG_GUISERVER_REPAINT sended %d\n", controlList[i]->getThreadID());
					}
				}
			// クリックイベント発生
			} else if (control->getIconified() == false) {
				//MouseEvent *event = new MouseEvent(MOUSE_PRESSED, control, mx, my);
				//control->postEvent(event);
				//delete(event);
				// マウスイベントを投げる
				if (MonAPI::Message::send(control->getThreadID(), MSG_GUISERVER_ONMOUSEPRESS, mx, my, 0, NULL)) {
					//printf("WindowManager->Window: MSG_GUISERVER_ONMOUSEPRESS failed %d\n", control->getThreadID());
				} else {
					//printf("WindowManager->Window: MSG_GUISERVER_ONMOUSEPRESS sended %d\n", control->getThreadID());
				}
				//printf("throw mouse_press event\n");
			}
		}
	} else {
		//printf("switch window\n");
	}
}

/**
 マウスイベント
 */
void WindowManager::onMouseDrag(int mx, int my)
{
	if (mx <= 0) mx = 0;
	if (mx >= width) mx = width;
	if (my <= 0) my = 0;
	if (my >= height) my = height;
	Control *control = getActiveControl(mx, my);
	// ウィンドウが一つもないときはイベントを送らない
	if (control != NULL) {
		Rect *rect = control->getRect();
		// ウィンドウを移動する
		if (rect->x <= mx && mx <= rect->x + rect->width && 
			rect->y <= my && my <= rect->y + INSETS_TOP)
		{
			if (state == STATE_NORMAL) {
				// ドラッグ開始位置を記憶する
				preX = mx;
				preY = my;
				// モード設定
				state = STATE_MOVING;
				//printf("move window start: %d,%d\n", preX, preY);
			}
		// ドラッグイベント発生
		} else if (control->getIconified() == false) {
			//MouseEvent *event = new MouseEvent(MOUSE_DRAGGED, control, mx, my);
			//control->postEvent(event);
			//delete(event);
			// マウスイベントを投げる
			if (MonAPI::Message::send(control->getThreadID(), MSG_GUISERVER_ONMOUSEDRAG, mx, my, 0, NULL)) {
				//printf("WindowManager->Window: MSG_GUISERVER_ONMOUSEDRAG failed %d\n", control->getThreadID());
			} else {
				//printf("WindowManager->Window: MSG_GUISERVER_ONMOUSEDRAG sended %d\n", control->getThreadID());
			}
			//printf("throw mouse_drag event\n");
		}
	}
}

/**
 マウスイベント
 */
void WindowManager::onMouseRelease(int mx, int my)
{
	MessageInfo info;

	Control *control = getActiveControl();
	// ウィンドウが一つもないときはイベントを送らない
	if (control != NULL) {
		Rect *rect = control->getRect();
		// ウィンドウ移動
		if (state == STATE_MOVING) {
			// debug
			//printf("moved: %d, %d\n", (rect->x + mx + preX), (rect->y + my + preY));
			// 背景を塗りつぶす
			if (wallpaper->getData() != NULL) {
				_g->drawImage(wallpaper, rect->x, rect->y, rect->width + 1, rect->height + 1);
			} else {
				_g->setColor(128,128,255);
				_g->fillRect(rect->x, rect->y, rect->width + 1, rect->height + 1);
			}
			// 画面からはみだしていないかチェック
			int wx = rect->x + mx - preX;
			int wy = rect->y + my - preY;
			if (rect->x + mx - preX <= 0) wx = 0;
			if (rect->y + my - preY <= 22) wy = 22;
			if (rect->x + mx - preX + rect->width + 1 >= width) wx = width - rect->width - 1;
			if (rect->y + my - preY + rect->height + 1 >= height) wy = height - rect->height - 1;
			// ウィンドウ移動
			getActiveControl()->setRect(
				wx,
				wy,
				rect->width,
				rect->height
			);
			// 領域変更メッセージを投げる
			if (MonAPI::Message::send(getActiveControl()->getThreadID(), MSG_GUISERVER_SETRECT, 
				(wx << 16 | wy), (rect->width << 16 | rect->height), 0, NULL)) {
				//printf("WindowManager->Window: MSG_GUISERVER_SETRECT failed %d\n", getActiveControl()->getThreadID());
			} else {
				//printf("WindowManager->Window: MSG_GUISERVER_SETRECT sended %d\n", getActiveControl()->getThreadID());
			}
			// ウィンドウ再描画
			for (int i = 0; i < controlListPtr; i++) {
				//controlList[i]->repaint();
				// 再描画メッセージを投げる
				if (MonAPI::Message::sendReceive(&info, controlList[i]->getThreadID(), MSG_GUISERVER_REPAINT, 0, 0, 0, NULL)) {
					//printf("WindowManager->Window: MSG_GUISERVER_REPAINT failed %d\n", controlList[i]->getThreadID());
				} else {
					//printf("WindowManager->Window: MSG_GUISERVER_REPAINT sended %d\n", controlList[i]->getThreadID());
				}
			}
			// モード設定
			state = STATE_NORMAL;
			//printf("move window end: %d,%d\n", preX, preY);
		} else  if (control->getIconified() == false) {
			//MouseEvent *event = new MouseEvent(MOUSE_RELEASED, control, mx, my);
			//control->postEvent(event);
			//delete(event);
			// マウスイベントを投げる
			if (MonAPI::Message::send(control->getThreadID(), MSG_GUISERVER_ONMOUSERELEASE, mx, my, 0, NULL)) {
				//printf("WindowManager->Window: MSG_GUISERVER_ONMOUSERELEASE failed %d\n", control->getThreadID());
			} else {
				//printf("WindowManager->Window: MSG_GUISERVER_ONMOUSERELEASE sended %d\n", control->getThreadID());
			}
			//printf("throw mouse_up event\n");
		}
	}
}

/**
 指定したウィンドウを追加する.
 @param control 指定するウィンドウ
 */
void WindowManager::add(Control *control)
{
	int i;
	MessageInfo info;
	
	// 最後に追加する
	if (controlListPtr < MAX_CONTROLLIST_LEN - 1) {
		// ウィンドウ追加
		controlList[controlListPtr] = control;
		// 活性・非活性の設定
		for (i = 0; i < controlListPtr; i++) {
			controlList[i]->setEnabled(false);
			// 非活性メッセージを投げる
			if (MonAPI::Message::send(controlList[i]->getThreadID(), MSG_GUISERVER_DISABLED, 0, 0, 0, NULL)) {
				//printf("WindowManager->Window: MSG_GUISERVER_DISABLED failed %d\n", controlList[i]->getThreadID());
			} else {
				//printf("WindowManager->Window: MSG_GUISERVER_DISABLED sended %d\n", controlList[i]->getThreadID());
			}
		}
		controlList[i]->setEnabled(true);
		// 活性メッセージを投げる
		if (MonAPI::Message::send(controlList[i]->getThreadID(), MSG_GUISERVER_ENABLED, 0, 0, 0, NULL)) {
			//printf("WindowManager->Window: MSG_GUISERVER_ENABLED failed %d\n", controlList[i]->getThreadID());
		} else {
			//printf("WindowManager->Window: MSG_GUISERVER_ENABLED sended %d\n", controlList[i]->getThreadID());
		}
		// ウィンドウ再描画
		if (firstpaint == true) {
			if (i > 0) {
				//controlList[i - 1]->repaint();
				// 再描画メッセージを投げる
				if (MonAPI::Message::sendReceive(&info, controlList[i - 1]->getThreadID(), MSG_GUISERVER_REPAINT, 0, 0, 0, NULL)) {
					//printf("WindowManager->Window: MSG_GUISERVER_REPAINT failed %d\n", controlList[i - 1]->getThreadID());
				} else {
					//printf("WindowManager->Window: MSG_GUISERVER_REPAINT sended %d\n", controlList[i - 1]->getThreadID());
				}
			}
			//controlList[i]->repaint();
			// 再描画メッセージを投げる
			if (MonAPI::Message::sendReceive(&info, controlList[i]->getThreadID(), MSG_GUISERVER_REPAINT, 0, 0, 0, NULL)) {
				//printf("WindowManager->Window: MSG_GUISERVER_REPAINT failed %d\n", controlList[i]->getThreadID());
			} else {
				//printf("WindowManager->Window: MSG_GUISERVER_REPAINT sended %d\n", controlList[i]->getThreadID());
			}
		}
		controlListPtr++;
	}
}

/**
 指定したウィンドウを削除する.
 用がすんだら、返ってきたインスタンスを必ずdeleteすること。
 @param control 指定するウィンドウ
 @return 削除されたウィンドウ（なければNULL）
 */
Control *WindowManager::remove(Control *control)
{
	int i, j;
	MessageInfo info;
	
	for (i = controlListPtr - 1; i >= 0; i--) {
		if (controlList[i] == control) {
			// 後ろから詰める
			for (j = i; j < controlListPtr - 1; j++) {
				controlList[j] = controlList[j + 1];
			}
			// 活性・非活性
			for (j = 0; j < controlListPtr - 2; j++) {
				controlList[j]->setEnabled(false);
				// 非活性メッセージを投げる
				if (MonAPI::Message::send(controlList[j]->getThreadID(), MSG_GUISERVER_DISABLED, 0, 0, 0, NULL)) {
					//printf("WindowManager->Window: MSG_GUISERVER_DISABLED failed %d\n", controlList[j]->getThreadID());
				} else {
					//printf("WindowManager->Window: MSG_GUISERVER_DISABLED sended %d\n", controlList[j]->getThreadID());
				}
			}
			controlList[j]->setEnabled(true);
			// 活性メッセージを投げる
			if (MonAPI::Message::send(controlList[j]->getThreadID(), MSG_GUISERVER_ENABLED, 0, 0, 0, NULL)) {
				//printf("WindowManager->Window: MSG_GUISERVER_ENABLED failed %d\n", controlList[j]->getThreadID());
			} else {
				//printf("WindowManager->Window: MSG_GUISERVER_ENABLED sended %d\n", controlList[j]->getThreadID());
			}
			controlListPtr--;
			// 背景を塗りつぶす
			Rect *rect = control->getRect();
			if (wallpaper->getData() != NULL) {
				_g->drawImage(wallpaper, rect->x, rect->y, rect->width + 1, rect->height + 1);
			} else {
				_g->setColor(128,128,255);
				_g->fillRect(rect->x, rect->y, rect->width + 1, rect->height + 1);
			}
			// ウィンドウ再描画
			for (j = 0; j < controlListPtr; j++) {
				//controlList[j]->repaint();
				// 再描画メッセージを投げる
				if (MonAPI::Message::sendReceive(&info, controlList[j]->getThreadID(), MSG_GUISERVER_REPAINT, 0, 0, 0, NULL)) {
					//printf("WindowManager->Window: MSG_GUISERVER_REPAINT failed %d\n", controlList[j]->getThreadID());
				} else {
					//printf("WindowManager->Window: MSG_GUISERVER_REPAINT sended %d\n", controlList[j]->getThreadID());
				}
			}
			// 削除メッセージを投げる
			if (MonAPI::Message::send(control->getThreadID(), MSG_GUISERVER_REMOVE, 0, 0, 0, NULL)) {
				//printf("WindowManager->Window: MSG_GUISERVER_REMOVE failed %d\n", control->getThreadID());
			} else {
				//printf("WindowManager->Window: MSG_GUISERVER_REMOVE sended %d\n", control->getThreadID());
			}
			return control;
		}
	}
	
	return NULL;
}

/**
 指定したウィンドウを活性にするする.
 @param control 指定するウィンドウ
 */
void WindowManager::setActiveControl(Control *control)
{
	int i, j;
	MessageInfo info;
	
	for (i = controlListPtr - 1; i >= 0; i--) {
		if (controlList[i] == control) {
			// 後ろから詰める
			for (j = i; j < controlListPtr - 1; j++) {
				controlList[j] = controlList[j + 1];
				//controlList[j]->setEnabled(false);
				//controlList[j]->repaint();
				// 非活性メッセージを投げる
				if (MonAPI::Message::send(controlList[i]->getThreadID(), MSG_GUISERVER_DISABLED, 0, 0, 0, NULL)) {
					//printf("WindowManager->Window: MSG_GUISERVER_DISABLED failed %d\n", controlList[i]->getThreadID());
				} else {
					//printf("WindowManager->Window: MSG_GUISERVER_DISABLED sended %d\n", controlList[i]->getThreadID());
				}
				// 再描画メッセージを投げる
				if (MonAPI::Message::sendReceive(&info, controlList[i]->getThreadID(), MSG_GUISERVER_REPAINT, 0, 0, 0, NULL)) {
					//printf("WindowManager->Window: MSG_GUISERVER_REPAINT failed %d\n", controlList[i]->getThreadID());
				} else {
					//printf("WindowManager->Window: MSG_GUISERVER_REPAINT sended %d\n", controlList[i]->getThreadID());
				}
			}
			controlList[j] = control;
			controlList[j]->setEnabled(true);
			//controlList[j]->repaint();
			// 活性メッセージを投げる
			if (MonAPI::Message::send(controlList[j]->getThreadID(), MSG_GUISERVER_ENABLED, 0, 0, 0, NULL)) {
				//printf("WindowManager->Window: MSG_GUISERVER_ENABLED failed %d\n", controlList[j]->getThreadID());
			} else {
				//printf("WindowManager->Window: MSG_GUISERVER_ENABLED sended %d\n", controlList[j]->getThreadID());
			}
			// 再描画メッセージを投げる
			if (MonAPI::Message::sendReceive(&info, controlList[j]->getThreadID(), MSG_GUISERVER_REPAINT, 0, 0, 0, NULL)) {
				//printf("WindowManager->Window: MSG_GUISERVER_REPAINT failed %d\n", controlList[j]->getThreadID());
			} else {
				//printf("WindowManager->Window: MSG_GUISERVER_REPAINT sended %d\n", controlList[j]->getThreadID());
			}
			return;
		}
	}
}

/**
 指定したウィンドウを活性にするする.
 @param mx 指定するX座標
 @param my 指定するY座標
 */
bool WindowManager::setActiveControl(int mx, int my)
{
	int i;
	Rect *rect;
	
	if (controlListPtr <= 1) return false;
	
	// アクティブウィンドウ内をクリックしたときは無視
	rect = getActiveControl()->getRect();
	if (rect->x <= mx && mx <= rect->x + rect->width && 
		rect->y <= my && my <= rect->y + rect->height &&
		getActiveControl()->getIconified() == false)
	{
		return false;
	}
	if (rect->x <= mx && mx <= rect->x + rect->width && 
		rect->y <= my && my <= rect->y + INSETS_TOP &&
		getActiveControl()->getIconified() == true)
	{
		return false;
	}
	
	// 上から順にチェック
	for (i = controlListPtr - 2; i >= 0; i--) {
		rect = controlList[i]->getRect();
		if (rect->x <= mx && mx <= rect->x + rect->width && 
			rect->y <= my && my <= rect->y + rect->height)
		{
			setActiveControl(controlList[i]);
			return true;
		}
	}
	
	return false;
}

/** 活性ウィンドウを得る */
Control *WindowManager::getActiveControl()
{
	if (controlListPtr <= 0) {
		return NULL;
	} else {
		return controlList[controlListPtr - 1];
	}
}

/**
 活性ウィンドウを得る
 @param mx 指定するX座標
 @param my 指定するY座標
 */
Control *WindowManager::getActiveControl(int mx, int my)
{
	if (controlListPtr <= 0) {
		return NULL;
	} else {
		Control *control = controlList[controlListPtr - 1];
		Rect *rect = control->getRect();
		// マウスカーソルがある範囲にウィンドウがあるかどうかチェック
		if (rect->x <= mx && mx <= rect->x + rect->width && 
			rect->y <= my && my <= rect->y + rect->height)
		{
			return control;
		} else {
			return NULL;
		}
	}
}

/**
 活性ウィンドウを得る
 @param threadID スレッドID
 */
Control *WindowManager::getActiveControl(dword threadID)
{
	if (controlListPtr <= 0) {
		return NULL;
	} else {
		for (int i = 0; i < controlListPtr; i++) {
			if (controlList[i]->getThreadID() == threadID) {
				return controlList[i];
			}
		}
	}
	return NULL;
}

/** 再描画 */
void WindowManager::repaint()
{
	int i, j;
	
	if (isRunning == false)
		return;
	if (firstpaint == false)
		firstpaint = true;

	// 背景を塗りつぶす
	if (wallpaper->getData() != NULL) {
		_g->drawImage(wallpaper, 0, 0);
	} else {
		_g->setColor(128,128,255);
		_g->fillRect(0, 0, width, height);
	}
	
	// メニューバー
	_g->setColor(200,200,200);
	_g->fillRect(0,0,width,20);
	_g->setColor(128,128,128);
	_g->drawLine(0,20,width-1,20);
	_g->setColor(0,0,0);
	_g->drawLine(0,21,width-1,21);

	// 左上
	_g->drawLine(0,0,4,0);
	_g->drawLine(0,1,2,1);
	_g->drawLine(0,2,1,2);
	_g->drawLine(0,3,0,3);
	_g->drawLine(0,4,0,4);
	
	// 右上
	_g->drawLine(width-5,0,width-1,0);
	_g->drawLine(width-3,1,width-1,1);
	_g->drawLine(width-2,2,width-1,2);
	_g->drawLine(width-1,3,width-1,3);
	_g->drawLine(width-1,4,width-1,4);

	// 左下
	_g->drawLine(0,height-5,0,height-5);
	_g->drawLine(0,height-4,0,height-4);
	_g->drawLine(0,height-3,1,height-3);
	_g->drawLine(0,height-2,2,height-2);
	_g->drawLine(0,height-1,4,height-1);

	// 右下
	_g->drawLine(width-5,height-1,width-1,height-1);
	_g->drawLine(width-3,height-2,width-1,height-2);
	_g->drawLine(width-2,height-3,width-1,height-3);
	_g->drawLine(width-1,height-4,width-1,height-4);
	_g->drawLine(width-1,height-5,width-1,height-5);

	// オレンジアイコン
	for (i = 0; i < 15; i++) {
		for (j = 0; j < 15; j++) {
			_g->screen->putPixel16(19 + j, 4 + i, orangeIcon[i][j]);
		}
	}

	// メニュー
	int fh = FontManager::getInstance()->getHeight();
	_g->setColor(128,128,128);
	_g->drawText("ファイル　編集　表示　特別　ヘルプ", 45, 4 + (16 - fh) / 2);
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
	syscall_clear_screen();
	syscall_set_cursor(0, 0);
	printf("starting baygui ...\n");

#if 0
	// シェル出力を黙らせる
	MessageInfo msg, src;
	dword id = syscall_mthread_create((dword)StdoutMessageLoop);
	syscall_mthread_join(id);
	src.header = MSG_SERVER_START_OK;
	MonAPI::Message::receive(&msg, &src, MonAPI::Message::equalsHeader);
	stdoutID = msg.from;
	procsvrID = monapi_get_server_thread_id(ID_PROCESS_SERVER);
	if (procsvrID == THREAD_UNKNOWN) {
		//printf("baygui: MSG_PROCESS_GRAB_STDOUT failed\n");
	} else {
		MonAPI::Message::send(NULL, procsvrID + 1, MSG_PROCESS_GRAB_STDOUT, stdoutID);
		//printf("baygui: MSG_PROCESS_GRAB_STDOUT succeed\n");
	}
#endif

	// テストアプリ
	monapi_call_process_execute_file("/APPS/GRUNNER.EX5", MONAPI_FALSE);

	// 再描画
	repaint();
	
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
					// ウィンドウリスト
					if (info.arg3 == 'l') {
						int i;
						for (i = 0; i < controlListPtr; i++) {
							//printf("controlList[%d] = %d\n", i, controlList[i]->getThreadID());
						}
					}
				}
				// ウィンドウが一つもないときはイベントを送らない
				if (controlListPtr > 0) {
					if (info.arg2 & KEY_MODIFIER_DOWN) {
						onKeyPress(info.arg1, info.arg2, info.arg3);
					} else if (info.arg2 & KEY_MODIFIER_UP) {
						onKeyRelease(info.arg1, info.arg2, info.arg3);
					}
				}
				break;
			case MSG_MOUSE_INFO:
				monapi_call_mouse_set_cursor(0);
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
				monapi_call_mouse_set_cursor(1);
				break;
			case MSG_GUISERVER_ADD:
				{
					Control *control = new Control();
					control->setThreadID(info.arg1);
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
				remove(getActiveControl());
				break;
			case MSG_GUISERVER_STOP:
				//printf("Window->WindowManager MSG_GUISERVER_STOP received %d\n", info.arg1);
				isRunning = false;
				break;
			case MSG_GUISERVER_GETFONT:
				//printf("FontManager->WindowManager MSG_GUISERVER_GETFONT received\n");
				{
					monapi_cmemoryinfo *fpMemory = FontManager::getInstance()->fpMemory;
					MonAPI::Message::reply(&info, fpMemory->Handle, fpMemory->Size);
				}
				break;
			case MSG_GUISERVER_SETTIMER:
				//printf("Control->WindowManager MSG_GUISERVER_SETTIMER received %d, %d, %d\n", info.arg1, info.arg2, info.arg3);
				{
					Control *control = getActiveControl(info.arg1);
					if (control != NULL) {
						TimerEvent *timerEvent = control->getTimer();
						timerEvent->duration = info.arg2;
						timerEvent->end = info.arg3;
					}
				}
				break;
			}
		} else {
			// タイマーイベント
			int i;
			for (i = 0; i < controlListPtr; i++) {
				if (controlList[i]->getTimer()->duration > 0 && 
					controlList[i]->getTimer()->end <= syscall_get_tick() * 10)
				{
					controlList[i]->getTimer()->setDuration(-1);
					// タイマー設定メッセージを投げる
					if (MonAPI::Message::send(controlList[i]->getThreadID(), MSG_GUISERVER_ONTIMER, 0, 0, 0, NULL)) {
						//printf("WindowManager->Control: MSG_GUISERVER_ONTIMER failed %d\n", threadID);
						return;
					} else {
						//printf("WindowManager->Control: MSG_GUISERVER_ONTIMER sended %d\n", threadID);
					}
				}
			}
		}
	}
	
	// 画面クリア
	monapi_call_mouse_set_cursor(0);
	syscall_clear_screen();
	monapi_call_mouse_set_cursor(1);
	syscall_set_cursor(0, 0);
	printf("shutdown baygui ...\n");
}

/** メイン */
int MonaMain(List<char*>* pekoe)
{
	WindowManager *manager = WindowManager::getInstance();
	manager->service();
	delete(manager);
	return 0;
}
