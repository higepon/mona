/*
Copyright (c) 2004 bayside
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. The name of the author may not be used to endorse or promote products
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

#include "baygui.h"

/** タイマースレッドID */
static dword timerID = THREAD_UNKNOWN;

/** タイマースレッド */
static void TimerThread()
{
	MonAPI::Message::send(timerID, MSG_SERVER_START_OK);
	//printf("TimerThread created\n");
	
	MessageInfo info;
	while (1) {
		if (!MonAPI::Message::receive(&info)) {
			if (info.header == MSG_GUISERVER_SETTIMER) {
				//printf("WindowManager->TimerThread MSG_GUISERVER_SETTIMER received %d, %d\n", info.arg1, info.arg2);
				syscall_sleep(info.arg2 / 10);
				MonAPI::Message::send(info.arg1, MSG_GUISERVER_ONTIMER, 0, 0, 0, NULL);
			}
		}
	}
}

/** 閉じるボタン */
static unsigned int closeIcon[13][13] = {
	{0x94928c,0x949294,0x848284,0x848684,0x949294,0x848684,0x848684,0x848284,0x8c8a8c,0x848284,0x949294,0x7c7e7c,0xe4e2e4},
	{0x848284,0x141214,0x343234,0x1c1e1c,0x242624,0x242224,0x1c1e1c,0x242224,0x2c2e2c,0x141614,0x343634,0x1c1e1c,0xeceaec},
	{0x94928c,0x444244,0xdcdedc,0xd4d2d4,0xcccecc,0xcccecc,0xcccecc,0xcccecc,0xc4c6c4,0xd4d2d4,0xb4b2b4,0x3c3a3c,0xfcfefc},
	{0x84827c,0x141214,0xd4d6d4,0x9c9e9c,0x9c9a9c,0xacaaac,0xa4a6a4,0xbcbabc,0xbcbebc,0xc4c6c4,0x949694,0x1c1a1c,0xfcfefc},
	{0x8c8a8c,0x2c2a2c,0xd4d6d4,0x9c9a9c,0xa4a6a4,0xb4b6b4,0xbcbabc,0xc4c2c4,0xcccecc,0xd4d2d4,0x8c8a8c,0x1c1a1c,0xfcfefc},
	{0x8c8a84,0x1c1e1c,0xcccacc,0xa4a2a4,0xb4b2b4,0xb4b6b4,0xc4c2c4,0xcccacc,0xd4d2d4,0xdcdadc,0x8c8a8c,0x242224,0xfcfefc},
	{0x848684,0x242224,0xcccacc,0xacaeac,0xb4b6b4,0xbcbebc,0xcccecc,0xd4d2d4,0xdcdedc,0xdcdedc,0x848284,0x242624,0xf4f6f4},
	{0x8c8a8c,0x2c2a2c,0xcccecc,0xb4b2b4,0xbcbebc,0xcccacc,0xcccacc,0xd4d2d4,0xe4e6e4,0xf4f2f4,0x94928c,0x242224,0xfcfafc},
	{0x94928c,0x1c1e1c,0xcccecc,0xbcbebc,0xcccacc,0xd4d6d4,0xd4d2d4,0xdcdedc,0xeceeec,0xe4e6e4,0x8c8a8c,0x141614,0xfcfefc},
	{0x84827c,0x1c1a1c,0xd4d2d4,0xc4c6c4,0xcccacc,0xdcdedc,0xe4e2e4,0xf4f2f4,0xf4f2f4,0xf4f6f4,0x9c9a9c,0x141614,0xfcfefc},
	{0x948e8c,0x2c2e2c,0xb4b2b4,0x9c9a9c,0x848684,0x8c8a8c,0x8c8a8c,0x8c8a8c,0x7c7e7c,0x9c9a9c,0x747274,0x2c2a2c,0xfcfefc},
	{0x9c9e9c,0x141214,0x444244,0x242624,0x1c1e1c,0x2c2a2c,0x2c2a2c,0x242224,0x242624,0x1c1a1c,0x2c2a2c,0x343634,0xe4e6e4},
	{0xc4c2c4,0xfcfefc,0xfcfefc,0xf4f2f4,0xf4f6f4,0xfcfefc,0xfcfefc,0xf4f6f4,0xfcfefc,0xfcfefc,0xfcfefc,0xeceaec,0xfcfefc},
};

