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
#include "IconData.h"

ShortCut::ShortCut()
{
	setSize(ARRANGE_WIDTH, 49);
	setTransColor(0xff8ebb8a);
}

ShortCut::~ShortCut()
{
}

/** 初期化時に呼ばれる */
void ShortCut::onStart()
{
	Window::onStart();
	
	this->_object->Flags |= WINDOWFLAGS_BOTTOMMOST | WINDOWFLAGS_NOBORDER;
	this->_object->__internal2 = true;
}

/** アイコンの種類を設定する */
void ShortCut::setType(int type)
{
	if (this->type == type) return;
	
	this->type = type;
	if (this->getVisible()) this->repaint();
}

/** イベントハンドラ */
void ShortCut::onEvent(Event *e)
{
	if (e->type == MOUSE_RELEASED) {
		if (this->type == iconsFloppy) {
			// ファイルウィンドウを開く
			monapi_call_process_execute_file(FILE_FULLNAME, MONAPI_FALSE);
		} else if (this->type == iconsTerminal) {
			// 端末エミュレータを開く
			monapi_call_process_execute_file(TERMINAL_FULLNAME, MONAPI_FALSE);
		}
	}
}

/** 内部領域描画 */
void ShortCut::drawInternal()
{
	_P<Graphics> g = new Graphics(this->buffer);
	
	// 背景を透過色で塗りつぶす
	g->setColor(0xff8ebb8a);
	g->fillRect(0, 0, this->getWidth(), this->getHeight());
	_P<FontMetrics> fm = new FontMetrics();
	
	// アイコン
	int x = (ARRANGE_WIDTH - 32) / 2;
	int y = 0;
	IconData::drawIcon(g, this->type, x, y);
	
	// タイトル
	int fw = fm->getWidth(this->getTitle());
	g->setColor(COLOR_WHITE);
	g->fillRect((ARRANGE_WIDTH - fw - 8) / 2, 34, fw + 8, 12);
	g->setColor(COLOR_BLACK);
	g->drawText(this->getTitle(), (ARRANGE_WIDTH - fw - 8) / 2 + 4, 34);
	
	g->dispose();
}
