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


#include <baygui.h>

#define SCREEN_W 160
#define SCREEN_H 100

extern "C" int DrawFire();

int typeDraw = 0, fireSeed = 0x1234;
unsigned char calc, *fireScreen, *pImage;

/** クラス宣言 */
class GFire : public Window {
public:
	unsigned char *pbuf;
	GFire::GFire();
	GFire::~GFire();
	void draw();
	void onEvent(Event *e);
	void onPaint(Graphics *g);
};


/** アプリケーションインスタンス */
static GFire *fire = NULL;

/** 描画スレッドID */
static dword drawThreadID = THREAD_UNKNOWN;

/** 描画スレッド */
static void DrawThread()
{
	MonAPI::Message::send(drawThreadID, MSG_SERVER_START_OK);

	while (1) {
		if (DrawFire() != 0) {
			fire->draw();
		}
	}
}

/** コンストラクタ */
GFire::GFire() {
	setRect((800 - SCREEN_W - 12) / 2, (600 - SCREEN_H - 28) / 2, SCREEN_W + 12, SCREEN_H + 28);
	setTitle("FIRE EFFECT");
	fireScreen = (unsigned char *)malloc(0x2300);
	pImage = (unsigned char *)malloc(SCREEN_W * SCREEN_H * 3);
}

/** デストラクタ */
GFire::~GFire() {
	syscall_kill_thread(drawThreadID);
	free(fireScreen);
	free(pImage);
}

/** ウィンドウ内部領域のみの描画 */
void GFire::draw() {
	for (int y = 0; y < SCREEN_H; y++) {
		for (int x = 0; x < SCREEN_W; x++) {
			int k = (x + y * SCREEN_W) * 3;
			unsigned char r = pImage[k];
			unsigned char g = pImage[k + 1];
			unsigned char b = pImage[k + 2];
			__g->drawPixel(x, y, r << 16 | g << 8 | b);
		}
	}
}

/** イベント処理 */
void GFire::onEvent(Event *e) {
	if (e->type == FOCUS_IN || e->type == DEICONIFIED) {
		// 描画スレッド起動
		if (drawThreadID == THREAD_UNKNOWN) {
			drawThreadID = syscall_get_tid();
			MessageInfo msg, src;
			dword id = syscall_mthread_create((dword)DrawThread);
			syscall_mthread_join(id);
			src.header = MSG_SERVER_START_OK;
			MonAPI::Message::receive(&msg, &src, MonAPI::Message::equalsHeader);
			drawThreadID = msg.from;
		}
	} else if (e->type == FOCUS_OUT || e->type == ICONIFIED) {
		// 描画スレッド停止
		syscall_kill_thread(drawThreadID);
		drawThreadID = THREAD_UNKNOWN;
	}
}

/** 描画 */
void GFire::onPaint(Graphics *g) {
	draw();
}

int MonaMain(List<char*>* pekoe) {
	fire = new GFire();
	fire->run();
	delete(fire);
	return 0;
}
