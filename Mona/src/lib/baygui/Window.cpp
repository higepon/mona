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

/** 閉じるボタン (パレット) */
static unsigned int close_palette[] = {
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
static unsigned char close_data[] = {
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

/** 共通パラメータ */
CommonParameters *__commonParams;
/** 共通パラメータのハンドル */
static dword commonParamsHandle;
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
			if (info.header == TIMER && info.arg2 > 0) {
				int duration = (info.arg2 < 10) ? 1 : (info.arg2 / 10);
				syscall_sleep(duration);
				MonAPI::Message::send(info.arg1, TIMER, 0, 0, 0, NULL);
			}
		}
	}
}

/** コンストラクタ */
Window::Window()
{
	// 共通パラメータを得る
	MessageInfo info;
	if (MonAPI::Message::sendReceive(&info, monapi_get_server_thread_id(ID_PROCESS_SERVER), MSG_PROCESS_GET_COMMON_PARAMS) != 0) {
		printf("%s:%d:ERROR: can not get common parameter!\n", __FILE__, __LINE__);
		exit(1);
	} else {
		commonParamsHandle = info.arg2;
		__commonParams = (CommonParameters*)MonAPI::MemoryMap::map(commonParamsHandle);
	}

	// GUIサーバーを探す
	this->guisvrID = monapi_get_server_thread_id(ID_GUI_SERVER);
	if (this->guisvrID == THREAD_UNKNOWN) {
		printf("%s:%d:ERROR: can not connect to GUI server!\n", __FILE__, __LINE__);
		exit(1);
	}

	// GUIサーバーに自分を登録する
	if (!monapi_register_to_server(ID_GUI_SERVER, MONAPI_TRUE)) {
		printf("%s:%d:ERROR: can not register to GUI server!\n", __FILE__, __LINE__);
		exit(1);
	}

	this->title = "window";
	this->threadID = MonAPI::System::getThreadID();
	this->modifiers = this->state = this->preX = this->preY = 0;
	this->overlap = 0;
	this->isRunning = false;
	
	// キーイベント
	this->keyEvent.type      = KEY_PRESSED;
	this->keyEvent.source    = this;
	this->keyEvent.keycode   = 0;
	this->keyEvent.modifiers = 0;
	
	// マウスイベント
	this->mouseEvent.type   = MOUSE_PRESSED;
	this->mouseEvent.source = this;
	this->mouseEvent.x      = 0;
	this->mouseEvent.y      = 0;
	this->mouseEvent.button = 0;
	
	// タイマーイベント
	this->timerEvent.type = TIMER;
	this->timerEvent.source = this;
	
	// カスタムイベント
	this->customEvent.type = CUSTOM_EVENT;
	this->customEvent.source = this;
	
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
	// タイマースレッド停止
	syscall_kill_thread(timerID);
	
	// GUIサーバーから自分を抹消する
	monapi_register_to_server(ID_GUI_SERVER, MONAPI_FALSE);
	MonAPI::MemoryMap::unmap(commonParamsHandle);
}

/**
 部品生成時に呼ばれる.
 dispose()後に呼ぶと再初期化できる。
*/
void Window::create()
{
	if (this->_buffer != NULL) return;

	// 描画バッファー、描画オブジェクトの生成
	this->_buffer = new Image
		(this->width - INSETS_LEFT - INSETS_RIGHT, this->height - INSETS_TOP - INSETS_BOTTOM);
	for (int i = 0; i < this->height; i++) {
		for (int j = 0; j < this->width; j++) {
			_buffer->setPixel(j, i, DEFAULT_BACKCOLOR);
		}
	}
	this->_g = new Graphics(this->_buffer);
	this->_metrics = new FontMetrics();
	this->__buffer = new Image(this->width, this->height);
	this->__g = new Graphics(this->__buffer);

	// ウィンドウを生成する
	MessageInfo msg;
	if (MonAPI::Message::sendReceive(&msg, guisvrID, MSG_GUISERVER_CREATEWINDOW) != 0) {
		printf("%s:%d:ERROR: can not connect to GUI server!\n", __FILE__, __LINE__);
		return;
	}

	// GUIサーバー上のウィンドウオブジェクトを生成する
	this->_window = (guiserver_window*)MonAPI::MemoryMap::map(msg.arg2);
	if (this->_window == NULL) {
		printf("%s:%d:ERROR: can not create window!\n", __FILE__, __LINE__);
		return;
	}

	// 初期設定
	this->_window->X = this->x;
	this->_window->Y = this->y;
	this->_window->Width  = this->width;
	this->_window->Height = this->height;
	this->_window->OffsetX = this->offsetX;
	this->_window->OffsetY = this->offsetY;
	this->_window->BufferHandle = this->_buffer->getHandle();
	this->_window->FormBufferHandle = this->__buffer->getHandle();
	this->_window->TransparencyKey = 0x00000000;
	this->_window->Visible = true;
	this->_window->Opacity = 0xff; // 不透明
	//if (this->parent != NULL) {
	//	this->foreColor = this->parent->getForeground();
	//	this->backColor = this->parent->getBackground();
	//}
	this->_window->__internal2 = true;
	this->focused = true;

	// ウィンドウをアクティブにする
	if (MonAPI::Message::sendReceive(NULL, guisvrID, MSG_GUISERVER_ACTIVATEWINDOW, this->_window->Handle)) {
		printf("%s:%d:ERROR: can not activate window!\n", __FILE__, __LINE__);
	}
}

