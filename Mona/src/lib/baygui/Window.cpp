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

#include "baygui.h"

#ifdef SDL
/** スクリーンオブジェクト */
SDL_Surface* screen;

/** タイマーID */
static SDL_TimerID globalTimerID;

/** タイマーコールバック関数 */
static unsigned int sdl_timer_callback(unsigned int interval, void* param) {
	SDL_Event event;
	SDL_RemoveTimer(globalTimerID);
	event.type = SDL_USEREVENT;
	return SDL_PushEvent(&event);
}
#endif

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

Window::Window()
{
#ifdef MONA
	// GUIサーバーに自分を登録する
	if (!monapi_register_to_server(ID_GUI_SERVER, MONAPI_TRUE)) {
		printf("%s:%d:ERROR: can not register to GUI server!\n", __FILE__, __LINE__);
		exit(1);
	}
#endif

	this->title = "window";
	this->modifiers = this->state = this->preX = this->preY = this->offsetX = this->offsetY = 0;
	this->overlap = 0;
	this->isRunning = false;
	this->timerID = 0;
	this->_g = NULL;
	this->__g = NULL;
	this->_buffer = NULL;
	this->__buffer = NULL;
	
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

#ifdef MONA
	// ウィンドウを生成する
	MessageInfo msg;
	if (MonAPI::Message::sendReceive(&msg, getGuisvrID(), MSG_GUISERVER_CREATEWINDOW) != 0) {
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
	if (MonAPI::Message::sendReceive(NULL, getGuisvrID(), MSG_GUISERVER_ACTIVATEWINDOW, getHandle())) {
		printf("%s:%d:ERROR: can not activate window!\n", __FILE__, __LINE__);
	}
#else
	setFocused(true);
#endif
}

void Window::onExit()
{
	setVisible(false);
	
	delete(this->_buffer);
	delete(this->_g);
	delete(this->__buffer);
	delete(this->__g);

#ifdef MONA
	// ウィンドウ破棄要求
	if (MonAPI::Message::sendReceive(NULL, getGuisvrID(), MSG_GUISERVER_DISPOSEWINDOW, getHandle())) {
		printf("%s:%d:ERROR: can not connect to GUI server!\n", __FILE__, __LINE__);
	}
	
	// タイマー停止
	kill_timer(timerID);
	
	// GUIサーバーから自分を抹消する
	monapi_register_to_server(ID_GUI_SERVER, MONAPI_FALSE);
#endif
}

Graphics *Window::getGraphics()
{
#ifdef MONA
	if ((this->_window->Flags & WINDOWFLAGS_NOBORDER) == WINDOWFLAGS_NOBORDER) {
		return this->__g;
	} else {
		return this->_g;
	}
#else
	return this->_g;
#endif
}

Image *Window::getBuffer()
{
#ifdef MONA
	if ((this->_window->Flags & WINDOWFLAGS_NOBORDER) == WINDOWFLAGS_NOBORDER) {
		return this->__buffer;
	} else {
		return this->_buffer;
	}
#else
	return this->_buffer;
#endif
}

void Window::setTitle(char *title)
{
	this->title = title;
}

void Window::setVisible(bool visible)
{
	Control::setVisible(visible);
#ifdef MONA
	this->_window->Visible = visible;
#endif
	update();
}

void Window::setLocation(int x, int y)
{
	Control::setLocation(x, y);
#ifdef MONA
	if (this->_window == NULL) return;
	MonAPI::Message::sendReceive(NULL, getGuisvrID(), MSG_GUISERVER_MOVEWINDOW, 
		getHandle(), (unsigned int)x, (unsigned int)y);
#endif
	update();
}

void Window::setTimer(int duration)
{
	// 非活性のときはタイマーを発生させない
	if (getEnabled() == false) return;
	
#if defined(MONA)
	if (duration < 10) duration = 10;
	timerID = set_timer(duration);
#elif defined(SDL)
	if (duration <= 0) duration = 1;
	globalTimerID = SDL_AddTimer(duration, sdl_timer_callback, (void*)1);
#endif
}

void Window::repaint()
{
	if (this->__g == NULL) return;
	
#ifdef MONA
	if ((this->_window->Flags & WINDOWFLAGS_NOBORDER) != WINDOWFLAGS_NOBORDER) {
#endif
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
#ifdef MONA
	}
#endif
	Container::repaint();
}

void Window::update()
{
#ifdef MONA
	if ((this->_window->Flags & WINDOWFLAGS_NOBORDER) != WINDOWFLAGS_NOBORDER) {
		__g->drawImage(this->_buffer, INSETS_LEFT, INSETS_TOP);
	}
	
	MonAPI::Message::sendReceive(NULL, getGuisvrID(), MSG_GUISERVER_DRAWWINDOW, getHandle());
#else
	__g->drawImage(this->_buffer, INSETS_LEFT, INSETS_TOP);
	#ifdef SDL
	{
		SDL_Surface *bitmap;
		
		int x = getX();
		int y = getY();
		int w = getWidth();
		int h = getHeight();
		
		/* bitmapを作成 */
		// flags, w, h, bpp, pit, rmask, gmask, bmask, amask
		bitmap = SDL_CreateRGBSurfaceFrom((void *)this->__buffer->getSource(), w, h, 32, w * 4, 0xff0000, 0xff00, 0xff, 0x0);
		/* 透過色を設定する */
		//SDL_SetColorKey(bitmap, SDL_SRCCOLORKEY, SDL_MapRGB(bitmap->format, 255, 255, 255));
		/* 矩形を作る */
		SDL_Rect rect;
		rect.x = x;
		rect.y = y;
		/* screenにbitmapを描画 */
		SDL_BlitSurface(bitmap, NULL, screen, &rect);
		/* screenの全領域を更新 */
		SDL_UpdateRect(screen, x, y, w, h);
		/* bitmapを解放する */
		SDL_FreeSurface(bitmap);
	}
	#endif
#endif
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
			#ifdef MONA
				// キャプチャー要求とウィンドウ移動用オブジェクト作成要求
				MessageInfo info;
				MonAPI::Message::sendReceive(NULL, getGuisvrID(), MSG_GUISERVER_MOUSECAPTURE, 
					getHandle(), 1);
				MonAPI::Message::sendReceive(&info, getGuisvrID(), MSG_GUISERVER_CREATEOVERLAP, 
					getX(), getY(), MAKE_DWORD(getWidth(), getHeight()));
				this->overlap = info.arg2;
			#endif
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
			#ifdef MONA
				// キャプチャー破棄要求とウィンドウ移動用オブジェクト破棄要求
				MonAPI::Message::sendReceive(NULL, getGuisvrID(), MSG_GUISERVER_DISPOSEOVERLAP, 
					this->overlap);
				MonAPI::Message::sendReceive(NULL, getGuisvrID(), MSG_GUISERVER_MOUSECAPTURE, 
					getHandle(), 0);
				this->overlap = 0;
			#endif
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
			#ifdef MONA
				// ウィンドウ移動用オブジェクトの移動
				MonAPI::Message::sendReceive(NULL, getGuisvrID(), MSG_GUISERVER_MOVEOVERLAP, this->overlap,
					MAKE_DWORD(me->getX() - this->preX, me->getY() - this->preY), 
					MAKE_DWORD(getWidth(), getHeight()));
			#endif
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
#if defined(MONA)
	/* 表示開始 */
	onStart();
	
	/* 再描画 */
	repaint();
	
	/* 実行開始 */
	this->isRunning = true;
	
	MessageInfo info;
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
			case MSG_TIMER:
				kill_timer(info.arg1);
				postEvent(&this->timerEvent);
				break;
			default:
				this->customEvent.header = info.header;
				this->customEvent.arg1   = info.arg1;
				this->customEvent.arg2   = info.arg2;
				this->customEvent.arg3   = info.arg3;
				this->customEvent.from   = info.from;
				//strcpy(this->customEvent.str, info.str);
				memcpy(this->customEvent.str, info.str, sizeof(this->customEvent.str));
				this->customEvent.length = info.length;
				postEvent(&this->customEvent);
				// MSG_PROCESS_STDOUT_DATA は SendReceive で送るので必要
				if (info.header == MSG_PROCESS_STDOUT_DATA) {
					MonAPI::Message::reply(&info);
				}
				break;
			}
		}
	}
