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
				//info.str[127] = '\0';
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
	/** コマンド履歴ポインター */
	int historyPtr;
	/** コマンド履歴 */
	LinkedList *history;
	/** 行リスト */
	LinkedList *lines;
	/** １行バッファ */
	char lineBuffer[256];
	/** コマンドバッファ */
	char commandBuffer[256];
	/** カレントパス */
	char currentPath[256];

private:
	/** ディレクトリ表示 */
	inline void ls(char *str) {
		// ディレクトリを開く
		monapi_cmemoryinfo* mi = monapi_call_file_read_directory(str, MONAPI_TRUE);
		int size = *(int*)mi->Data;
		if (mi == NULL || size == 0) {
			this->addLine("ファイルまたはディレクトリが見つかりません。\n");
			return;
		}
		// ディレクトリを検索
		monapi_directoryinfo* p = (monapi_directoryinfo*)&mi->Data[sizeof(int)];
		char temp[128];
		memset(temp, 0, sizeof(temp));
		for (int i = 0; i < size; i++, p++) {
			// 1行4項目まで表示する
			if ((i > 0) && (i % 4 == 0)) {
				temp[strlen(temp)] = '\n';
				this->addLine(temp);
				memset(temp, 0, sizeof(temp));
			}
			// 大文字を小文字に変換する
			for (int i = 0; i < (int)strlen(p->name); i++) {
				if ('A' <= (p->name)[i] && (p->name)[i] <= 'Z') {
					(p->name)[i] = (p->name)[i] + ('a' - 'A');
				}
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
		// 残りの3項目以下のファイルを追加
		if (strlen(temp) > 0) {
			temp[strlen(temp)] = '\n';
			this->addLine(temp);
			memset(temp, 0, sizeof(temp));
		}
	}

	/** ディレクトリ移動 */
	inline void cd(char *str) {
		String s = str;
		// 一つ上のディレクトリへ
		if (s.equals("..") && strlen(currentPath) > 0 && strcmp(currentPath, "/") != 0) {
			int I = strlen(currentPath) - 1;
			for (int i = I; i > 0; i--) {
				if (currentPath[i] == '/') {
					currentPath[i] = '\0';
					return;
				} else {
					currentPath[i] = '\0';
				}
			}
		// 絶対パス
		} else if (s.startsWith("/")) {
			memset(currentPath, 0, sizeof(currentPath));
			strcpy(currentPath, str);
		// 相対パス
		} else {
			if (strcmp(currentPath, "/") != 0) {
				strcat(currentPath, "/");
			}
			strcat(currentPath, str);
		}
	}
	
	/** ファイルの内容を表示 */
	inline void cat(char *str) {
		// 絶対パスへ変換する
		char temp[256];
		memset(temp, 0, sizeof(temp));
		strcpy(temp, currentPath);
		strcat(temp, "/");
		strcat(temp, str);
		
		// 圧縮されたファイルも表示する
		monapi_cmemoryinfo* mi = NULL;
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
			} else if (strlen(lineBuffer) == (GSHELL_WIDTH / 8) - 1 || mi->Data[i] == '\n') {
				lineBuffer[strlen(lineBuffer)] = mi->Data[i];
				this->addLine(lineBuffer);
				memset(lineBuffer, 0, sizeof(lineBuffer));
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
		this->addLine(time);
	}
	
	/** プロセス情報を表示 */
	inline void ps() {
		syscall_set_ps_dump();
		PsInfo info;
		this->addLine("[tid] [状態]  [eip]    [esp]    [cr3]    [名前]\n");
		char buf[128];
		while (syscall_read_ps_dump(&info) == 0) {
			sprintf(buf, "%5d %s %08x %08x %08x %s\n",
			info.tid, info.state ? "実行中" : "待機中",
			info.eip, info.esp, info.cr3, info.name);
			this->addLine(buf);
		}
	}
	
	/** ファイルを実行する */
	inline void exec(char *filename) {
		String s = filename;
		// 絶対パス
		if (s.startsWith("/")) {
			monapi_call_process_execute_file(filename, MONAPI_FALSE);
		// APP形式
		} else if (s.endsWith(".app") || s.endsWith(".APP")) {
			char temp[256];
			memset(temp, 0, sizeof(temp));
			strcpy(temp, currentPath);
			strcat(temp, "/");
			strcat(temp, filename);
			// ディレクトリを開く
			monapi_cmemoryinfo* mi = monapi_call_file_read_directory(temp, MONAPI_TRUE);
			int size = *(int*)mi->Data;
			if (mi == NULL || size == 0) return;
			// ディレクトリを検索
			monapi_directoryinfo* p = (monapi_directoryinfo*)&mi->Data[sizeof(int)];
			for (int i = 0; i < size; i++, p++) {
				// 一般ファイル
				if ((p->attr & ATTRIBUTE_DIRECTORY) == 0 &&
					p->name[strlen(p->name) - 4] == '.' && 
					p->name[strlen(p->name) - 3] == 'E')
				{
					// *.E?? は実行形式だと認識する
					// *.ELF/*.EL2/*.EL5/*.EXE/*.EX2/*.EX5
					strcat(temp, "/");
					strcat(temp, p->name);
					break;
				}
			}
			monapi_call_process_execute_file(temp, MONAPI_FALSE);
		// 相対パス
		} else {
			char temp[256];
			memset(temp, 0, sizeof(temp));
			strcpy(temp, currentPath);
			strcat(temp, "/");
			strcat(temp, filename);
			monapi_call_process_execute_file(temp, MONAPI_FALSE);
		}
	}
	
	/** 1行追加 */
	inline void addLine(char *str) {
		// 最下行まで表示されているときは最上行を削除する
		// ここでremoveしなければ後々スクロールバーをつけたときには役に立つかも
		if (strlen(lineBuffer) > 0 && lines->getLength() >= (GSHELL_HEIGHT / 12 - 2)) {
			lines->remove(0);
		} else if (strlen(lineBuffer) == 0 && lines->getLength() >= (GSHELL_HEIGHT / 12 - 1)) {
			lines->remove(0);
		}
		lines->add(new String(str));
	}
	
	/** 指定したファイルがあるかどうか調べる */
	inline bool existsFile(char *filename) {
		// ディレクトリを開く
		monapi_cmemoryinfo* mi = monapi_call_file_read_directory(this->currentPath, MONAPI_TRUE);
		int size = *(int*)mi->Data;
		if (mi == NULL || size == 0) {
			this->addLine("ファイルまたはディレクトリが見つかりません。\n");
			return false;
		}
		// ディレクトリを検索
		monapi_directoryinfo* p = (monapi_directoryinfo*)&mi->Data[sizeof(int)];
		for (int i = 0; i < size; i++, p++) {
			// 大文字を小文字に変換する
			for (int i = 0; i < (int)strlen(p->name); i++) {
				if ('A' <= (p->name)[i] && (p->name)[i] <= 'Z') {
					(p->name)[i] = (p->name)[i] + ('a' - 'A');
				}
			}
			if (strcmp(filename, p->name) == 0) return true;
		}
		this->addLine("ファイルまたはディレクトリが見つかりません。\n");
		return false;
	}
	
	/** コマンド解析 */
	void parse(char *cmd) {
		// NULLチェック
		if (cmd == NULL || strlen(cmd) == 0) return;
		
		//
		// help
		//
		String s = cmd;
		if (s.equals("help")) {
			this->addLine("GUIシェル 内部コマンド一覧\n");
			this->addLine(" help, ls/dir, cd, cat/type, date, uname/ver, \n");
			this->addLine(" clear/cls, ps, kill, touch, exec\n");
		//
		// ls/dir
		//
		} else if (s.equals("ls") || s.equals("dir")) {
			this->ls(currentPath);
		//
		// cd [pathname]
		//
		} else if (s.startsWith("cd ")) {
			char pathname[128];
			sscanf(cmd, "cd %s", pathname);
			// NULLチェック
			if (pathname != NULL && strlen(pathname) > 0 && existsFile(pathname) == true) {
				this->cd(pathname);
			}
		//
		// cat [filename]
		//
		} else if (s.startsWith("cat ")) {
			char filename[128];
			sscanf(cmd, "cat %s", filename);
			// NULLチェック
			if (filename != NULL && strlen(filename) > 0 && existsFile(filename) == true) {
				this->cat(filename);
			}
		//
		// type [filename]
		//
		} else if (s.startsWith("type ")) {
			char filename[128];
			sscanf(cmd, "type %s", filename);
			// NULLチェック
			if (filename != NULL && strlen(filename) > 0 && existsFile(filename) == true) {
				this->cat(filename);
			}
		//
		// date
		//
		} else if (s.equals("date")) {
			this->date();
		//
		// uname/ver
		//
		} else if (s.equals("uname") || s.equals("ver")) {
			char uname[128];
			syscall_get_kernel_version(uname, 128);
			uname[strlen(uname)] = '\n';
			this->addLine(uname);
		//
		// clear/cls
		//
		} else if (s.equals("clear") || s.equals("cls")) {
			int I = lines->getLength();
			for (int i = 0; i < I; i++) {
				lines->remove(0);
			}
		//
		// ps
		//
		} else if (s.equals("ps")) {
			this->ps();
		//
		// kill [pid]
		//
		} else if (s.startsWith("kill ")) {
			int pid;
			sscanf(cmd, "kill %d", &pid);
			syscall_kill_thread((dword)pid);
		//
		// exec [filename]
		//
		} else if (s.startsWith("exec ")) {
			char filename[128];
			memset(filename, 0, sizeof(filename));
			// NULLチェック
			if (strlen(cmd) > 5) {
				// 先頭の"exec "を削る
				for (int i = 5; i < (int)strlen(cmd); i++) {
					filename[i - 5] = cmd[i];
				}
				if (filename[0] == '/' || existsFile(filename) == true) {
					this->exec(filename);
				}
			}
		//
		// touch [filename]
		//
		} else if (s.startsWith("touch ")) {
			char filename[128];
			sscanf(cmd, "touch %s", filename);
			// NULLチェック
			if (filename != NULL && strlen(filename) > 0) {
				syscall_file_create(filename);
			}
		} else {
			char temp[256];
			sprintf(temp, "'%s'ｺﾏﾝﾄﾞｴﾗｰｷﾀｰｰｰ(ﾟ∀ﾟ)ｰｰｰ!!!\n", cmd);
			this->addLine(temp);
		}
	}

public:
	/** コンストラクタ */
	GShell() {
		setRect((800-GSHELL_WIDTH-12)/2,(600-GSHELL_HEIGHT-28)/2,GSHELL_WIDTH+12,GSHELL_HEIGHT+28);
		setTitle("GUIシェル");
		historyPtr = 0;
		this->history = new LinkedList();
		this->lines = new LinkedList();
		memset(lineBuffer, 0, sizeof(lineBuffer));
		memset(commandBuffer, 0, sizeof(commandBuffer));
		memset(currentPath, 0, sizeof(currentPath));
		strcpy(currentPath, "/apps");
	}
	
	/** デストラクタ */
	~GShell() {
		delete(this->history);
		delete(this->lines);
	}
	
	/** イベントハンドラ */
	virtual void onEvent(Event *e) {
		// printfをハンドリング
		if (e->type == CUSTOM_EVENT) {
			#if 0
			char *temp = e->str;
			if (strlen(lineBuffer) == 0) {
				strcpy(lineBuffer, temp);
			} else {
				strcat(lineBuffer, temp);
			}
			if (temp[strlen(temp) - 1] == '\n') {
				// リストに追加
				this->addLine(lineBuffer);
				memset(lineBuffer, 0, 256);
				// 再描画
				onPaint(getGraphics());
				update();
			}
			#endif
			#if 1
			for (int i = 0; i < (int)strlen(e->str); i++) {
				if (strlen(lineBuffer) == (GSHELL_WIDTH / 8) - 1 || e->str[i] == '\n') {
					// リストに追加
					lineBuffer[strlen(lineBuffer)] = e->str[i];
					this->addLine(lineBuffer);
					memset(lineBuffer, 0, sizeof(lineBuffer));
					// 再描画
					onPaint(getGraphics());
					update();
				} else {
					lineBuffer[strlen(lineBuffer)] = e->str[i];
				}
			}
			#endif
		// キーイベント
		} else if (e->type == KEY_PRESSED) {
			KeyEvent *ke = (KeyEvent *)e;
			// コマンド実行
			if (ke->keycode == VKEY_ENTER) {
				if (strlen(commandBuffer) == 0) return;
				// /% ....
				char temp[256];
				sprintf(temp, "%s%% %s", currentPath, commandBuffer);
				// リストに追加
				this->addLine(temp);
				this->history->add(new String(temp));
				// 入力したコマンドを解析
				parse(commandBuffer);
				memset(lineBuffer, 0, 256);
				memset(commandBuffer, 0, 256);
				// 再描画
				onPaint(getGraphics());
				update();
			// １文字削除
			} else if (ke->keycode == VKEY_BACKSPACE) {
				if (strlen(commandBuffer) == 0) return;
				memset(lineBuffer, 0, 256);
				commandBuffer[strlen(commandBuffer) - 1] = 0;
				// 再描画
				onPaint(getGraphics());
				update();
			// １つ前の履歴
			} else if (ke->keycode == VKEY_UP) {
				if (historyPtr == 0) return;
				historyPtr--;
				strcpy(commandBuffer, ((String *)history->get(historyPtr))->getBytes());
				// 再描画
				onPaint(getGraphics());
				update();
			// １つ次の履歴
			} else if (ke->keycode == VKEY_DOWN) {
				if (historyPtr < history->getLength() - 1) {
					historyPtr++;
					strcpy(commandBuffer, ((String *)history->get(historyPtr))->getBytes());
				} else {
					memset(commandBuffer, 0, sizeof(commandBuffer));
				}
				// 再描画
				onPaint(getGraphics());
				update();
			// 1文字追加
			} else if (0 < ke->keycode && ke->keycode < 128) {
				memset(lineBuffer, 0, sizeof(lineBuffer));
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
		//g->setColor(COLOR_BLACK);
		g->fillRect(0, 0, this->width, this->height);
		g->setColor(COLOR_BLACK);
		//g->setColor(COLOR_WHITE);
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
		char temp[256];
		sprintf(temp, "%s%% %s", currentPath, commandBuffer);
		g->drawText(temp, 0, i * 12);
		// キャレット
		int x0 = strlen(temp) * 8;
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