/**
 部品破棄時に呼ばれる.
 後にcreate()を呼ぶと再初期化できる。
*/
void Window::dispose()
{
	// ウィンドウ破棄要求
	setVisible(false);
	dword handle = getHandle();
	delete(_buffer);
	delete(_g);
	delete(__buffer);
	delete(__g);
	if (MonAPI::Message::sendReceive(NULL, guisvrID, MSG_GUISERVER_DISPOSEWINDOW, handle)) {
		printf("%s:%d:ERROR: can not connect to GUI server!\n", __FILE__, __LINE__);
	}
}

/** ハンドルを得る */
unsigned int Window::getHandle()
{
	if (this->_window != NULL) {
		return this->_window->Handle;
	} else {
		return 0;
	}
}

/**
 タイトル設定
 @param title タイトル
 */
void Window::setTitle(char *title)
{
	this->title = title;
}

/**
 表示状態を設定する
 @param visible 表示状態 (true / false)
 */
void Window::setVisible(bool visible)
{
	Control::setVisible(visible);
	this->_window->Visible = visible;
	update();
}

/**
 位置を変更する
 @param x X座標
 @param y Y座標
*/
void Window::setLocation(int x, int y)
{
	Control::setLocation(x, y);
	
	if (this->_window == NULL) return;
	if (this->parent == NULL) {
		MonAPI::Message::sendReceive(NULL, guisvrID, MSG_GUISERVER_MOVEWINDOW, 
			getHandle(), (unsigned int)x, (unsigned int)y);
	} else {
		this->_window->X = x;
		this->_window->Y = y;
	}
	
	update();
}

/**
 タイマーをセットする
 @param duration 発動するまでの時間[ms]
 */
void Window::setTimer(int duration)
{
	// 非活性のときはタイマーを発生させない
	if (this->enabled == false) return;
	
	// タイマー設定メッセージを投げる
	if (MonAPI::Message::send(timerID, TIMER, this->threadID, duration, 0, NULL)) {
		printf("%s:%d:ERROR: can not send TIMER\n", __FILE__, __LINE__);
	}
}

/** 再描画 */
void Window::repaint()
{
	int i, j, w, h;
	
	if (this->_buffer == NULL) return;
	
	w = this->width;
	h = this->height;
	
	// 外枠
	__g->setColor(COLOR_LIGHTGRAY);
	__g->fillRect(0, 0, w, h);
	__g->setColor(COLOR_BLACK);
	__g->drawRect(0, 0, w, h);
	
	// 内枠
	__g->setColor(COLOR_BLACK);
	__g->drawRect(5, 21, w - 10, h - 26);
	
	// 枠
	__g->setColor(COLOR_WHITE);
	__g->drawLine(1, 1, w - 2, 1);
	__g->drawLine(1, 1, 1, h - 2);
	__g->drawLine(w - 5, 21, w - 5, h - 5);
	__g->drawLine(5, h - 5, w - 5, h - 5);
	__g->setColor(COLOR_GRAY);
	__g->drawLine(w - 2, 2, w - 2, h - 2);
	__g->drawLine(2, h - 2, w - 2, h - 2);
	__g->drawLine(4, 20, w - 6, 20);
	__g->drawLine(4, 20, 4, h - 6);
	
	if (this->focused == true) {
		// タイトルバー
		for (i = 4; i <= 14; i = i + 2) {
			__g->setColor(COLOR_GRAY);
			__g->drawLine(20, i, w - 7, i);
			__g->setColor(COLOR_WHITE);
			__g->drawLine(21, i + 1, w - 6, i + 1);
		}
		
		// 閉じるボタン
		for (i = 0; i < 13; i++) {
			for (j = 0; j < 13; j++) {
				__g->drawPixel(j + 4, i + 4, close_palette[close_data[i * 13 + j] & 0xFF]);
			}
		}
	}

	// タイトル
	FontMetrics metrics;
	int fw = metrics.getWidth(getTitle());
	int fh = metrics.getHeight(getTitle());
	__g->setColor(COLOR_LIGHTGRAY);
	__g->fillRect(((w - fw) / 2) - 4, 2, fw + 8, INSETS_TOP - 4);
	if (this->focused == true) {
		__g->setColor(COLOR_BLACK);
	} else {
		__g->setColor(COLOR_GRAY);
	}
	__g->drawText(getTitle(), ((w - fw) / 2), ((INSETS_TOP - fh) / 2));

	Container::repaint();
}