#elif defined(SDL)
	/* 初期化 */
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) exit(-1);
	
	/* ウィンドウタイトル設定 */
	SDL_WM_SetCaption("BayGUI for SDL", "");
	
	/* キーコード設定 */
	SDL_EnableUNICODE(1);
	
	/* ビデオモード設定 */
	screen = SDL_SetVideoMode(800, 600, 24, SDL_SWSURFACE);
	if (!screen) exit(-1);
	
	/* 背景を塗りつぶす */
	//SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 255, 255, 255));
	
	/* 壁紙表示 */
	SDL_BlitSurface(SDL_LoadBMP("MONAWALL.BMP"), NULL, screen, NULL);
	
	/* 表示開始 */
	onStart();
	
	/* 再描画 */
	repaint();
	
	/* 実行開始 */
	this->isRunning = true;
	
	/* イベントループ */
	while(this->isRunning) {
		SDL_Event event;
		if (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				break;
			} else if (event.type == SDL_KEYDOWN) {
				int key = 0;
				int keycode = event.key.keysym.unicode & 0x7F;
				int modcode = event.key.keysym.mod;
				
				// ESCで終了
				if (keycode == 0x1B) {
					break;
				}
				
				// 修飾キーの処理
				if (modcode == KMOD_NONE) {
					this->modifiers = 0;
				} else if ((modcode & KMOD_LCTRL) || (modcode & KMOD_RCTRL)) {
					this->modifiers = KeyEvent::VKEY_CTRL;
				} else if ((modcode & KMOD_LSHIFT) || (modcode & KMOD_RSHIFT)) {
					this->modifiers = KeyEvent::VKEY_LSHIFT;
				} else if ((modcode & KMOD_LALT) || (modcode & KMOD_RALT)) {
					this->modifiers = KeyEvent::VKEY_ALT;
				} else {
					this->modifiers = modcode;
				}
				
				// キーの処理
				if (' ' <= keycode && keycode <= 'z') {
					key = keycode;
				} else {
					if (event.key.keysym.sym == SDLK_BACKSPACE) {
						key = '?';
					} else if (event.key.keysym.sym == SDLK_TAB) {
						key = '?';
					} else if (event.key.keysym.sym == SDLK_RETURN) {
						key = '?';
					} else if (event.key.keysym.sym == SDLK_DELETE) {
						key = '?';
					} else if (event.key.keysym.sym == SDLK_UP) {
						key = '?';
					} else if (event.key.keysym.sym == SDLK_RIGHT) {
						key = '?';
					} else if (event.key.keysym.sym == SDLK_DOWN) {
						key = '?';
					} else if (event.key.keysym.sym == SDLK_LEFT) {
						key = '?';
					} else if (event.key.keysym.sym == SDLK_INSERT) {
						key = '?';
					} else if (event.key.keysym.sym == SDLK_HOME) {
						key = '?';
					} else if (event.key.keysym.sym == SDLK_END) {
						key = '?';
					} else if (event.key.keysym.sym == SDLK_PAGEUP) {
						key = '?';
					} else if (event.key.keysym.sym == SDLK_PAGEDOWN) {
						key = '?';
					} else {
						key = 0;
					}
				}
				//_P<KeyEvent> e = new KeyEvent(this, KeyEvent::KEY_PRESSED, key, this->modifiers);
				//processKeyEvent(e.get());
			} else if (event.type == SDL_KEYUP) {
				//this->modifiers = 0;
				//_P<KeyEvent> e = new KeyEvent(this, KeyEvent::KEY_RELEASED, key, this->modifiers);
				//processKeyEvent(e.get());
			} else if (event.type == SDL_MOUSEBUTTONDOWN) {
				int buttons = 0;
				if (event.button.button == SDL_BUTTON_LEFT) {
					buttons = 1;
				} else if (event.button.button == SDL_BUTTON_MIDDLE) {
					buttons = 2;
				} else if (event.button.button == SDL_BUTTON_RIGHT) {
					buttons = 3;
				}
				//_P<MouseEvent> e = new MouseEvent(
				//	this, MouseEvent::MOUSE_PRESSED, event.button.x, event.button.y, buttons);
			} else if (event.type == SDL_MOUSEBUTTONUP) {
				int buttons = 0;
				if (event.button.button == SDL_BUTTON_LEFT) {
					buttons = 1;
				} else if (event.button.button == SDL_BUTTON_MIDDLE) {
					buttons = 2;
				} else if (event.button.button == SDL_BUTTON_RIGHT) {
					buttons = 3;
				}
				//_P<MouseEvent> e = new MouseEvent(
				//	this, MouseEvent::MOUSE_RELEASED, event.button.x, event.button.y, buttons);
			} else if (event.type == SDL_MOUSEMOTION) {
				if (event.motion.state == SDL_PRESSED) {
					//_P<MouseEvent> e = new MouseEvent(
					//	this, MouseEvent::MOUSE_DRAGGED, event.button.x, event.button.y, 1);
				} else if (event.motion.state == SDL_RELEASED) {
					//_P<MouseEvent> e = new MouseEvent(
					//	this, MouseEvent::MOUSE_MOVED, event.button.x, event.button.y, 1);
				}
			} else if (event.type == SDL_USEREVENT) {
				postEvent(&this->timerEvent);
			}
		}
	}
	
	/* 終了 */
	SDL_Quit();
#endif // SDL
}
