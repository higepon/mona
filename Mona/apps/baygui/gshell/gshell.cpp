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

#define GSHELL_WIDTH  480 /* 8dot X 60chars */
#define GSHELL_HEIGHT 300 /* 12dot X 25 chars */

static dword my_tid, stdout_tid;

/** 標準出力監視スレッド */
static void StdoutMessageLoop() {
	MonAPI::Message::send(my_tid, MSG_SERVER_START_OK);

	while(1) {
		MessageInfo info;
		dword id = THREAD_UNKNOWN;
		if (!MonAPI::Message::receive(&info)) {
			switch (info.header) {
			case MSG_PROCESS_STDOUT_DATA:
		        case MSG_STDOUT:
				info.str[127] = '\0';
				id = MonAPI::Message::lookupMainThread("GSHELL.EX5");
				MonAPI::Message::send(id, CUSTOM_EVENT, 0, 0, 0, info.str);
				MonAPI::Message::reply(&info);
				break;
			}
		}
	}
}

/** GUIコンソールクラス */
class GShell : public Window {
private:
	LinkedList *lines;
	char lineBuffer[256];
	char commandBuffer[256];

public:
	GShell() {
		setRect((800-GSHELL_WIDTH-12)/2,(600-GSHELL_HEIGHT-28)/2,GSHELL_WIDTH+12,GSHELL_HEIGHT+28);
		setTitle("GUIシェル");
		lines = new LinkedList();
		memset(lineBuffer, 0, 256);
		memset(commandBuffer, 0, 256);
	}
	
	/** 1行追加 */
	void add(char *str) {
		// 最下行まで表示されているときは最上行を削除する
		// ここでremoveしなければ後々スクロールバーをつけたときには役に立つかも
		if (strlen(lineBuffer) > 0 && lines->getLength() >= (GSHELL_HEIGHT / 12 - 2)) {
			lines->remove(0);
		} else if (strlen(lineBuffer) == 0 && lines->getLength() >= (GSHELL_HEIGHT / 12 - 1)) {
			lines->remove(0);
		}
		lines->add(new String(str));
	}
	
	/** ディレクトリ表示 */
	inline void ls(char *str) {
		// ディレクトリを開く
		monapi_cmemoryinfo* mi = monapi_call_file_read_directory(str, MONAPI_TRUE);
		int size = *(int*)mi->Data;
		if (mi == NULL || size == 0) {
			add("ディレクトリが見つかりません。\n");
			return;
		}
		// ディレクトリを検索
		monapi_directoryinfo* p = (monapi_directoryinfo*)&mi->Data[sizeof(int)];
		char temp[128];
		memset(temp, 0, 128);
		for (int i = 0; i < size; i++, p++) {
			// 1行4項目まで表示する
			if ((i > 0) && (i % 4 == 0)) {
				temp[strlen(temp)] = '\n';
				add(temp);
				memset(temp, 0, 128);
			}
			// ディレクトリ
			if ((p->attr & ATTRIBUTE_DIRECTORY) != 0) {
				if (strlen(temp) == 0) {
					strcpy(temp, "[");
				} else {
					strcat(temp, "[");
				}
				strcat(temp, p->name);
				strcat(temp, "]");
				for (int j = 0; j < (13 - (int)strlen(p->name)); j++) {
					strcat(temp, " ");
				}
			// 一般ファイル
			} else {
				if (strlen(temp) == 0) {
					strcpy(temp, p->name);
				} else {
					strcat(temp, p->name);
				}
				for (int j = 0; j < (15 - (int)strlen(p->name)); j++) {
					strcat(temp, " ");
				}
			}
		}
	}

	/** ディレクトリ移動 */
	inline void cd(char *str) {
	}
	
