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
#include "Icons.h"
#include "FileManager.h"

#define MAX_APP_LEN 12

/** ラベル（ファイルマネージャに表示される）*/
static char *iconLabelList[MAX_APP_LEN] = {
	"..",
	"FIRE.EX2",
	"GBBALL.EX5",
	"GCLOCK.EX5",
	"GHEBTN.APP",
	"GNOIZ2BG.EX2",
	"GNULLPO.APP",
	"GVERSION.EX5",
	"NEKO.JPG",
	"STARTING.JPG",
	"AUTOEXEC.MSH",
	"LICENSE.BZ2",
};

/** コマンド（アイコンがクリックされたときに実行される）*/
static char *iconCommandList[MAX_APP_LEN] = {
	"..",
	"/APPS/BAYGUI/FIRE.EX2",
	"/APPS/BAYGUI/GBBALL.EX5",
	"/APPS/BAYGUI/GCLOCK.EX5",
	"/APPS/BAYGUI/GHEBTN.APP",
	"/APPS/BAYGUI/GNOIZ2BG.EX2",
	"/APPS/BAYGUI/GNULLPO.APP",
	"/APPS/BAYGUI/GVERSION.EX5",
	"/APPS/BAYGUI/GLAUNCH.EX5 BITMAP /NEKO.JPG",
	"/APPS/BAYGUI/GLAUNCH.EX5 BITMAP /STARTING.JPG",
	"/APPS/BAYGUI/GLAUNCH.EX5 TEXT /AUTOEXEC.MSH",
	"/APPS/BAYGUI/GLAUNCH.EX5 TEXT /LICENSE.BZ2",
};

/** アイコンタイプ */
static int iconTypeList[MAX_APP_LEN] = {
	iconsFolder,
	iconsExecutable,
	iconsExecutable,
	iconsExecutable,
	iconsExecutable,
	iconsExecutable,
	iconsExecutable,
	iconsExecutable,
	iconsPicture,
	iconsPicture,
	iconsText,
	iconsText,
};

FileManager::FileManager()
{
	setRect(80, 80, 510, 340);
	setTitle(FILE_TITLE);
	
	// 1行6個
	for (int i = 0; i < MAX_APP_LEN; i++) {
		Icons *icon = new Icons();
		icon->setLocation(ARRANGE_WIDTH * (i % 6), ARRANGE_HEIGHT * (i / 6) + 4);
		icon->setLabel(iconLabelList[i]);
		icon->setCommand(iconCommandList[i]);
		icon->setType(iconTypeList[i]);
		this->add(icon);
	}
}

FileManager::~FileManager()
{
}

/** イベントハンドラ */
void FileManager::onEvent(Event *e)
{ 
}

/** 描画ハンドラ */
void FileManager::onPaint(_P<Graphics> g)
{
	g->setColor(COLOR_WHITE);
	g->fillRect(0, 0, this->getWidth(), this->getHeight());
}
