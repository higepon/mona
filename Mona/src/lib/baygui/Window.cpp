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
	guisvrID = MonAPI::Message::lookupMainThread("BAYGUI.EX5");
	if (guisvrID == 0xFFFFFFFF) {
		//printf("Window: GuiServer not found %d\n", threadID);
	} else {
		//printf("Window: GuiServer found %d\n", threadID);
	}

	// フォントロード
	FontManager::getInstance();
}

/** デストラクタ */
Window::~Window() {
	// draw mouse_cursor
	monapi_call_mouse_set_cursor(1);
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
	// 子領域
	__g->translate(x + INSETS_LEFT, y + INSETS_TOP);
	__g->setClip(x + INSETS_LEFT, y + INSETS_TOP, 
		width - INSETS_LEFT - INSETS_RIGHT, height - INSETS_TOP - INSETS_BOTTOM);

	// 子部品の位置も更新
	for (i = 0; i < controlListPtr; i++) {
		Rect *rect = controlList[i]->getRect();
		controlList[i]->setRect(
			rect->x + mx, 
			rect->y + my, 
			rect->width, 
			rect->height
		);
	}
}

/** イベント処理 */
void Window::postEvent(Event *event)
{
	// 活性部品にキーイベントを投げる
	if (KEY_PRESSED <= event->type && event->type <= KEY_RELEASED) {
		Control *control = getActiveControl();
		// 部品でイベントが起こった
		if (control != NULL) {
			event->source = control;
			// debug
			//WindowManager::getInstance()->//printf("Control::postEvent %x,%d,%d,%d\n", 
			//	event->source, event->type, ((KeyEvent *)event)->key, ((KeyEvent *)event)->modifires);
			control->postEvent(event);
		// 部品以外でイベントが起こった
		} else {
			// debug
			//WindowManager::getInstance()->//printf("Window::onEvent %x,%d,%d,%d\n", 
			//	event->source, event->type, ((KeyEvent *)event)->key, ((KeyEvent *)event)->modifires);
			onEvent(event);
		}
	// マウスクリック、マウスリリース
	} else if (event->type == MOUSE_PRESSED || event->type == MOUSE_RELEASED) {
		// マウスイベントが起こった部品を探す
		int ex = ((MouseEvent *)event)->x;
		int ey = ((MouseEvent *)event)->y;
		Control *control = getActiveControl(ex, ey);
		// 部品でイベントが起こった
		if (control != NULL) {
			setActiveControl(control);
			event->source = control;
			// 座標変換
			((MouseEvent *)event)->x -= (x + INSETS_LEFT);
			((MouseEvent *)event)->y -= (y + INSETS_TOP);
			// debug
			//WindowManager::getInstance()->//printf("Control::postEvent %x,%d,%d,%d\n", 
			//	event->source, event->type, ((MouseEvent *)event)->x, ((MouseEvent *)event)->x);
			control->postEvent(event);
		// 部品以外でイベントが起こった
		} else {
			setActiveControl(NULL);
			// debug
			//WindowManager::getInstance()->//printf("Window::onEvent %x,%d,%d,%d\n", 
			//	event->source, event->type, ((MouseEvent *)event)->x, ((MouseEvent *)event)->x);
			onEvent(event);
		}
	// マウスドラッグ
	} else if (event->type == MOUSE_DRAGGED) {
		Control *control = getActiveControl();
		// 部品でイベントが起こった
		if (control != NULL) {
			// debug
			//WindowManager::getInstance()->//printf("Control::postEvent %x,%d,%d,%d\n", 
			//	event->source, event->type, ((MouseEvent *)event)->x, ((MouseEvent *)event)->x);
			control->postEvent(event);
		// 部品以外でイベントが起こった
		} else {
			// debug
			//WindowManager::getInstance()->//printf("Window::onEvent %x,%d,%d,%d\n", 
			//	event->source, event->type, ((MouseEvent *)event)->x, ((MouseEvent *)event)->x);
			onEvent(event);
		}
	} else {
		// debug
		//WindowManager::getInstance()->//printf("Window::onEvent %x,%d\n", 
		//	event->source, event->type);
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
		_g->setColor(0, 0, 0);
		_g->drawRect(0, 0, width, INSETS_TOP);
		_g->setColor(200, 200, 200);
		_g->fillRect(1, 1, width - 2, INSETS_TOP - 2);
		// 輪郭線
		_g->setColor(255, 255, 255);
		_g->drawLine(1, 1, width - 1, 1);
		_g->drawLine(1, 1, 1, INSETS_TOP - 1);
		_g->setColor(128, 128, 128);
		_g->drawLine(width - 1, 2, width - 1, INSETS_TOP - 1);
		_g->drawLine(2, INSETS_TOP - 1, width - 1, INSETS_TOP - 1);
	} else {
		// 外枠
		_g->setColor(0, 0, 0);
		_g->drawRect(0, 0, width, height);
		_g->setColor(200, 200, 200);
		_g->fillRect(1, 1, width - 2, height - 2);
		// 内枠
		_g->setColor(0,0,0);
		_g->drawRect(5, 21, width - 10, height - 26);
		// 輪郭線
		_g->setColor(255, 255, 255);
		_g->drawLine(1, 1, width - 1, 1);
		_g->drawLine(1, 1, 1, height - 1);
		_g->drawLine(width - 4, 21, width - 4, height - 4);
		_g->drawLine(5, height - 4, width - 4, height - 4);
		_g->setColor(128, 128, 128);
		_g->drawLine(width - 1, 2, width - 1, height - 1);
		_g->drawLine(2, height - 1, width - 1, height - 1);
		_g->drawLine(4, 20, width - 5, 20);
		_g->drawLine(4, 20, 4, height - 5);
	}

	if (enabled == true) {
		// タイトルライン
		for (i = 5; i <= 15; i = i + 2) {
			_g->setColor(128, 128, 128);
			_g->drawLine(20, i, width - 21, i);
			_g->setColor(255, 255, 255);
			_g->drawLine(21, i + 1, width - 22, i + 1);
		}
		// 閉じるアイコン
		for (i = 0; i < 13; i++) {
			for (j = 0; j < 13; j++) {
				_g->screen->putPixel16(x + 4 + j, y + 5 + i, closeIcon[i][j]);
				_g->screen->putPixel16(x + width - 16 + j, y + 5 + i, minimizeIcon[i][j]);
			}
		}
	} else {
		// NOP
	}

	// タイトル
	int fw = FontManager::getInstance()->getWidth(title);
	int fh = FontManager::getInstance()->getHeight();
	_g->setColor(200, 200, 200);
	_g->fillRect(((width - fw) / 2) - 4, 2, fw + 8, INSETS_TOP - 4);
	if (enabled == true) {
		_g->setColor(0, 0, 0);
	} else {
		_g->setColor(128, 128, 128);
	}
	_g->drawText(title, ((width - fw) / 2), ((INSETS_TOP - fh) / 2));
	//_g->drawText(title, ((width - fw) / 2) + 1, ((INSETS_TOP - fh) / 2));

	if (iconified == false) {
		// 子部品も更新
		for (i = 0; i < controlListPtr; i++) {
			controlList[i]->repaint();
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

	while (1) {
		if(!MonAPI::Message::receive(&info)){
		//if(!MonAPI::Message::peek(&info, 0, PEEK_REMOVE)){
			// draw mouse_cursor
			monapi_call_mouse_set_cursor(0);
			
			switch(info.header){
			case MSG_GUISERVER_ONKEYPRESS:
				//printf("WindowManager->Window MSG_GUISERVER_ONKEYPRESS received %d,%d,%d\n", info.arg1, info.arg2, info.arg3);
				{
					KeyEvent *event = new KeyEvent(KEY_PRESSED, this, info.arg1, 0);
					postEvent(event);
					delete(event);
				}
				break;
			case MSG_GUISERVER_ONKEYRELEASE:
				//printf("WindowManager->Window MSG_GUISERVER_ONKEYRELEASE received %d,%d,%d\n", info.arg1, info.arg2, info.arg3);
				{
					KeyEvent *event = new KeyEvent(KEY_RELEASED, this, info.arg1, 0);
					postEvent(event);
					delete(event);
				}
				break;
			case MSG_GUISERVER_ONMOUSEPRESS:
				//printf("WindowManager->Window MSG_GUISERVER_ONMOUSEPRESS received %d,%d,%d\n", info.arg1, info.arg2, info.arg3);
				{
					MouseEvent *event = new MouseEvent(MOUSE_PRESSED, this, info.arg1, info.arg2);
					postEvent(event);
					delete(event);
				}
				break;
			case MSG_GUISERVER_ONMOUSEDRAG:
				//printf("WindowManager->Window MSG_GUISERVER_ONMOUSEDRAG received %d,%d,%d\n", info.arg1, info.arg2, info.arg3);
				{
					MouseEvent *event = new MouseEvent(MOUSE_DRAGGED, this, info.arg1, info.arg2);
					postEvent(event);
					delete(event);
				}
				break;
			case MSG_GUISERVER_ONMOUSERELEASE:
				//printf("WindowManager->Window MSG_GUISERVER_ONMOUSERELEASE received %d,%d,%d\n", info.arg1, info.arg2, info.arg3);
				{
					MouseEvent *event = new MouseEvent(MOUSE_RELEASED, this, info.arg1, info.arg2);
					postEvent(event);
					delete(event);
				}
				break;
			case MSG_GUISERVER_ONMOUSEMOVE:
				//printf("WindowManager->Window MSG_GUISERVER_ONMOUSEMOVE received %d,%d,%d\n", info.arg1, info.arg2, info.arg3);
				{
					MouseEvent *event = new MouseEvent(MOUSE_MOVED, this, info.arg1, info.arg2);
					postEvent(event);
					delete(event);
				}
				break;
			case MSG_GUISERVER_ONTIMER:
				//printf("WindowManager->Window MSG_GUISERVER_ONTIMER received\n");
				postEvent(_timerEvent);
				_timerEvent->setDuration(-1);
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
			case MSG_GUISERVER_ICONIFIED:
				//printf("WindowManager->Window MSG_GUISERVER_ICONIFIED received %d\n", threadID);
				setIconified(true);
				//MonAPI::Message::reply(&info);
				break;
			case MSG_GUISERVER_DEICONIFIED:
				//printf("WindowManager->Window MSG_GUISERVER_DEICONIFIED received %d\n", threadID);
				setIconified(false);
				//MonAPI::Message::reply(&info);
				break;
			case MSG_GUISERVER_REMOVE:
				//printf("WindowManager->Window MSG_GUISERVER_REMOVE received %d\n", threadID);
				delete(this);
				exit(0);
				break;
			default:
				break;
			}

			// draw mouse_cursor
			monapi_call_mouse_set_cursor(1);
		}
	}
}