	/** ファイルの内容を表示 */
	inline void cat(char *str) {
		monapi_cmemoryinfo* mi = NULL;
		// 圧縮されたファイルも表示する
		if (str[strlen(str) - 1] == '2') {
			mi = monapi_call_file_decompress_bz2_file(str, MONAPI_FALSE);
		} else if (str[strlen(str) - 1] == '5') {
			mi = monapi_call_file_decompress_st5_file(str, MONAPI_FALSE);
		} else {
			mi = monapi_call_file_read_data(str, MONAPI_FALSE);
		}
		
		if (mi == NULL) return;
		if (mi->Size < 0) return;
		
		for (dword i = 0; i < mi->Size; i++) {
			if (mi->Data[i] == '\r') {
				// NOP
			} else if (mi->Data[i] == '\n') {
				add(lineBuffer);
				memset(lineBuffer, 0, 256);
			} else {
				lineBuffer[strlen(lineBuffer)] = mi->Data[i];
			}
		}
		
		monapi_cmemoryinfo_dispose(mi);
		monapi_cmemoryinfo_delete(mi);
	}
	
	/** 日付を表示 */
	inline void date() {
		MonAPI::Date date;
		const char* day [] = { "日", "月", "火", "水", "木", "金", "土" };
		const char* ampm[] = { "午前", "午後" };
		char time[128];
		date.refresh();
		sprintf(time, "%d年%02d月%02d日(%s) %s %02d:%02d:%02d\n",
			date.year(), date.month(), date.day(), day[date.dayofweek() % 7],
			ampm[date.hour() / 12], date.hour() % 12, date.min(), date.sec());
		add(time);
	}
	
	/** プロセス情報を表示 */
	inline void ps() {
		syscall_set_ps_dump();
		PsInfo info;
		add("[tid] [状態]  [eip]    [esp]    [cr3]    [名前]\n");
		char buf[128];
		while (syscall_read_ps_dump(&info) == 0) {
			sprintf(buf, "%5d %s %08x %08x %08x %s\n",
			info.tid, info.state ? "実行中" : "待機中",
			info.eip, info.esp, info.cr3, info.name);
			add(buf);
		}
	}
	
	/** コマンド解析 */
	void parseCommand(char *cmd) {
		String s = cmd;
		if (s.equals("help")) {
			add("GUIシェル 内部コマンド一覧\n");
			add(" help, ls, cd, cat, date, uname, clear, ps, kill, exec\n");
		} else if (s.equals("ls") || s.equals("dir")) {
			ls("/");
		} else if (s.startWith("cd ")) {
			char pathname[128];
			sscanf(cmd, "cd %s", pathname);
			cd(pathname);
		} else if (s.startWith("cat ")) {
			char filename[128];
			sscanf(cmd, "cat %s", filename);
			cat(filename);
		} else if (s.startWith("type ")) {
			char filename[128];
			sscanf(cmd, "type %s", filename);
			cat(filename);
		} else if (s.equals("date")) {
			date();
		} else if (s.equals("uname") || s.equals("ver")) {
			char uname[128];
			syscall_get_kernel_version(uname, 128);
			uname[strlen(uname)] = '\n';
			add(uname);
		} else if (s.equals("clear") || s.equals("cls")) {
			int I = lines->getLength();
			for (int i = 0; i < I; i++) {
				lines->remove(0);
			}
		} else if (s.equals("ps")) {
			ps();
		} else if (s.startWith("kill ")) {
			int pid;
			sscanf(cmd, "kill %d", &pid);
			syscall_kill_thread((dword)pid);
		} else if (s.startWith("exec ")) {
			char filename[128];
			memset(filename, 0, 128);
			// 先頭の"exec "を削る
			for (int i = 5; i < (int)strlen(cmd); i++) {
				filename[i - 5] = cmd[i];
			}
			monapi_call_process_execute_file(filename, MONAPI_FALSE);
		} else {
			char temp[256];
			sprintf(temp, "'%s'ｺﾏﾝﾄﾞｴﾗｰｷﾀｰｰｰ(ﾟ∀ﾟ)ｰｰｰ!!!\n", cmd);
			add(temp);
		}
	}
	
