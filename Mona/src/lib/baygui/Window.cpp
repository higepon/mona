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
			if (info.header == Event::TIMER && info.arg2 > 0) {
				int duration = (info.arg2 < 10) ? 1 : (info.arg2 / 10);
				syscall_sleep(duration);
				MonAPI::Message::send(info.arg1, Event::TIMER, 0, 0, 0, NULL);
			}
		}
	}
}

Window::Window()
{
	// GUIサーバーに自分を登録する
	if (!monapi_register_to_server(ID_GUI_SERVER, MONAPI_TRUE)) {
		printf("%s:%d:ERROR: can not register to GUI server!\n", __FILE__, __LINE__);
		exit(1);
	}

	this->title = "window";
	this->modifiers = this->state = this->preX = this->preY = this->offsetX = this->offsetY = 0;
	this->overlap = 0;
	this->isRunning = false;
	
	// キーイベント
	this->keyEvent.setType(KeyEvent::KEY_PRESSED);
	this->keyEvent.setSource(this);
	
	// マウスイベント
	this->mouseEvent.setType(MouseEvent::MOUSE_PRESSED);
	this->mouseEvent.setSource(this);
	
	// タイマーイベント
	this->timerEvent.setType(Event::TIMER);
	this->timerEvent.setSource(this);
	
	// カスタムイベント
	this->customEvent.setType(Event::CUSTOM_EVENT);
	this->customEvent.setSource(this);
	
	// タイマー起動
	if (timerID == THREAD_UNKNOWN) timerID = syscall_get_tid();
	MessageInfo msg, src;
	dword id = syscall_mthread_create((dword)TimerThread);
	syscall_mthread_join(id);
	src.header = MSG_SERVER_START_OK;
	MonAPI::Message::receive(&msg, &src, MonAPI::Message::equalsHeader);
	timerID = msg.from;
}

Window::~Window() {
	onExit();
}