/** 領域更新 */
void Window::update()
{
	__g->drawImage(this->_buffer, INSETS_LEFT, INSETS_TOP);
	MonAPI::Message::sendReceive(NULL, guisvrID, MSG_GUISERVER_DRAWWINDOW, getHandle());
}

/** イベント処理 */
void Window::postEvent(Event *event)
{
	if (event->type >= MOUSE_PRESSED && event->type <= MOUSE_MOVED) {
		MouseEvent *me = (MouseEvent*)event;
		int px = me->x - this->x;
		int py = me->y - this->y;
		if (event->type == MOUSE_PRESSED) {
			// 閉じるボタンクリック
			if (4 <= px && px < 17 && 4 <= py && py < 17) {
				isRunning = false;
				dispose();
				return;
			// タイトルバークリック
			} else if (0 <= px && px < this->width && 0 <= py && py < INSETS_TOP) {
				this->state = STATE_MOVING;
				// キャプチャー要求とウィンドウ移動用オブジェクト作成要求
				MessageInfo info;
				MonAPI::Message::sendReceive(NULL, guisvrID, MSG_GUISERVER_MOUSECAPTURE, 
					getHandle(), 1);
				MonAPI::Message::sendReceive(&info, guisvrID, MSG_GUISERVER_CREATEOVERLAP, 
					this->x, this->y, MAKE_DWORD(this->width, this->height));
				this->overlap = info.arg2;
				this->preX = px;
				this->preY = py;
			// ウィンドウ内クリック
			} else {
				// 絶対座標→相対座標
				me->x = px - INSETS_LEFT;
				me->y = py - INSETS_TOP;
				Container::postEvent(me);
			}
		} else if (event->type == MOUSE_RELEASED) {
			// タイトルバーリリース
			if (this->state == STATE_MOVING) {
				this->state = STATE_NORMAL;
				// キャプチャー破棄要求とウィンドウ移動用オブジェクト破棄要求
				MonAPI::Message::sendReceive(NULL, guisvrID, MSG_GUISERVER_DISPOSEOVERLAP, 
					this->overlap);
				MonAPI::Message::sendReceive(NULL, guisvrID, MSG_GUISERVER_MOUSECAPTURE, 
					getHandle(), 0);
				this->overlap = 0;
				// ウィンドウを実際に移動させる
				setLocation(me->x - this->preX, me->y - this->preY);
			// ウィンドウ内リリース
			} else {
				// 絶対座標→相対座標
				me->x = px - INSETS_LEFT;
				me->y = py - INSETS_TOP;
				Container::postEvent(me);
			}
		} else if (event->type == MOUSE_DRAGGED) {
			// ウィンドウ移動
			if (this->state == STATE_MOVING) {
				// ウィンドウ移動用オブジェクトの移動
				MonAPI::Message::sendReceive(NULL, guisvrID, MSG_GUISERVER_MOVEOVERLAP, this->overlap,
					MAKE_DWORD(me->x - this->preX, me->y - this->preY), 
					MAKE_DWORD(this->width, this->height));
			// ウィンドウ内移動
			} else {
				// 絶対座標→相対座標
				me->x = px - INSETS_LEFT;
				me->y = py - INSETS_TOP;
				Container::postEvent(me);
			}
		}
	} else {
		Container::postEvent(event);
	}
}

