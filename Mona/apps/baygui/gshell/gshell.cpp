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
				MonAPI::Message::sendReceive(NULL, id, MSG_PROCESS_STDOUT_DATA, 0, 0, 0, info.str);
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
	/** コマンドライン引数 */
	LinkedList *argv;
	/** コマンド履歴 */
	LinkedList *history;
	/** 行リスト */
	LinkedList *lines;
	/** １行バッファ */
	char lineBuffer[128];
	/** コマンドバッファ */
	char commandBuffer[128];
	/** カレントパス */
	char currentPath[128];

public:
	/** コンストラクタ */
	GShell() {
		setRect((800-GSHELL_WIDTH-12)/2,(600-GSHELL_HEIGHT-28)/2,GSHELL_WIDTH+12,GSHELL_HEIGHT+28);
		setTitle("た～みなる");
		historyPtr = -1;
		this->argv = new LinkedList();
		this->history = new LinkedList();
		this->lines = new LinkedList();
		memset(lineBuffer, 0, sizeof(lineBuffer));
		memset(commandBuffer, 0, sizeof(commandBuffer));
		memset(currentPath, 0, sizeof(currentPath));
		strcpy(currentPath, "/apps/baygui");
	}
	
	/** デストラクタ */
	~GShell() {
		this->argv->removeAll();
		this->history->removeAll();
		this->lines->removeAll();
		delete(this->argv);
		delete(this->history);
		delete(this->lines);
	}