	/** イベントハンドラ */
	virtual void onEvent(Event *e) {
		// printfをハンドリング
		if (e->type == CUSTOM_EVENT) {
			char *temp = e->str;
			if (strlen(lineBuffer) == 0) {
				strcpy(lineBuffer, temp);
			} else {
				strcat(lineBuffer, temp);
			}
			if (temp[strlen(temp) - 1] == '\n') {
				// リストに追加
				add(temp);
				memset(lineBuffer, 0, 256);
				onPaint(getGraphics());
				update();
			}
		// キーイベント
		} else if (e->type == KEY_PRESSED) {
			KeyEvent *ke = (KeyEvent *)e;
			if (ke->keycode == VKEY_ENTER) {
				if (strlen(commandBuffer) > 0) {
					// MONA>....
					char temp[5 + 1+ 256];
					sprintf(temp, "MONA>%s", commandBuffer);
					// リストに追加
					add(temp);
					// 入力したコマンドを解析
					parseCommand(commandBuffer);
					memset(lineBuffer, 0, 256);
					memset(commandBuffer, 0, 256);
					// 再描画
					onPaint(getGraphics());
					update();
				}
			} else if (ke->keycode == VKEY_BACKSPACE) {
				if (strlen(commandBuffer) > 0) {
					// 1文字削除
					memset(lineBuffer, 0, 256);
					commandBuffer[strlen(commandBuffer) - 1] = 0;
					// 再描画
					onPaint(getGraphics());
					update();
				}
			} else {
				// 1文字追加
				memset(lineBuffer, 0, 256);
				commandBuffer[strlen(commandBuffer)] = ke->keycode;
				// 再描画
				onPaint(getGraphics());
				update();
			}
		}
	}

	/** 描画ハンドラ */
	virtual void onPaint(Graphics *g) {
		// 背景色で塗りつぶし
		g->setColor(COLOR_WHITE);
		g->fillRect(0, 0, this->width, this->height);
		g->setColor(COLOR_BLACK);
		g->setFontStyle(FONT_FIXED);
		int i = 0;
		// 確定ずみのprintfバッファー
		for (i = 0; i < lines->getLength(); i++) {
			String *temp = (String *)lines->get(i);
			g->drawText(temp->getBytes(), 0, i * 12);
		}
		// 作業中のprintfバッファー
		if (strlen(lineBuffer) > 0) {
			g->drawText(lineBuffer, 0, i * 12);
			i++;
		}
		// コマンドライン
		g->drawText("MONA>", 0, i * 12);
		g->drawText(commandBuffer, 8 * 5, i * 12);
		// キャレット
		int x0 = 8 * 5 + 8 * strlen(commandBuffer);
		int y0 = i * 12 + 10;
		g->drawLine(x0, y0, x0 + 8, y0);
		g->drawLine(x0, y0 + 1, x0 + 8, y0 + 1);
	}
	
	/** ウィンドウ生成時に呼ばれる */
	virtual void create() {
		Window::create();
		
		// 標準出力監視スレッド起動
		my_tid = syscall_get_tid();
		syscall_mthread_join(syscall_mthread_create((dword)StdoutMessageLoop));
		MessageInfo msg, src;
		src.header = MSG_SERVER_START_OK;
		MonAPI::Message::receive(&msg, &src, MonAPI::Message::equalsHeader);
		stdout_tid = msg.from;

		// 標準出力を得る
		dword tid = monapi_get_server_thread_id(ID_PROCESS_SERVER);
		if (tid != THREAD_UNKNOWN) {
			MonAPI::Message::sendReceive(NULL, tid + 1, MSG_PROCESS_GRAB_STDOUT, stdout_tid);
		}
	}
	
	/** ウィンドウ破棄時に呼ばれる */
	virtual void dispose() {
		Window::dispose();
		
		// 標準出力を開放する
		dword tid = monapi_get_server_thread_id(ID_PROCESS_SERVER);
		if (tid != THREAD_UNKNOWN) {
			MonAPI::Message::sendReceive(NULL, tid + 1, MSG_PROCESS_UNGRAB_STDOUT, stdout_tid);
		}
		syscall_kill_thread(stdout_tid);
	}
};

/** メイン */
int MonaMain(List<char*>* pekoe) {
	// アプリケーションを初期化する
	GShell *shell = new GShell();
	shell->run();
	delete(shell);
	return 0;
}
