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
#include "BitmapViewer.h"
#include "Icons.h"
#include "IconData.h"

Icons::Icons()
{
	setSize(ARRANGE_WIDTH, 49);
}

Icons::~Icons()
{
}

/** アイコンの種類を設定する */
void Icons::setType(int type)
{
	if (this->type == type) return;
	
	this->type = type;
	//if (this->getVisible()) this->repaint();
}

/** ラベルを設定する */
void Icons::setLabel(const char* label)
{
	this->label = label;
}

/** コマンド名を設定する */
void Icons::setCommand(const char* command)
{
	this->command = command;
}

/** イベントハンドラ */
void Icons::onEvent(Event *e)
{
	if (e->type == MOUSE_RELEASED) {
		if (this->type == iconsPicture) {
			monapi_call_process_execute_file(getCommand(), MONAPI_FALSE);
		} else if (this->type == iconsText) {
			monapi_call_process_execute_file(getCommand(), MONAPI_FALSE);
		} else if (this->type == iconsExecutable) {
			monapi_call_process_execute_file(getCommand(), MONAPI_FALSE);
		}
	}
}

/** 描画ハンドラ */
void Icons::onPaint(_P<Graphics> g)
{
	// 背景を透過色で塗りつぶす
	g->setColor(COLOR_WHITE);
	g->fillRect(0, 0, this->getWidth(), this->getHeight());
	_P<FontMetrics> fm = new FontMetrics();
	
	// アイコン
	int x = (ARRANGE_WIDTH - 32) / 2;
	int y = 0;
	IconData::drawIcon(g, this->type, x, y);
	
	// ラベル
	int fw = fm->getWidth(this->getLabel());
	g->setColor(COLOR_WHITE);
	g->fillRect((ARRANGE_WIDTH - fw - 8) / 2, 34, fw + 8, 12);
	g->setColor(COLOR_BLACK);
	g->drawText(this->getLabel(), (ARRANGE_WIDTH - fw - 8) / 2 + 4, 34);
}