/** 最小化ボタン */
static unsigned int minimizeIcon[13][13] = {
	{0x8c8e8c,0x8c8e8c,0x8c8e8c,0x8c8e8c,0x8c8e8c,0x8c8e8c,0x8c8e8c,0x8c8e8c,0x8c8e8c,0x8c8e8c,0x8c8e8c,0x8c8e8c,0xcccecc},
	{0x8c8e8c,0x242224,0x242224,0x242224,0x242224,0x242224,0x242224,0x242224,0x242224,0x242224,0x242224,0x242224,0xfcfefc},
	{0x8c8e8c,0x242224,0xfcfefc,0xcccecc,0xcccecc,0xcccecc,0xcccecc,0xcccecc,0xcccecc,0xcccecc,0xcccecc,0x242224,0xfcfefc},
	{0x8c8e8c,0x242224,0xcccecc,0x9c9e9c,0x9c9e9c,0xacaeac,0xacaeac,0xbcbebc,0xbcbebc,0xcccecc,0x8c8e8c,0x242224,0xfcfefc},
	{0x8c8e8c,0x242224,0xbcbebc,0x9c9e9c,0xacaeac,0xacaeac,0xbcbebc,0xbcbebc,0xcccecc,0xcccecc,0x8c8e8c,0x242224,0xfcfefc},
	{0x8c8e8c,0x242224,0x242224,0x242224,0x242224,0x242224,0x242224,0x242224,0x242224,0x242224,0x242224,0x242224,0xfcfefc},
	{0x8c8e8c,0x242224,0xcccecc,0xacaeac,0xbcbebc,0xbcbebc,0xcccecc,0xcccecc,0xdcdedc,0xdcdedc,0x8c8e8c,0x242224,0xfcfefc},
	{0x8c8e8c,0x242224,0x242224,0x242224,0x242224,0x242224,0x242224,0x242224,0x242224,0x242224,0x242224,0x242224,0xfcfefc},
	{0x8c8e8c,0x242224,0xcccecc,0xbcbebc,0xcccecc,0xcccecc,0xdcdedc,0xdcdedc,0xeceeec,0xeceeec,0x8c8e8c,0x242224,0xfcfefc},
	{0x8c8e8c,0x242224,0xcccecc,0xcccecc,0xcccecc,0xdcdedc,0xdcdedc,0xeceeec,0xeceeec,0xfcfefc,0x8c8e8c,0x242224,0xfcfefc},
	{0x8c8e8c,0x242224,0xcccecc,0x8c8e8c,0x8c8e8c,0x8c8e8c,0x8c8e8c,0x8c8e8c,0x8c8e8c,0x8c8e8c,0x8c8e8c,0x242224,0xfcfefc},
	{0x8c8e8c,0x242224,0x242224,0x242224,0x242224,0x242224,0x242224,0x242224,0x242224,0x242224,0x242224,0x242224,0xfcfefc},
	{0xcccecc,0xfcfefc,0xfcfefc,0xfcfefc,0xfcfefc,0xfcfefc,0xfcfefc,0xfcfefc,0xfcfefc,0xfcfefc,0xfcfefc,0xfcfefc,0xfcfefc},
};

/** コンストラクタ */
Window::Window()
{
	copyString(title, "window");

	// スレッドIDを得る
	threadID = MonAPI::System::getThreadID();

	// GUIサーバーを探す
	guisvrID = MonAPI::Message::lookupMainThread(GUISERVER_NAME);
	if (guisvrID == 0xFFFFFFFF) {
		//printf("Window: GuiServer not found %d\n", threadID);
	} else {
		//printf("Window: GuiServer found %d\n", threadID);
	}

	// ウィンドウ内部描画領域
	__g = new Graphics();
	
	// キーイベント
	_keyEvent = new KeyEvent(KEY_PRESSED, this, 0, 0);
	
	// マウスイベント
	_mouseEvent = new MouseEvent(MOUSE_PRESSED, this, 0, 0);
	
	// タイマーイベント
	_timerEvent = new Event(TIMER, this);

	// フォントロード
	FontManager::getInstance();

	// タイマー起動
	if (timerID == THREAD_UNKNOWN) timerID = syscall_get_tid();
	MessageInfo msg, src;
	dword id = syscall_mthread_create((dword)TimerThread);
	syscall_mthread_join(id);
	src.header = MSG_SERVER_START_OK;
	MonAPI::Message::receive(&msg, &src, MonAPI::Message::equalsHeader);
	timerID = msg.from;
}