void Window::onStart()
{
	if (this->_buffer != NULL) return;

	// 描画バッファー、描画オブジェクトの生成
	this->_buffer = new Image
		(getWidth() - INSETS_LEFT - INSETS_RIGHT, getHeight() - INSETS_TOP - INSETS_BOTTOM);
	this->_g = new Graphics(this->_buffer);
	this->__buffer = new Image(getWidth(), getHeight());
	this->__g = new Graphics(this->__buffer);

	// ウィンドウを生成する
	MessageInfo msg;
	if (MonAPI::Message::sendReceive(&msg, this->guisvrID, MSG_GUISERVER_CREATEWINDOW) != 0) {
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
	this->_window->X = getX();
	this->_window->Y = getY();
	this->_window->Width  = getWidth();
	this->_window->Height = getHeight();
	this->_window->OffsetX = this->offsetX;
	this->_window->OffsetY = this->offsetY;
	this->_window->BufferHandle = this->_buffer->getHandle();
	this->_window->FormBufferHandle = this->__buffer->getHandle();
	this->_window->TransparencyKey = 0x00000000;
	this->_window->Visible = true;
	this->_window->Opacity = 0xff; // 不透明
	this->_window->__internal2 = true;

	// ウィンドウをアクティブにする
	setFocused(true);
	if (MonAPI::Message::sendReceive(NULL, this->guisvrID, MSG_GUISERVER_ACTIVATEWINDOW, getHandle())) {
		printf("%s:%d:ERROR: can not activate window!\n", __FILE__, __LINE__);
	}
}

void Window::onExit()
{
	setVisible(false);
	
	delete(this->_buffer);
	delete(this->_g);
	delete(this->__buffer);
	delete(this->__g);
	
	// ウィンドウ破棄要求
	if (MonAPI::Message::sendReceive(NULL, this->guisvrID, MSG_GUISERVER_DISPOSEWINDOW, getHandle())) {
		printf("%s:%d:ERROR: can not connect to GUI server!\n", __FILE__, __LINE__);
	}
	
	// タイマースレッド停止
	syscall_kill_thread(timerID);
	
	// GUIサーバーから自分を抹消する
	monapi_register_to_server(ID_GUI_SERVER, MONAPI_FALSE);
}

unsigned int Window::getHandle()
{
	if (this->_window != NULL) {
		return this->_window->Handle;
	} else {
		return 0;
	}
}

Graphics *Window::getGraphics()
{
	if ((this->_window->Flags & WINDOWFLAGS_NOBORDER) == WINDOWFLAGS_NOBORDER) {
		return this->__g;
	} else {
		return this->_g;
	}
}

Image *Window::getBuffer()
{
	if ((this->_window->Flags & WINDOWFLAGS_NOBORDER) == WINDOWFLAGS_NOBORDER) {
		return this->__buffer;
	} else {
		return this->_buffer;
	}
}

void Window::setTitle(char *title)
{
	this->title = title;
}

void Window::setVisible(bool visible)
{
	Control::setVisible(visible);
	this->_window->Visible = visible;
	update();
}

void Window::setLocation(int x, int y)
{
	Control::setLocation(x, y);
	
	if (this->_window == NULL) return;
	
	MonAPI::Message::sendReceive(NULL, this->guisvrID, MSG_GUISERVER_MOVEWINDOW, 
		getHandle(), (unsigned int)x, (unsigned int)y);
	
	update();
}

void Window::setTimer(int duration)
{
	// 非活性のときはタイマーを発生させない
	if (getEnabled() == false) return;
	
	// タイマー設定メッセージを投げる
	if (MonAPI::Message::send(timerID, Event::TIMER, this->threadID, duration, 0, NULL)) {
		printf("%s:%d:ERROR: can not send TIMER\n", __FILE__, __LINE__);
	}
}

void Window::repaint()
{
	if (this->_buffer == NULL) return;
	
	if ((this->_window->Flags & WINDOWFLAGS_NOBORDER) != WINDOWFLAGS_NOBORDER) {
		// 矩形ウィンドウ
		int w = getWidth();
		int h = getHeight();
		
		// 外枠
		__g->setColor(Color::LIGHTGRAY);
		__g->fillRect(0, 0, w, h);
		__g->setColor(Color::BLACK);
		__g->drawRect(0, 0, w, h);
		
		// 内枠
		__g->setColor(Color::BLACK);
		__g->drawRect(5, 21, w - 10, h - 26);
		
		// 枠
		__g->setColor(Color::WHITE);
		__g->drawLine(1, 1, w - 2, 1);
		__g->drawLine(1, 1, 1, h - 2);
		__g->drawLine(w - 5, 21, w - 5, h - 5);
		__g->drawLine(5, h - 5, w - 5, h - 5);
		__g->setColor(Color::GRAY);
		__g->drawLine(w - 2, 2, w - 2, h - 2);
		__g->drawLine(2, h - 2, w - 2, h - 2);
		__g->drawLine(4, 20, w - 6, 20);
		__g->drawLine(4, 20, 4, h - 6);
		
		if (getFocused() == true) {
			// タイトルバー
			for (int i = 4; i <= 14; i = i + 2) {
				__g->setColor(Color::GRAY);
				__g->drawLine(20, i, w - 7, i);
				__g->setColor(Color::WHITE);
				__g->drawLine(21, i + 1, w - 6, i + 1);
			}
			
			// 閉じるボタン
			for (int i = 0; i < 13; i++) {
				for (int j = 0; j < 13; j++) {
					__g->drawPixel(j + 4, i + 4, close_palette[close_data[i * 13 + j] & 0xFF]);
				}
			}
		}

		// タイトル
		int fw = getFontMetrics()->getWidth(getTitle());
		int fh = getFontMetrics()->getHeight(getTitle());
		__g->setColor(Color::LIGHTGRAY);
		__g->fillRect(((w - fw) / 2) - 4, 2, fw + 8, INSETS_TOP - 4);
		if (getFocused() == true) {
			__g->setColor(Color::BLACK);
		} else {
			__g->setColor(Color::GRAY);
		}
		__g->drawText(getTitle(), ((w - fw) / 2), ((INSETS_TOP - fh) / 2));
	}
	Container::repaint();
}

void Window::update()
{
	if ((this->_window->Flags & WINDOWFLAGS_NOBORDER) != WINDOWFLAGS_NOBORDER) {
		__g->drawImage(this->_buffer, INSETS_LEFT, INSETS_TOP);
	}
	MonAPI::Message::sendReceive(NULL, this->guisvrID, MSG_GUISERVER_DRAWWINDOW, getHandle());
}

void Window::postEvent(Event *event)
{
	if (event->getType() >= MouseEvent::MOUSE_PRESSED && event->getType() <= MouseEvent::MOUSE_MOVED) {
		MouseEvent *me = (MouseEvent*)event;
		int px = me->getX() - getX();
		int py = me->getY() - getY();
		if (event->getType() == MouseEvent::MOUSE_PRESSED) {
			// 閉じるボタンクリック
			if (4 <= px && px < 17 && 4 <= py && py < 17) {
				stop();
				return;
			// タイトルバークリック
			} else if (0 <= px && px < getWidth() && 0 <= py && py < INSETS_TOP) {
				this->state = STATE_MOVING;
				// キャプチャー要求とウィンドウ移動用オブジェクト作成要求
				MessageInfo info;
				MonAPI::Message::sendReceive(NULL, this->guisvrID, MSG_GUISERVER_MOUSECAPTURE, 
					getHandle(), 1);
				MonAPI::Message::sendReceive(&info, this->guisvrID, MSG_GUISERVER_CREATEOVERLAP, 
					getX(), getY(), MAKE_DWORD(getWidth(), getHeight()));
				this->overlap = info.arg2;
				this->preX = px;
				this->preY = py;
			// ウィンドウ内クリック
			} else {
				// 絶対座標→相対座標
				me->setX(px - INSETS_LEFT);
				me->setY(py - INSETS_TOP);
				Container::postEvent(me);
			}
		} else if (event->getType() == MouseEvent::MOUSE_RELEASED) {
			// タイトルバーリリース
			if (this->state == STATE_MOVING) {
				this->state = STATE_NORMAL;
				// キャプチャー破棄要求とウィンドウ移動用オブジェクト破棄要求
				MonAPI::Message::sendReceive(NULL, this->guisvrID, MSG_GUISERVER_DISPOSEOVERLAP, 
					this->overlap);
				MonAPI::Message::sendReceive(NULL, this->guisvrID, MSG_GUISERVER_MOUSECAPTURE, 
					getHandle(), 0);
				this->overlap = 0;
				// ウィンドウを実際に移動させる
				setLocation(me->getX() - this->preX, me->getY() - this->preY);
			// ウィンドウ内リリース
			} else {
				// 絶対座標→相対座標
				me->setX(px - INSETS_LEFT);
				me->setY(py - INSETS_TOP);
				Container::postEvent(me);
			}
		} else if (event->getType() == MouseEvent::MOUSE_DRAGGED) {
			// ウィンドウ移動
			if (this->state == STATE_MOVING) {
				// ウィンドウ移動用オブジェクトの移動
				MonAPI::Message::sendReceive(NULL, this->guisvrID, MSG_GUISERVER_MOVEOVERLAP, this->overlap,
					MAKE_DWORD(me->getX() - this->preX, me->getY() - this->preY), 
					MAKE_DWORD(getWidth(), getHeight()));
			// ウィンドウ内移動
			} else {
				// 絶対座標→相対座標
				me->setX(px - INSETS_LEFT);
				me->setY(py - INSETS_TOP);
				Container::postEvent(me);
			}
		}
	} else {
		Container::postEvent(event);
	}
}

void Window::stop()
{
	this->isRunning = false;
}

void Window::run()
{
	onStart();
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
						this->modifiers = KeyEvent::VKEY_LSHIFT;
					} else if ((modcode & KEY_MODIFIER_ALT) == KEY_MODIFIER_ALT) {
						this->modifiers = KeyEvent::VKEY_ALT;
					} else if ((modcode & KEY_MODIFIER_CTRL) == KEY_MODIFIER_CTRL) {
						this->modifiers = KeyEvent::VKEY_CTRL;
					}
				} else if ((modcode & KEY_MODIFIER_UP) == KEY_MODIFIER_UP) {
					this->modifiers = 0;
				}
				
				// 一般キーの判定
				if (keycode == 33 || keycode == 105) {
					key = KeyEvent::VKEY_PGUP;
				} else if (keycode == 34 || keycode == 99) {
					key = KeyEvent::VKEY_PGDOWN;
				} else if (keycode == 36 || keycode == 103) {
					key = KeyEvent::VKEY_HOME;
				} else if (keycode == 35 || keycode == 97) {
					key = KeyEvent::VKEY_END;
				} else if (keycode == 38 || keycode == 104) {
					key = KeyEvent::VKEY_UP;
				} else if (keycode == 40 || keycode == 98) {
					key = KeyEvent::VKEY_DOWN;
				} else if (keycode == 37 || keycode == 100) {
					key = KeyEvent::VKEY_LEFT;
				} else if (keycode == 39 || keycode == 102) {
					key = KeyEvent::VKEY_RIGHT;
				} else if (keycode == 45 || keycode == 96) {
					key = KeyEvent::VKEY_INSERT;
				} else if (keycode == 13) {
					key = KeyEvent::VKEY_ENTER;
				} else if (keycode == 9) {
					key = KeyEvent::VKEY_TAB;
				} else if (keycode == 8) {
					key = KeyEvent::VKEY_BACKSPACE;
				} else if (keycode == 46 || keycode == 110) {
					key = KeyEvent::VKEY_DELETE;
				} else {
					key = charcode;
				}
				
				// キーコードが０ならイベントを投げない
				if (key > 0) {
					this->keyEvent.setKeycode(key);
					this->keyEvent.setModifiers(this->modifiers);
					if (info.arg2 & KEY_MODIFIER_DOWN) {
						this->keyEvent.setType(KeyEvent::KEY_PRESSED);
					} else if (info.arg2 & KEY_MODIFIER_UP) {
						this->keyEvent.setType(KeyEvent::KEY_RELEASED);
					}
					postEvent(&this->keyEvent);
				}
				
				break;
			}
			case MSG_MOUSE_INFO:
			{
				monapi_call_mouse_set_cursor(0);
				
				this->mouseEvent.setX(info.arg1);
				this->mouseEvent.setY(info.arg2);
				
				if (info.arg3 != 0){
					// マウスドラッグ
					if (this->mouseEvent.getButton() != 0) {
						this->mouseEvent.setType(MouseEvent::MOUSE_DRAGGED);
						//syscall_print("D");
					// マウスプレス
					} else {
						this->mouseEvent.setType(MouseEvent::MOUSE_PRESSED);
						this->mouseEvent.setButton(info.arg3);
						//syscall_print("P");
					}
				} else {
					// マウスリリース
					if (this->mouseEvent.getButton() != 0) {
						this->mouseEvent.setType(MouseEvent::MOUSE_RELEASED);
						this->mouseEvent.setButton(0);
						//syscall_print("R");
					// マウス移動
					} else {
						this->mouseEvent.setType(MouseEvent::MOUSE_MOVED);
						//syscall_print("M");
					}
				}
				postEvent(&this->mouseEvent);
				//this->mouseEvent.x = info.arg1 - getX() - INSETS_LEFT;
				//this->mouseEvent.y = info.arg2 - getY() - INSETS_TOP;
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
			case Event::TIMER:
				postEvent(&this->timerEvent);
				break;
			default:
				#ifdef MONA
				this->customEvent.header = info.header;
				this->customEvent.arg1   = info.arg1;
				this->customEvent.arg2   = info.arg2;
				this->customEvent.arg3   = info.arg3;
				this->customEvent.from   = info.from;
				strcpy(this->customEvent.str, info.str);
				this->customEvent.length = info.length;
				postEvent(&this->customEvent);
				// MSG_PROCESS_STDOUT_DATA は SendReceive で送るので必要
				if (info.header == MSG_PROCESS_STDOUT_DATA) {
					MonAPI::Message::reply(&info);
				}
				#endif
				break;
			}
		}
	}
}