private:
	/** ディレクトリ表示 */
	inline void ls(char *pathname) {
		// ディレクトリを開く
		monapi_cmemoryinfo* mi = monapi_call_file_read_directory(pathname, MONAPI_TRUE);
		if (mi == NULL) {
			this->addLine("ファイルまたはディレクトリが見つかりません。\n");
			return;
		}
		int size = *(int*)mi->Data;
		if (size == 0) {
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
			// 最後に'/'があるところまで文字を削る
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
	inline void cat(char *pathname) {
		// 圧縮されたファイルも表示する
		monapi_cmemoryinfo* mi = NULL;
		if (pathname[strlen(pathname) - 1] == '2') {
			mi = monapi_call_file_decompress_bz2_file(pathname, MONAPI_FALSE);
		} else if (pathname[strlen(pathname) - 1] == '5') {
			mi = monapi_call_file_decompress_st5_file(pathname, MONAPI_FALSE);
		} else {
			mi = monapi_call_file_read_data(pathname, MONAPI_FALSE);
		}
		
		// NULLチェック
		if (mi == NULL) return;
		if (mi->Size < 0) return;
		
		for (dword i = 0; i < mi->Size; i++) {
			// '\r' は無視する
			if (mi->Data[i] == '\r') {
				// NOP
			// 1行の最大文字数に達するか改行に達した時
			} else if (strlen(lineBuffer) == (GSHELL_WIDTH / 8) - 1 || mi->Data[i] == '\n') {
				lineBuffer[strlen(lineBuffer)] = mi->Data[i];
				this->addLine(lineBuffer);
				memset(lineBuffer, 0, sizeof(lineBuffer));
			// 1行バッファーに1バイト追加する
			} else {
				lineBuffer[strlen(lineBuffer)] = mi->Data[i];
			}
		}
		
		// ファイルを閉じる
		monapi_cmemoryinfo_dispose(mi);
		monapi_cmemoryinfo_delete(mi);
	}
	
	/** ファイルを実行する */
	inline void exec(char *pathname) {
		String s = pathname;
		// 通常形式
		if (s.endsWith("elf") || s.endsWith("el2") || s.endsWith("el5") ||
			s.endsWith("exe") || s.endsWith("ex2") || s.endsWith("ex5"))
		{
			// 引数を後ろにくっつける
			char temp[128];
			memset(temp, 0, sizeof(temp));
			if (this->argv->getLength() == 1) {
				strcpy(temp, pathname);
			} else if (this->argv->getLength() == 2) {
				sprintf(temp, "%s %s", pathname, 
					((String *)this->argv->get(1))->getBytes()
				);
			} else if (this->argv->getLength() == 3) {
				sprintf(temp, "%s %s %s", pathname, 
					((String *)this->argv->get(1))->getBytes(), 
					((String *)this->argv->get(2))->getBytes()
				);
			} else if (this->argv->getLength() == 4) {
				sprintf(temp, "%s %s %s %s", pathname, 
					((String *)this->argv->get(1))->getBytes(), 
					((String *)this->argv->get(2))->getBytes(), 
					((String *)this->argv->get(3))->getBytes()
				);
			}
			monapi_call_process_execute_file(temp, MONAPI_FALSE);
		// APP形式
		} else if (s.endsWith(".app") || s.endsWith(".APP")) {
			char temp[128];
			memset(temp, 0, sizeof(temp));
			strcpy(temp, pathname);
			strcat(temp, "/");
			
			// ディレクトリを開く
			monapi_cmemoryinfo* mi = monapi_call_file_read_directory(temp, MONAPI_TRUE);
			if (mi == NULL) return;
			int size = *(int*)mi->Data;
			if (size == 0) return;
			
			// ディレクトリを検索
			monapi_directoryinfo* p = (monapi_directoryinfo*)&mi->Data[sizeof(int)];
			for (int i = 0; i < size; i++, p++) {
				// *.E?? は実行形式だと認識する
				// *.ELF/*.EL2/*.EL5/*.EXE/*.EX2/*.EX5
				if ((p->attr & ATTRIBUTE_DIRECTORY) == 0 &&
					p->name[strlen(p->name) - 4] == '.' && 
					p->name[strlen(p->name) - 3] == 'E')
				{
					strcat(temp, p->name);
					break;
				}
			}
			monapi_call_process_execute_file(temp, MONAPI_FALSE);
		}
	}
	
	/** 1行追加 */
	inline void addLine(char *str) {
		// 最下行まで表示されているときは最上行を削除する
		// ここでremoveしなければ後々スクロールバーをつけたときには役に立つかも
		if (lines->getLength() >= (GSHELL_HEIGHT / 12 - 1)) {
			lines->remove(0);
		}
		lines->add(new String(str));
	}
	
	/** 指定したファイルがあるかどうか調べる */
	inline bool existsFile(char *filename) {
		// ディレクトリを開く
		monapi_cmemoryinfo* mi = monapi_call_file_read_directory(this->currentPath, MONAPI_TRUE);
		if (mi == NULL) {
			this->addLine("ファイルまたはディレクトリが見つかりません。\n");
			return false;
		}
		int size = *(int*)mi->Data;
		if (size == 0) {
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
		if (cmd == NULL || strlen(cmd) == 0) return;
		
		this->argv->removeAll();
		
		char temp[256];
		memset(temp, 0, sizeof(temp));
		
		// 文字列を' 'で区切る
		for (int i = 0; i < (int)strlen(cmd); i++) {
			if (cmd[i] == ' ') {
				this->argv->add(new String(temp));
				memset(temp, 0, sizeof(temp));
			} else {
				temp[strlen(temp)] = cmd[i];
				// ? のみのときなど
				if (i == (int)strlen(cmd) - 1) {
					this->argv->add(new String(temp));
				}
			}
		}
		
		//
		// help/?
		//
		String s = ((String *)this->argv->get(0))->getBytes();
		if (s.equals("help") || s.equals("?")) {
			this->addLine("GUIシェル 内部コマンド一覧\n");
			this->addLine(" help/?, ls/dir, cd, cat/type, date/time, uname/ver, \n");
			this->addLine(" clear/cls, ps, kill, touch, exit/quit\n");
		//
		// ls/dir
		//
		} else if (s.equals("ls") || s.equals("dir")) {
			this->ls(currentPath);
		//
		// cd [pathname]
		//
		} else if (s.equals("cd")) {
			if (this->argv->getLength() >= 2) {
				char *pathname = ((String *)this->argv->get(1))->getBytes();
				if (pathname[0] == '/' || existsFile(pathname) == true) {
					this->cd(pathname);
				}
			} else {
				this->addLine("使い方：cd [パス名]\n");
			}
		//
		// cat [pathname] / type [pathname]
		//
		} else if (s.equals("cat") || s.equals("type")) {
			if (this->argv->getLength() >= 2) {
				char *pathname = ((String *)this->argv->get(1))->getBytes();
				if (pathname[0] == '/' || existsFile(pathname) == true) {
					memset(temp, 0, sizeof(temp));
					// 相対パスから絶対パスへ変換する
					if (pathname[0] != '/') {
						strcpy(temp, currentPath);
						// 現在ルートにいるときは'/'を付与しない
						if (strcmp(currentPath, "/") != 0) {
							strcat(temp, "/");
						}
						strcat(temp, pathname);
					// 絶対パスのままにする
					} else {
						strcpy(temp, pathname);
					}
					this->cat(temp);
				}
			} else {
				if (s.equals("cat")) {
					this->addLine("使い方：cat [ファイル名]\n");
				} else if(s.equals("type")) {
					this->addLine("使い方：type [ファイル名]\n");
				}
			}
		//
		// date
		//
		} else if (s.equals("date") || s.equals("time")) {
			const char* day [] = { "日", "月", "火", "水", "木", "金", "土" };
			const char* ampm[] = { "午前", "午後" };
			
			MonAPI::Date date;
			date.refresh();
			
			memset(temp, 0, sizeof(temp));
			sprintf(temp, "%d年%02d月%02d日(%s) %s %02d:%02d:%02d\n",
				date.year(), date.month(), date.day(), day[date.dayofweek() % 7],
				ampm[date.hour() / 12], date.hour() % 12, date.min(), date.sec());
			this->addLine(temp);
		//
		// uname/ver
		//
		} else if (s.equals("uname") || s.equals("ver")) {
			char uname[128];
			syscall_get_kernel_version(uname, 128);
			this->addLine(uname);
		//
		// clear/cls
		//
		} else if (s.equals("clear") || s.equals("cls")) {
			lines->removeAll();
		//
		// ps
		//
		} else if (s.equals("ps")) {
			this->addLine("[tid] [状態]  [eip]    [esp]    [cr3]    [名前]\n");
			memset(temp, 0, sizeof(temp));
			
			PsInfo info;
			syscall_set_ps_dump();
			while (syscall_read_ps_dump(&info) == 0) {
				sprintf(temp, "%5d %s %08x %08x %08x %s\n",
					info.tid, info.state ? "実行中" : "待機中",
					info.eip, info.esp, info.cr3, info.name
				);
				this->addLine(temp);
			}
		//
		// kill [pid]
		//
		} else if (s.equals("kill")) {
			if (this->argv->getLength() >= 2) {
				int pid = atoi(((String *)this->argv->get(1))->getBytes());
				syscall_kill_thread((dword)pid);
			} else {
				this->addLine("使い方：kill [プロセスID]\n");
			}
		//
		// touch [pathname]
		//
		} else if (s.equals("touch")) {
			if (this->argv->getLength() >= 2) {
				char *pathname = ((String *)this->argv->get(1))->getBytes();
				memset(temp, 0, sizeof(temp));
				// 相対パスから絶対パスへ変換する
				if (pathname[0] != '/') {
					strcpy(temp, currentPath);
					// 現在ルートにいるときは'/'を付与しない
					if (strcmp(currentPath, "/") != 0) {
						strcat(temp, "/");
					}
					strcat(temp, pathname);
				// 絶対パスのままにする
				} else {
					strcpy(temp, pathname);
				}
				syscall_file_create(temp);
			} else {
				this->addLine("使い方：touch [ファイル名]\n");
			}
		//
		// exit / quit
		//
		} else if (s.equals("exit") || s.equals("quit")) {
			onExit();
			return;
		//
		// exec [pathname]
		//
		} else if (s.endsWith(".elf") || s.endsWith(".el2") || s.endsWith(".el5") ||
			s.endsWith(".exe") || s.endsWith(".ex2") || s.endsWith(".ex5") ||
			s.endsWith(".app"))
		{
			char *pathname = s.getBytes();
			memset(temp, 0, sizeof(temp));
			// 相対パスから絶対パスへ変換する
			if (pathname[0] != '/') {
				strcpy(temp, currentPath);
				// 現在ルートにいるときは'/'を付与しない
				if (strcmp(currentPath, "/") != 0) {
					strcat(temp, "/");
				}
				strcat(temp, pathname);
			// 絶対パスのままにする
			} else {
				strcpy(temp, pathname);
			}
			this->exec(temp);
		} else {
			memset(temp, 0, sizeof(temp));
			sprintf(temp, "'%s'ｺﾏﾝﾄﾞｴﾗｰｷﾀｰｰｰ(ﾟ∀ﾟ)ｰｰｰ!!!\n", cmd);
			this->addLine(temp);
		}
		
	}

public:
	/** イベントハンドラ */
	virtual void onEvent(Event *e) {
		// printfをハンドリング
		if (e->getType() == Event::CUSTOM_EVENT) {
			for (int i = 0; i < (int)strlen(e->str); i++) {
				if (strlen(lineBuffer) == (GSHELL_WIDTH / 8) - 1 || e->str[i] == '\n') {
					// リストに追加
					lineBuffer[strlen(lineBuffer)] = e->str[i];
					this->addLine(lineBuffer);
					memset(lineBuffer, 0, sizeof(lineBuffer));
					// 再描画
					onPaint(getGraphics());
				} else if (e->str[i] != '\r') {
					lineBuffer[strlen(lineBuffer)] = e->str[i];
				}
			}
		// キーイベント
		} else if (e->getType() == KeyEvent::KEY_PRESSED) {
			KeyEvent *ke = (KeyEvent *)e;
			int keycode = ke->getKeycode();
			// コマンド実行
			if (keycode == KeyEvent::VKEY_ENTER) {
				char temp[256];
				if (strlen(commandBuffer) == 0) {
					sprintf(temp, "%s%% \n", currentPath);
					// リストに追加
					this->addLine(temp);
				} else {
					sprintf(temp, "%s%% %s", currentPath, commandBuffer);
					// リストに追加
					this->addLine(temp);
					this->history->add(new String(commandBuffer));
					this->historyPtr = this->history->getLength();
					// 入力したコマンドを解析
					parse(commandBuffer);
					memset(lineBuffer, 0, sizeof(lineBuffer));
					memset(commandBuffer, 0, sizeof(commandBuffer));
				}
				// 再描画
				onPaint(getGraphics());
			// １文字削除
			} else if (keycode == KeyEvent::VKEY_BACKSPACE) {
				if (strlen(commandBuffer) == 0) return;
				memset(lineBuffer, 0, sizeof(lineBuffer));
				commandBuffer[strlen(commandBuffer) - 1] = 0;
				// 再描画
				onPaint(getGraphics());
			// １つ前の履歴
			} else if (keycode == KeyEvent::VKEY_UP) {
				if (this->historyPtr == -1) return;
				this->historyPtr--;
				strcpy(commandBuffer, ((String *)history->get(this->historyPtr))->getBytes());
				// 再描画
				onPaint(getGraphics());
			// １つ次の履歴
			} else if (keycode == KeyEvent::VKEY_DOWN) {
				if (this->historyPtr < this->history->getLength() - 1) {
					this->historyPtr++;
					strcpy(commandBuffer, ((String *)history->get(this->historyPtr))->getBytes());
				} else {
					memset(commandBuffer, 0, sizeof(commandBuffer));
				}
				// 再描画
				onPaint(getGraphics());
			// 1文字追加
			} else if (0 < keycode && keycode < 128) {
				memset(lineBuffer, 0, sizeof(lineBuffer));
				commandBuffer[strlen(commandBuffer)] = keycode;
				// 再描画
				onPaint(getGraphics());
			}
		}
	}

	/** 描画ハンドラ */
	virtual void onPaint(Graphics *g) {
		// 背景色で塗りつぶし
		g->setColor(Color::WHITE);
		g->fillRect(0, 0, getWidth(), getHeight());
		g->setColor(Color::BLACK);
		g->setFontStyle(Font::FIXED);
		
		// 確定ずみのprintfバッファー
		int i = 0;
		for (i = 0; i < lines->getLength(); i++) {
			String *temp = (String *)lines->get(i);
			g->drawText(temp->getBytes(), 0, i * 12);
		}
		
		// コマンドライン
		char temp[256];
		sprintf(temp, "%s%% %s", currentPath, commandBuffer);
		g->drawText(temp, 0, i * 12);
		
		// キャレット
		int x0 = strlen(temp) * 8;
		int y0 = i * 12 + 10;
		g->drawLine(x0, y0, x0 + 7, y0);
		g->drawLine(x0, y0 + 1, x0 + 7, y0 + 1);
		update();
	}
	
	/** ウィンドウ生成時に呼ばれる */
	virtual void onStart() {
		Window::onStart();
		
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
	virtual void onExit() {
		Window::onExit();
		
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