/** デストラクタ */
Window::~Window() {
	delete(__g);
	delete(_keyEvent);
	delete(_mouseEvent);
	delete(_timerEvent);
}

/** タイトルを得る */
char *Window::getTitle()
{
	return title;
}

/**
 タイトル設定
 @param title タイトル
 */
void Window::setTitle(char *title)
{
	copyString(this->title, title);
}

/**
 大きさを設定する.
 @param x x座標
 @param y y座標
 @param width 幅
 @param height 高さ
*/
void Window::setRect(int x, int y, int width, int height)
{
	int i;
	int mx = x - this->x; // 移動量
	int my = y - this->y; // 移動量
	
	this->x = x;
	this->y = y;
	this->width = width;
	this->height = height;
	_g->translate(x, y);
	_g->setClip(x, y, width, height);

	// ウィンドウ内部描画領域領域
	__g->translate(x + INSETS_LEFT, y + INSETS_TOP);
	__g->setClip(x + INSETS_LEFT, y + INSETS_TOP, 
		width - INSETS_LEFT - INSETS_RIGHT, height - INSETS_TOP - INSETS_BOTTOM);

	// 子部品の位置も更新
	for(i = 0; i < _controlList->getLength(); i++) {
		Control *control = (Control *)_controlList->getItem(i)->data;
		Rect *rect = control->getRect();
		control->setRect(
			rect->x + mx, 
			rect->y + my, 
			rect->width, 
			rect->height
		);
	}
}

/**
 タイマーをセットする
 @param duration 発動するまでの時間[ms]
 */
void Window::setTimer(int duration)
{
	// 非活性のときはタイマーを発生させない
	if (focused == false || enabled == false || iconified == true) return;
	
	// タイマー設定メッセージを投げる
	if (MonAPI::Message::send(timerID, MSG_GUISERVER_SETTIMER, threadID, duration, 0, NULL)) {
		//printf("Window->Window: MSG_GUISERVER_SETTIMER failed %d\n", threadID);
	} else {
		//printf("Window->Window: MSG_GUISERVER_SETTIMER sended %d\n", threadID);
	}
}

/**
 ウィンドウの背景を復元する
 <ul>
 <li>壁紙があるときは壁紙を描画し、なおときは背景色で塗りつぶす
 <li>Window::repaint() をオーバーライドした矩形ウィンドウでないとおそらく使い道なし
 </ul>
 */
void Window::restoreBackGround()
{
	// 背景復元メッセージを投げる
	MessageInfo info;
	if (MonAPI::Message::sendReceive(&info, guisvrID, MSG_GUISERVER_RESTORE, threadID, 0, 0, NULL)) {
		//printf("Control->WindowManager: MSG_GUISERVER_RESTORE failed %d\n", threadID);
	} else {
		//printf("Control->WindowManager: MSG_GUISERVER_RESTORE sended %d\n", threadID);
	}
}

/**
 指定した部品を追加する
 @param control 指定する部品
 */
void Window::add(Control *control)
{
	// 最後に追加する
	control->setParent(this);
	Rect *rect = control->getRect();
	// 部品をコンテナーの中に配置する
	control->setRect
		(x + INSETS_LEFT + rect->x, y + INSETS_TOP + rect->y, rect->width, rect->height);
	// 部品を再描画
	if (firstpaint == true) {
		control->repaint();
	}
	_controlList->add(new LinkedItem(control));
}

/**
 指定した部品を削除する
 @param control 指定する部品
 @return 削除された部品（なければNULL）
 */
void Window::remove(Control *control)
{
	for(int i = 0; i < _controlList->getLength(); i++) {
		LinkedItem *item = _controlList->getItem(i);
		if (item->data == control) {
			_controlList->remove(item);
		}
	}
}

