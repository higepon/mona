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

#include <baygui.h>
#ifdef MONA
#include <monapi/CString.h>
#endif
#include "glaunch.h"

/** デフォルトコンストラクタ */
Glaunch::Glaunch()
{
	setBounds(0, 22, 108 + getInsets()->left + getInsets()->right, 250 + getInsets()->top + getInsets()->bottom);
	setTitle("mokon");

	// 前回の選択位置
	prevIndex = -1;

	// アプリ一覧リスト
	list = new ListBox();
	list->setBounds(0, 0, 108, 250);
	add(list);

#if defined(MONA)
	// APPSに移動
	if (monapi_call_change_directory("/APPS/BAYGUI") == MONA_FAILURE) {
		return;
	}

	monapi_cmemoryinfo* mi = monapi_call_file_read_directory("/APPS/BAYGUI", MONAPI_TRUE);

	int dsize = *(int*)mi->Data;
	if (mi == NULL || dsize == 0) return;

	monapi_directoryinfo* p = (monapi_directoryinfo*)&mi->Data[sizeof(int)];

	// ディレクトリ内のファイル検索
	for (int i = 0; i < dsize; i++, p++) {
		bool isDirectory = (p->attr & ATTRIBUTE_DIRECTORY) != 0;
		MonAPI::CString file = p->name;
		
		if (isDirectory) {
			// . と .. は除外する
			if (file != "." && file != "..") {
				 list->add((char*)(const char*)file);
			}
		 } else {
			// BayGUIアプリのみをピックアップ (*.EX5)
			// GLAUNCHとGSHELLは対象からはずす
			if (file[file.getLength() - 1] == '5' &&
				file != "GLAUNCH.EX5" &&
				file != "GSHELL.EX5")
			{
				list->add((char*)(const char*)file);
			}
		 }
	}

	monapi_cmemoryinfo_dispose(mi);
	monapi_cmemoryinfo_delete(mi);
#else
	list->add("TEST1.EXE");
	list->add("TEST2.EXE");
	list->add("TEST3.EXE");
#endif
}

/** デストラクタ */
Glaunch::~Glaunch()
{
	delete(list);
}

/** アプリ実行 */
void Glaunch::execute()
{
#if defined(MONA)
	char name[48];
	char *item = list->getSelectedItem();
	if (prevIndex == -1 || item == NULL || strlen(item) == 0) return;
	memset(name, '\0', sizeof(name));
	strcpy(name, "/APPS/BAYGUI/");
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
	prevIndex = -1;
	monapi_call_process_execute_file(name, MONAPI_TRUE);
#endif
}

/** イベントハンドラ */
void Glaunch::processEvent(Event* event)
{
	if (event->getType() == Event::ITEM_SELECTED) {
		// 前の選択位置と同じ（ダブルクリック）
		if (prevIndex == list->getSelectedIndex()) {
			execute();
		} else {
			prevIndex = list->getSelectedIndex();
		}
	} else if (event->getType() == KeyEvent::KEY_PRESSED) {
		// ENTERキー押下
		if (((KeyEvent *)event)->getKeycode() == KeyEvent::VKEY_ENTER) {
			execute();
		}
	}
}