/** スレッド開始 */
void Window::run()
{
	create();
	setFocused(true);
	repaint();

	MessageInfo info;
	this->isRunning = true;
	while (this->isRunning) {
		if (!MonAPI::Message::receive(&info)) {
		//if (!MonAPI::Message::peek(&info, 0, PEEK_REMOVE)) {
			switch(info.header){
			case MSG_KEY_VIRTUAL_CODE:
			{
				int key = 0;
				int keycode  = info.arg1;
				int modcode  = info.arg2;
				int charcode = info.arg3;
				
				// 修飾キーの判別
				if ((modcode & KEY_MODIFIER_DOWN) == KEY_MODIFIER_DOWN) {
					if ((modcode & KEY_MODIFIER_SHIFT) == KEY_MODIFIER_SHIFT) {
						this->modifiers = VKEY_LSHIFT;
					} else if ((modcode & KEY_MODIFIER_ALT) == KEY_MODIFIER_ALT) {
						this->modifiers = VKEY_ALT;
					} else if ((modcode & KEY_MODIFIER_CTRL) == KEY_MODIFIER_CTRL) {
						this->modifiers = VKEY_CTRL;
					}
				} else if ((modcode & KEY_MODIFIER_UP) == KEY_MODIFIER_UP) {
					this->modifiers = 0;
				}
				
				// 一般キーの判定
				if (keycode == 33 || keycode == 105) {
					key = VKEY_PGUP;
				} else if (keycode == 34 || keycode == 99) {
					key = VKEY_PGDOWN;
				} else if (keycode == 36 || keycode == 103) {
					key = VKEY_HOME;
				} else if (keycode == 35 || keycode == 97) {
					key = VKEY_END;
				} else if (keycode == 38 || keycode == 104) {
					key = VKEY_UP;
				} else if (keycode == 40 || keycode == 98) {
					key = VKEY_DOWN;
				} else if (keycode == 37 || keycode == 100) {
					key = VKEY_LEFT;
				} else if (keycode == 39 || keycode == 102) {
					key = VKEY_RIGHT;
				} else if (keycode == 45 || keycode == 96) {
					key = VKEY_INSERT;
				} else if (keycode == 13) {
					key = VKEY_ENTER;
				} else if (keycode == 9) {
					key = VKEY_TAB;
				} else if (keycode == 8) {
					key = VKEY_BACKSPACE;
				} else if (keycode == 46 || keycode == 110) {
					key = VKEY_DELETE;
				} else {
					key = charcode;
				}
				
				// キーコードが０ならイベントを投げない
				if (key > 0) {
					this->keyEvent.keycode = key;
					this->keyEvent.modifiers = this->modifiers;
					if (info.arg2 & KEY_MODIFIER_DOWN) {
						this->keyEvent.type = KEY_PRESSED;
					} else if (info.arg2 & KEY_MODIFIER_UP) {
						this->keyEvent.type = KEY_RELEASED;
					}
					postEvent(&this->keyEvent);
				}
				
				break;
			}
			case MSG_MOUSE_INFO:
			{
				monapi_call_mouse_set_cursor(0);
				
				this->mouseEvent.x = info.arg1;
				this->mouseEvent.y = info.arg2;
				
				if (info.arg3 != 0){
					// マウスドラッグ
					if (this->mouseEvent.button != 0) {
						this->mouseEvent.type = MOUSE_DRAGGED;
						//syscall_print("D");
					// マウスプレス
					} else {
						this->mouseEvent.type = MOUSE_PRESSED;
						this->mouseEvent.button = info.arg3;
						//syscall_print("P");
					}
				} else {
					// マウスリリース
					if (this->mouseEvent.button != 0) {
						this->mouseEvent.type = MOUSE_RELEASED;
						this->mouseEvent.button = 0;
						//syscall_print("R");
					// マウス移動
					} else {
						this->mouseEvent.type = MOUSE_MOVED;
						//syscall_print("M");
					}
				}
				postEvent(&this->mouseEvent);
				//this->mouseEvent.x = info.arg1 - this->x - INSETS_LEFT;
				//this->mouseEvent.y = info.arg2 - this->y - INSETS_TOP;
				//Container::postEvent(&this->mouseEvent);
				
				monapi_call_mouse_set_cursor(1);
				
				break;
			}
			case MSG_GUISERVER_ACTIVATED:
				setFocused(true);
				repaint();
				break;
			case MSG_GUISERVER_DEACTIVATE:
				setFocused(false);
				repaint();
				break;
			case TIMER:
				postEvent(&this->timerEvent);
				break;
			default:
				this->customEvent.header = info.header;
				this->customEvent.arg1   = info.arg1;
				this->customEvent.arg2   = info.arg2;
				this->customEvent.arg3   = info.arg3;
				this->customEvent.from   = info.from;
				strcpy(this->customEvent.str, info.str);
				this->customEvent.length = info.length;
				postEvent(&this->customEvent);
				break;
			}
		}
	}
}