/** イベント処理 */
void Window::postEvent(Event *event)
{
	// 非活性の時はイベントを受け付けない
	if (enabled == false) return;

	// 活性部品にキーイベントを投げる
	if (KEY_PRESSED <= event->type && event->type <= KEY_RELEASED) {
		Control *control = findChild();
		// 部品でイベントが起こった
		if (control != NULL) {
			event->source = control;
			control->postEvent(event);
		// 部品以外でイベントが起こった
		} else {
			onEvent(event);
		}
	// マウスクリック
	} else if (event->type == MOUSE_PRESSED) {
		// マウスイベントが起こった部品を探す
		int ex = ((MouseEvent *)event)->x;
		int ey = ((MouseEvent *)event)->y;
		Control *control = findChild(ex, ey);
		// 部品でイベントが起こった
		if (control != NULL) {
			// イベントが起こった部品以外をフォーカスアウト状態にする
			LinkedItem *item = _controlList->firstItem;
			// NULLチェック
			if (item != NULL) {
				Control *c = (Control *)item->data;
				if (c == control) {
					c->setFocused(true);
				} else {
					c->setFocused(false);
				}
				while (item->next != NULL) {
					item = item->next;
					c = (Control *)item->data;
					if (c == control) {
						c->setFocused(true);
					} else {
						c->setFocused(false);
					}
				}
			}
			event->source = control;
			// 座標変換
			((MouseEvent *)event)->x -= (x + INSETS_LEFT);
			((MouseEvent *)event)->y -= (y + INSETS_TOP);
			control->postEvent(event);
		// 部品以外でイベントが起こった
		} else {
			// 部品をフォーカスアウト状態にする
			LinkedItem *item = _controlList->firstItem;
			// NULLチェック
			if (item != NULL) {
				Control *c = (Control *)item->data;
				c->setFocused(false);
				while (item->next != NULL) {
					item = item->next;
					c = (Control *)item->data;
					c->setFocused(false);
				}
			}
			onEvent(event);
		}
	// マウスリリース
	} else if (event->type == MOUSE_RELEASED) {
		// マウスイベントが起こった部品を探す
		int ex = ((MouseEvent *)event)->x;
		int ey = ((MouseEvent *)event)->y;
		Control *control = findChild(ex, ey);
		// 部品でイベントが起こった
		if (control != NULL) {
			event->source = control;
			// 座標変換
			((MouseEvent *)event)->x -= (x + INSETS_LEFT);
			((MouseEvent *)event)->y -= (y + INSETS_TOP);
			control->postEvent(event);
		// 部品以外でイベントが起こった
		} else {
			onEvent(event);
		}
	// マウスドラッグ
	} else if (event->type == MOUSE_DRAGGED) {
		Control *control = findChild();
		// 部品でイベントが起こった
		if (control != NULL) {
			event->source = control;
			// 座標変換
			((MouseEvent *)event)->x -= (x + INSETS_LEFT);
			((MouseEvent *)event)->y -= (y + INSETS_TOP);
			control->postEvent(event);
		// 部品以外でイベントが起こった
		} else {
			onEvent(event);
		}
	} else {
		onEvent(event);
	}
}

