/*
Copyright (c) 2004 bayside
All rights reserved.

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

#include "resources.h"
#include "ShortCut.h"
#include "FileManager.h"
#include "BitmapViewer.h"
#include "TextViewer.h"
#include "WindowManager.h"

/** モナーアイコン（パレット） */
static unsigned int monaIconPalette [16] = {
	0xffc8c8c8,
	0xfffefefe,
	0xff000000,
	0xfffc6604,
};

/** モナーアイコン（データ） */
static char monaIconData [15][16] = {
	{0x0,0x0,0x0,0x0,0x2,0x0,0x0,0x0,0x0,0x0,0x2,0x0,0x0,0x0,0x0,0x0},
	{0x0,0x0,0x0,0x2,0x1,0x2,0x0,0x0,0x0,0x2,0x1,0x2,0x0,0x0,0x0,0x0},
	{0x0,0x0,0x0,0x2,0x1,0x2,0x0,0x0,0x0,0x2,0x1,0x2,0x0,0x0,0x0,0x0},
	{0x0,0x0,0x2,0x1,0x1,0x1,0x2,0x2,0x2,0x1,0x1,0x1,0x2,0x0,0x0,0x0},
	{0x0,0x0,0x2,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x2,0x0,0x0},
	{0x0,0x2,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x2,0x0},
	{0x0,0x2,0x1,0x1,0x1,0x2,0x1,0x1,0x1,0x1,0x1,0x2,0x1,0x1,0x2,0x0},
	{0x2,0x1,0x1,0x1,0x2,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x2,0x1,0x1,0x2},
	{0x2,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x2},
	{0x2,0x1,0x1,0x1,0x1,0x1,0x2,0x1,0x1,0x1,0x2,0x1,0x1,0x1,0x1,0x2},
	{0x2,0x1,0x1,0x1,0x1,0x1,0x1,0x2,0x2,0x2,0x1,0x1,0x1,0x1,0x1,0x2},
	{0x0,0x2,0x1,0x1,0x1,0x1,0x1,0x2,0x3,0x2,0x1,0x1,0x1,0x1,0x2,0x0},
	{0x0,0x2,0x1,0x1,0x1,0x1,0x1,0x1,0x2,0x1,0x1,0x1,0x1,0x1,0x2,0x0},
	{0x0,0x0,0x2,0x2,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x2,0x0,0x0},
	{0x0,0x0,0x0,0x0,0x2,0x2,0x2,0x2,0x2,0x2,0x2,0x2,0x2,0x0,0x0,0x0},
};

WindowManager::WindowManager()
{
	setRect(0,0,800,600);
	setTitle("GLAUNCH");
	setTransColor(COLOR_WHITE);
}

WindowManager::~WindowManager()
{
}

/** 初期化時に呼ばれる */
void WindowManager::onStart()
{
	Window::onStart();
	this->_object->Flags |= WINDOWFLAGS_BOTTOMMOST | WINDOWFLAGS_NOBORDER;
	this->_object->__internal2 = true;
}

/** 内部領域描画 */
void WindowManager::drawInternal()
{
	_P<Graphics> g = new Graphics(this->buffer);
	int w = this->getWidth(), h = this->getHeight();
	
	// メニューバー
	g->setColor(200,200,200);
	g->fillRect(0,0,w,20);
	g->setColor(128,128,128);
	g->drawLine(0,20,w-1,20);
	g->setColor(0,0,0);
	g->drawLine(0,21,w-1,21);

	// 左上
	g->drawLine(0,0,4,0);
	g->drawLine(0,1,2,1);
	g->drawLine(0,2,1,2);
	g->drawLine(0,3,0,3);
	g->drawLine(0,4,0,4);
	
	// 右上
	g->drawLine(w-5,0,w-1,0);
	g->drawLine(w-3,1,w-1,1);
	g->drawLine(w-2,2,w-1,2);
	g->drawLine(w-1,3,w-1,3);
	g->drawLine(w-1,4,w-1,4);

	// 左下
	g->drawLine(0,h-5,0,h-5);
	g->drawLine(0,h-4,0,h-4);
	g->drawLine(0,h-3,1,h-3);
	g->drawLine(0,h-2,2,h-2);
	g->drawLine(0,h-1,4,h-1);

	// 右下
	g->drawLine(w-5,h-1,w-1,h-1);
	g->drawLine(w-3,h-2,w-1,h-2);
	g->drawLine(w-2,h-3,w-1,h-3);
	g->drawLine(w-1,h-4,w-1,h-4);
	g->drawLine(w-1,h-5,w-1,h-5);
	
	// モナーアイコン
	for (int i = 0; i < 15; i++) {
		for (int j = 0; j < 16; j++) {
			g->drawPixel(18 + j, 4 + i, monaIconPalette[(int)(monaIconData[i][j])]);
		}
	}
	
	// メニュー
	g->setColor(128,128,128);
	g->drawText(WINDOWMANAGER_MENU_TITLE_JP, 45, 6);
	
	g->dispose();
}

/** 二重起動チェック */
static bool doubleStartCheck()
{
	syscall_set_ps_dump();

	bool result = false;
	dword launcherID = syscall_get_tid();

	PsInfo info;
	while (syscall_read_ps_dump(&info) == 0) {
		if (!result && strcmp(APP_NAME,info.name) == 0 && launcherID != info.tid) result = true;
	}

	return result;
}

int MonaMain(List<char*>* pekoe) {
	if (pekoe->size() == 0) {
		// 二重起動チェック
		if (doubleStartCheck() != false) {
			printf("%s:%d:ERROR: can not start twice!\n", __FILE__, __LINE__);
			return 0;
		}
		
		// ライブラリ初期化
		Application::initialize();
		
		// 「Mona FDD」アイコン
		_P<ShortCut> root = new ShortCut();
		root->setLocation(720,23);
		root->setTitle(FILE_TITLE);
		root->setType(iconsFloppy);
		root->setVisible(true);
		
		// 「た～みなる」アイコン
		_P<ShortCut> term = new ShortCut();
		term->setLocation(720,77);
		term->setTitle(TERMINAL_TITLE);
		term->setType(iconsTerminal);
		term->setVisible(true);
		
		// 「ゴミ箱」アイコン
		_P<ShortCut> trash = new ShortCut();
		trash->setLocation(720,545);
		trash->setTitle(TRASHBOX_TITLE);
		trash->setType(iconsTrashbox);
		trash->setVisible(true);
		
		Application::run(new WindowManager());
		Application::dispose();
	} else if (pekoe->size() == 1) {
		char *arg1 = pekoe->get(0);
		// ファイルマネージャ
		if (strcmp(FILE_NAME, arg1) == 0) {
			Application::initialize();
			Application::run(new FileManager());
			Application::dispose();
		}
	} else if (pekoe->size() == 2) {
		char *arg1 = pekoe->get(1);
		char *arg2 = pekoe->get(0);
		// ビットマップビューア
		if (strcmp(BITMAP_NAME, arg1) == 0) {
			Application::initialize();
			_P<Bitmap> picture = new Bitmap(arg2);
			if (picture->get() == NULL || picture->getWidth() == 0) {
				printf("%s:%d:ERROR: can not open bitmap!\n", __FILE__, __LINE__);
			} else {
				Application::run(new BitmapViewer(arg2, picture));
			}
			Application::dispose();
		// テキストビューア
		} else if (strcmp(TEXT_NAME, arg1) == 0) {
			Application::initialize();
			Application::run(new TextViewer(arg2));
			Application::dispose();
		}
	}
	return 0;
}
