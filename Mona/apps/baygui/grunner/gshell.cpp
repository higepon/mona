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
#include "gshell.h"

GShell::GShell()
{
	setRect(0, 22, 6 * TERMINAL_WIDTH + 12, 12 * TERMINAL_HEIGHT + 28);
	setTitle("た～みなる");
	clear();
	insert(TERMINAL_PROMPT);
}

/** バッファーをクリアする */
void GShell::clear()
{
	this->cursor = 0;
	this->rows = 0;
	this->cols = 0;
	memset(this->buffer, 0, TERMINAL_WIDTH * TERMINAL_HEIGHT);
}

/** 文字列を挿入する */
void GShell::insert(const char *text)
{
	for (int i = 0; i < (int)strlen(text); i++) {
		this->buffer[cursor++] = text[i];
		cols++;
	}
}

/** イベントハンドラ */
void GShell::onEvent(Event *e)
{
	if (e->type == KEY_PRESSED) {
		int keycode = ((KeyEvent*)e)->keycode;
		if (keycode == VKEY_BACKSPACE && cols > 5) {
			this->buffer[--cursor] = 0;
			cols--;
		} else if (keycode == VKEY_ENTER) {
			this->buffer[cursor++] = '\n';
			rows++;
			cols = 0;
			insert(TERMINAL_PROMPT);
		} else {
			this->buffer[cursor++] = keycode;
			cols++;
		}
		repaint();
	}
}

/** 描画ハンドラ */
void GShell::onPaint(_P<Graphics> g)
{
	g->setColor(COLOR_WHITE);
	g->fillRect(0, 0, this->getWidth(), this->getHeight());
	g->setColor(COLOR_BLACK);
	g->drawText(this->buffer, 0, 0);
}

int MonaMain(List<char*>* pekoe) {
	Application::initialize();
	_P<Window> shell = new GShell();
	Application::run(shell);
	Application::dispose();
	//GShell *shell = new GShell();
	//shell->run();
	return 0;
}