/** 再描画 */
void Window::repaint()
{
	int i, j;
	
	if (firstpaint == false)
		firstpaint = true;
	
	if (iconified == true) {
		// 外枠
		_g->setColor(0,0,0);
		_g->drawRect(0, 0, width, INSETS_TOP - 1);
		_g->setColor(200,200,200);
		_g->fillRect(1, 1, width - 2, INSETS_TOP - 2);
		// 輪郭線
		_g->setColor(255,255,255);
		_g->drawLine(1, 1, width - 1, 1);
		_g->drawLine(1, 1, 1, INSETS_TOP - 2);
		_g->setColor(128,128,128);
		_g->drawLine(width - 1, 2, width - 1, INSETS_TOP - 2);
		_g->drawLine(2, INSETS_TOP - 2, width - 1, INSETS_TOP - 2);
	} else {
		// 外枠
		_g->setColor(0,0,0);
		_g->drawRect(0, 0, width, height);
		_g->setColor(200,200,200);
		_g->fillRect(1, 1, width - 2, height - 2);
		// 内枠
		_g->setColor(0,0,0);
		_g->drawRect(5, 21, width - 10, height - 26);
		// 輪郭線
		_g->setColor(255,255,255);
		_g->drawLine(1, 1, width - 1, 1);
		_g->drawLine(1, 1, 1, height - 1);
		_g->drawLine(width - 4, 21, width - 4, height - 4);
		_g->drawLine(5, height - 4, width - 4, height - 4);
		_g->setColor(128,128,128);
		_g->drawLine(width - 1, 2, width - 1, height - 1);
		_g->drawLine(2, height - 1, width - 1, height - 1);
		_g->drawLine(4, 20, width - 5, 20);
		_g->drawLine(4, 20, 4, height - 5);
	}

	if (focused == true) {
		// タイトルライン
		for (i = 5; i <= 15; i = i + 2) {
			_g->setColor(128,128,128);
			_g->drawLine(20, i, width - 21, i);
			_g->setColor(255,255,255);
			_g->drawLine(21, i + 1, width - 22, i + 1);
		}
		// 閉じるアイコン、最小化アイコン
		for (i = 0; i < 13; i++) {
			for (j = 0; j < 13; j++) {
				_g->drawPixel(4 + j, 5 + i, closeIcon[i][j]);
				_g->drawPixel(width - 16 + j, 5 + i, minimizeIcon[i][j]);
			}
		}
	} else {
		// NOP
	}

	// タイトル
	int fw = FontManager::getInstance()->getWidth(title);
	int fh = FontManager::getInstance()->getHeight();
	_g->setColor(200,200,200);
	_g->fillRect(((width - fw) / 2) - 4, 2, fw + 8, INSETS_TOP - 4);
	if (focused == true) {
		_g->setColor(0,0,0);
	} else {
		_g->setColor(128,128,128);
	}
	_g->drawText(title, ((width - fw) / 2), ((INSETS_TOP - fh) / 2));
	//_g->drawText(title, ((width - fw) / 2) + 1, ((INSETS_TOP - fh) / 2));//太字

	if (iconified == false) {
		// 子部品も更新
		for(i = 0; i < _controlList->getLength(); i++) {
			Control *control = (Control *)_controlList->getItem(i)->data;
			control->repaint();
		}
		onPaint(__g);
	}
}

