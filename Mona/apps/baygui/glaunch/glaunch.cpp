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

/**
 アプリケーションランチャー
*/
class GLaunch : public Window {
private:
	int prevIndex;
	ListBox *list;
	virtual void execute();

public:
	GLaunch::GLaunch();
	virtual GLaunch::~GLaunch();
	virtual void onEvent(Event *event);
};

GLaunch::GLaunch()
{
	char name[15];
	int size, attr;

	setRect(0, 22, 108 + 12, 250 + 28);
	setTitle("mokon");
	
	// 前回の選択位置
	prevIndex = -1;
	
	// アプリ一覧リスト
	list = new ListBox();
	list->setRect(0, 0, 108, 250);
	add(list);

	// APPSに移動
	if (syscall_cd("/APPS") != 0) {
		return;
	}

	// ディレクトリを開く
	if (syscall_dir_open()) {
		return;
	}

	// ディレクトリ内のファイル検索
	while (syscall_dir_read(name, &size, &attr) == 0) {
		if ((attr & ATTRIBUTE_DIRECTORY) != 0) {
			// BayGUIアプリのみをピックアップ (*.APP)
			// GREVERSIは対象からはずす
			if (name[0] == 'G' && name[1] != 'R') {
				list->add(name);
			}
		} else {
			// BayGUIアプリのみをピックアップ (*.EX5)
			// GLAUNCHは対象からはずす
			if (name[strlen(name) - 1] == '5' && name[1] != 'L') {
				list->add(name);
			}
		}
	}

    syscall_dir_close();
}

GLaunch::~GLaunch()
{
	delete(list);
}

/** アプリ実行 */
void GLaunch::execute()
{
	char name[24];
	char *item = list->getSelectedItem();
	if (item != NULL && strlen(item) > 0) {
		strcpy(name, "/APPS/");
		strcat(name, item);
		// *.APP の場合
		if (item[strlen(item) - 1] == 'P') {
			strcat(name, "/");
			strcat(name, item);
			// 拡張子をAPPからEX5に変換
			name[strlen(name) - 3] = 'E';
			name[strlen(name) - 2] = 'X';
			name[strlen(name) - 1] = '5';
		}
		// アプリ実行
		monapi_call_process_execute_file(name, MONAPI_FALSE);
	}
}

/** イベント処理 */
void GLaunch::onEvent(Event *event)
{
	if (event->type == ITEM_SELECTED) {
		// 前の選択位置と同じ（ダブルクリック）
		if (prevIndex == list->getSelectedIndex()) {
			prevIndex = -1;
			execute();
		} else {
			prevIndex = list->getSelectedIndex();
		}
	} else if (event->type == KEY_PRESSED) {
		// ENTERキー押下
		if (((KeyEvent *)event)->keycode == VKEY_ENTER) {
			execute();
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
