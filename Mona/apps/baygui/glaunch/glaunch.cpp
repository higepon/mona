/*
Copyright (c) 2004 bayside
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this history of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this history of conditions and the following disclaimer in the
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

#define MAX_APP_LEN 8

/**
 アプリケーションランチャー
*/
class GLaunch : public Window {
private:
	Button *buttonList[MAX_APP_LEN];

public:
	GLaunch::GLaunch();
	virtual GLaunch::~GLaunch();
	virtual void onEvent(Event *event);
};

/** ボタンラベル一覧 */
static char *buttonLabelList[MAX_APP_LEN] = {
	"GBBALL",
	"GBROWSER",
	"GCLOCK",
	"GHEBTN",
	"GNOIZ2BG",
	"GRUNNER",
	"GUITEST1",
	"GVERSION"
};

/** コマンド名一覧 */
static char *buttonCommandList[MAX_APP_LEN] = {
	"/APPS/GBBALL.EX5",
	"/APPS/GBROWSER.EX5",
	"/APPS/GCLOCK.EX5",
	"/APPS/GHEBTN.APP/GHEBTN.EX5",
	"/APPS/GNOIZ2BG.EX5",
	"/APPS/GRUNNER.EX5",
	"/APPS/GUITEST1.EX5",
	"/APPS/GVERSION.EX5"
};

GLaunch::GLaunch()
{
	setRect(0, 22, 100, 200);
	setTitle("mokon");
	for (int i = 0; i < MAX_APP_LEN; i++) {
		buttonList[i] = new Button(buttonLabelList[i]);
		buttonList[i]->setRect(0,i*20,88,20);
		add(buttonList[i]);
	}
}

GLaunch::~GLaunch()
{
}

void GLaunch::onEvent(Event *event)
{
	if (event->type == MOUSE_PRESSED) {
		for (int i = 0; i < MAX_APP_LEN; i++) {
			if (event->source == buttonList[i]) {
				// アプリ実行
				monapi_call_process_execute_file(buttonCommandList[i], MONAPI_FALSE);
				break;
			}
		}
	}
}

/** メイン */
#if defined(MONA)
int MonaMain(List<char*>* pekoe)
#else
int main(int argc, char **argv)
#endif
{
	GLaunch *launch = new GLaunch();
	launch->run();
	delete(launch);
	return 0;
}