/** スレッド開始 */
void Window::run()
{
	MessageInfo info;

	// ウィンドウ追加メッセージを投げる
	if (MonAPI::Message::send(guisvrID, MSG_GUISERVER_ADD, threadID, (x << 16 | y), (width << 16 | height), NULL)) {
		//printf("Window->WindowManager: MSG_GUISERVER_ADD failed %d\n", threadID);
	} else {
		//printf("Window->WindowManager: MSG_GUISERVER_ADD sended %d\n", threadID);
	}

	// 実行中フラグ
	bool isRunning = true;

	while (isRunning) {
		if (!MonAPI::Message::receive(&info)) {
		//if (!MonAPI::Message::peek(&info, 0, PEEK_REMOVE)) {
			// draw mouse_cursor
			monapi_call_mouse_set_cursor(0);
			
			switch(info.header){
			case MSG_GUISERVER_ONKEYPRESS:
				//printf("WindowManager->Window MSG_GUISERVER_ONKEYPRESS received %d,%d,%d\n", info.arg1, info.arg2, info.arg3);
				_keyEvent->type = KEY_PRESSED;
				_keyEvent->keycode = info.arg1;
				_keyEvent->modifiers = info.arg2;
				postEvent(_keyEvent);
				break;
			case MSG_GUISERVER_ONKEYRELEASE:
				//printf("WindowManager->Window MSG_GUISERVER_ONKEYRELEASE received %d,%d,%d\n", info.arg1, info.arg2, info.arg3);
				_keyEvent->type = KEY_RELEASED;
				_keyEvent->keycode = info.arg1;
				_keyEvent->modifiers = info.arg2;
				postEvent(_keyEvent);
				break;
			case MSG_GUISERVER_ONMOUSEPRESS:
				//printf("WindowManager->Window MSG_GUISERVER_ONMOUSEPRESS received %d,%d,%d\n", info.arg1, info.arg2, info.arg3);
				_mouseEvent->type = MOUSE_PRESSED;
				_mouseEvent->x = info.arg1;
				_mouseEvent->y = info.arg2;
				postEvent(_mouseEvent);
				break;
			case MSG_GUISERVER_ONMOUSEDRAG:
				//printf("WindowManager->Window MSG_GUISERVER_ONMOUSEDRAG received %d,%d,%d\n", info.arg1, info.arg2, info.arg3);
				_mouseEvent->type = MOUSE_DRAGGED;
				_mouseEvent->x = info.arg1;
				_mouseEvent->y = info.arg2;
				postEvent(_mouseEvent);
				break;
			case MSG_GUISERVER_ONMOUSERELEASE:
				//printf("WindowManager->Window MSG_GUISERVER_ONMOUSERELEASE received %d,%d,%d\n", info.arg1, info.arg2, info.arg3);
				_mouseEvent->type = MOUSE_RELEASED;
				_mouseEvent->x = info.arg1;
				_mouseEvent->y = info.arg2;
				postEvent(_mouseEvent);
				break;
			case MSG_GUISERVER_ONMOUSEMOVE:
				//printf("WindowManager->Window MSG_GUISERVER_ONMOUSEMOVE received %d,%d,%d\n", info.arg1, info.arg2, info.arg3);
				_mouseEvent->type = MOUSE_MOVED;
				_mouseEvent->x = info.arg1;
				_mouseEvent->y = info.arg2;
				postEvent(_mouseEvent);
				break;
			case MSG_GUISERVER_ONTIMER:
				//printf("TimerThread->Window MSG_GUISERVER_ONTIMER received\n");
				postEvent(_timerEvent);
				break;
			case MSG_GUISERVER_SETRECT:
				{
					int x = info.arg1 >> 16;
					int y = info.arg1 & 0xFFFF;
					int w = info.arg2 >> 16;
					int h = info.arg2 & 0xFFFF;
					setRect(x, y, w, h);
					//printf("WindowManager->Window MSG_GUISERVER_SETRECT received %d,%d,%d,%d\n", x, y, w, h);
				}
				//MonAPI::Message::reply(&info);
				break;
			case MSG_GUISERVER_REPAINT:
				//printf("WindowManager->Window MSG_GUISERVER_REPAINT received %d\n", threadID);
				repaint();
				MonAPI::Message::reply(&info);
				break;
			case MSG_GUISERVER_ENABLED:
				//printf("WindowManager->Window MSG_GUISERVER_ENABLED received %d\n", threadID);
				setEnabled(true);
				//MonAPI::Message::reply(&info);
				break;
			case MSG_GUISERVER_DISABLED:
				//printf("WindowManager->Window MSG_GUISERVER_DISABLED received %d\n", threadID);
				setEnabled(false);
				//MonAPI::Message::reply(&info);
				break;
			case MSG_GUISERVER_FOCUSED:
				//printf("WindowManager->Window MSG_GUISERVER_FOCUSED received %d\n", threadID);
				setFocused(true);
				repaint();
				//MonAPI::Message::reply(&info);
				break;
			case MSG_GUISERVER_DEFOCUSED:
				//printf("WindowManager->Window MSG_GUISERVER_DEFOCUSED received %d\n", threadID);
				setFocused(false);
				{
					// 部品をフォーカスアウト状態にする
					// NULLチェック
					if (_controlList->firstItem == NULL) break;

					// 前からチェックしていく
					LinkedItem *item = _controlList->firstItem;
					Control *c = (Control *)item->data;
					c->setFocused(false);
					while (item->next != NULL) {
						item = item->next;
						c = (Control *)item->data;
						c->setFocused(false);
					}
				}
				//MonAPI::Message::reply(&info);
				break;
			case MSG_GUISERVER_ICONIFIED:
				//printf("WindowManager->Window MSG_GUISERVER_ICONIFIED received %d\n", threadID);
				setIconified(true);
				MonAPI::Message::reply(&info);
				break;
			case MSG_GUISERVER_DEICONIFIED:
				//printf("WindowManager->Window MSG_GUISERVER_DEICONIFIED received %d\n", threadID);
				setIconified(false);
				MonAPI::Message::reply(&info);
				break;
			case MSG_GUISERVER_REMOVE:
				//printf("WindowManager->Window MSG_GUISERVER_REMOVE received %d\n", threadID);
				// タイマースレッド停止
				syscall_kill_thread(timerID);
				// フラグ変更
				isRunning = false;
				MonAPI::Message::reply(&info);
				break;
			default:
				break;
			}

			// draw mouse_cursor
			monapi_call_mouse_set_cursor(1);
		}
	}
}
