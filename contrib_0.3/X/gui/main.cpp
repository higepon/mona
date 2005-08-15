/*!
	\file  main.cpp
	\brief  MonaForms GUIサーバ メイン

	This software is in the public domain.
	There are no restrictions on any sort of usage of this software.

	\author   
	\version $Revision$
	\date   create: update:$Date$
*/

#include "Rectangle.h"
#define _R ::System::Drawing::Rectangle

#include <servers/gui.h>
#include <monapi/messages.h>
#include <monapi/CString.h>
#include <monalibc.h>

#include "image.h"
#include "screen.h"
#include "utils.h"
#include "window.h"

using namespace MonAPI;

// 画面バッファ
extern guiserver_bitmap* screen_buffer;

// プロセス間共通パラメータ
CommonParameters* commonParams;
// 壁紙
guiserver_bitmap* wallpaper = NULL;
// 生成効果、削除効果、効果のステップ幅、効果の刻み時間(ms)
int we_creation = 0, we_destruction = 0, we_step = 6, we_wait = 30;

// クライアントのリスト
static HList<dword> clients;
// 共有メモリ上に確保するフォント
static monapi_cmemoryinfo* default_font = NULL;
// 壁紙を開くときにプロンプトに情報を表示するかどうか
static bool wallpaper_prompt = false;
// スタートアップアプリケーション
static HList<CString>* startup = NULL;

/*!
\brief ReadFont
	 共有メモリに指定フォントをロード
\param  const char* file [in] フォントファイル名
\author 
\date   create: update:$Date$
*/
static void ReadFont(const char* file)
{
	monapi_cmemoryinfo* mi = monapi_call_file_decompress_st5_file(file, true);
	if (mi == NULL) return;
	
	default_font = mi;
}

/*!
\brief DrawWallPaper
	 壁紙を描画する
\param  guiserver_bitmap* bmp [in] 壁紙
\param  int pos [in] 表示位置
\param  unsigned int transparent [in] 透明度 (0x0-0xFFFFFF)
\param  const char* src [in] 壁紙ファイル名
\param  bool prompt [in] プロンプトを表示するかどうか
\author 
\date   create: update:$Date$
*/
static void DrawWallPaper(guiserver_bitmap* bmp, int pos, unsigned int transparent, const char* src, bool prompt)
{
	if (wallpaper == NULL || bmp == NULL) return;
	
	int x = 0, y = 0, ww = wallpaper->Width, wh = wallpaper->Height;
	switch (pos)
	{
		case 0: // Stretch
		{
			if (ww != bmp->Width || wh != bmp->Height)
			{
				if (prompt) printf("%s: Resizing %s....", GUI_SERVER_NAME, (const char*)src);
				guiserver_bitmap* bmp2 = ResizeBitmap(bmp, ww, wh);
				DisposeBitmap(bmp->Handle);
				bmp = bmp2;
				if (prompt) printf("Done\n");
			}
			break;
		}
		case 1: // Top Left
			break;
		case 2: // Top Center
			x = (ww - bmp->Width) / 2;
			break;
		case 3: // Top Right
			x = ww - bmp->Width;
			break;
		case 4: // Center Left
			y = (wh - bmp->Height) / 2;
			break;
		case 5: // Center Center
			x = (ww - bmp->Width) / 2;
			y = (wh - bmp->Height) / 2;
			break;
		case 6: // Center Right
			x = ww - bmp->Width;
			y = (wh - bmp->Height) / 2;
			break;
		case 7: // Bottom Left
			y = wh - bmp->Height;
			break;
		case 8: // Bottom Center
			x = (ww - bmp->Width) / 2;
			y = wh - bmp->Height;
			break;
		case 9: // Bottom Right
			x = ww - bmp->Width;
			y = wh - bmp->Height;
			break;
	}
	DrawImage(wallpaper, bmp, x, y, 0, 0, -1, -1, transparent);
	DisposeBitmap(bmp->Handle);
}

/*!
\brief DrawWallPaper
	 壁紙を描画する
\param  const char* src [in] 壁紙ファイル名
\param  int pos [in] 表示位置
\param  unsigned int transparent [in] 透明度 (0x0-0xFFFFFF)
\param  int background [in] 背景色 (0x0-0xFFFFFF)
\author 
\date   create: update:$Date$
*/
static void DrawWallPaper(const char* src, int pos, unsigned int transparent, int background)
{
	bool prompt = wallpaper_prompt;
	wallpaper_prompt = false;
	
	Screen* scr = GetDefaultScreen();
	if (wallpaper == NULL)
	{
		wallpaper = CreateBitmap(scr->getWidth(), scr->getHeight(), background);
		if (wallpaper == NULL) return;
	}
	else
	{
		FillColor(wallpaper, background);
	}
	
	guiserver_bitmap* bmp = ReadImage(src, prompt);
	if (bmp != NULL) DrawWallPaper(bmp, pos, transparent, src, prompt);
	DrawImage(screen_buffer, wallpaper);
	DrawScreen();
}


/*!
\brief ReadConfig
	 設定ファイル *.INI よりMonaFormsの設定を得る
\author 
\date   create: update:$Date$
*/
static void ReadConfig(List<char*>* pekoe)
{
	monapi_cmemoryinfo* cfg = monapi_call_file_read_data(pekoe->get(0), MONAPI_TRUE);
	if (cfg == NULL) return;
	
	if (startup != NULL)
	{
		delete startup;
		startup = NULL;
	}
	char line[256];
	int linepos = 0, wppos = 5;
	unsigned int wptp = 0, bgcol = 0;
	CString section, src;
	for (dword pos = 0; pos <= cfg->Size; pos++)
	{
		char ch = pos < cfg->Size ? (char)cfg->Data[pos] : '\n';
		if (ch == '\r' || ch == '\n')
		{
			if (linepos > 0)
			{
				CString ln(line, linepos);
				if (ln.toUpper().startsWith("REM "))
				{
					// ignore remark
				}
				else if (ln.startsWith("[") && ln.endsWith("]"))
				{
					section = ln.substring(1, ln.getLength() - 2).toUpper();
				}
				else if (ln.indexOf('=') > 0)
				{
					_A<CString> data = CString(line, linepos).split('=');
					if (data.get_Length() == 2 && data[0] != NULL && data[1] != NULL)
					{
						CString name = data[0].toUpper();
						if (section == "GENERAL")
						{
							if (name == "RUN")
							{
								if (startup == NULL) startup = new HList<CString>();
								_A<CString> runs = data[1].split(',');
								FOREACH (CString, r, runs) startup->add(r); END_FOREACH
							}
						}
						else if (section == "WALLPAPER")
						{
							if (name == "SOURCE")
							{
								src = data[1];
							}
							else if (name == "POSITION")
							{
								wppos = atoi(data[1]);
							}
							else if (name == "TRANSPARENCYKEY")
							{
								wptp = xtoui(data[1]);
							}
							else if (name == "BACKCOLOR")
							{
								bgcol = ((unsigned int)xtoui(data[1])) | 0xff000000;
							}
						}
						else if (section == "WINDOW EFFECT")
						{
							if (name == "CREATION")
							{
								we_creation = atoi(data[1]);
							}
							else if (name == "DESTRUCTION")
							{
								we_destruction = atoi(data[1]);
							}
							else if (name == "STEP")
							{
								we_step = atoi(data[1]);
							}
							else if (name == "WAIT")
							{
								we_wait = atoi(data[1]);
							}
						}
					}
				}
				linepos = 0;
			}
		}
		else if (linepos < 255)
		{
			line[linepos++] = ch;
		}
	}
	monapi_cmemoryinfo_dispose(cfg);
	monapi_cmemoryinfo_delete(cfg);
	if (src[0] == '\0') return;

	wallpaper_prompt = true;
	DrawWallPaper(src, wppos, wptp, bgcol);
}

/*!
\brief CheckGUIServer
	 MonaFormsサーバ 起動済みチェック
	 既に起動していれば、2重起動せず終了する。
\author 
\date   create: update:$Date$
*/
static void CheckGUIServer()
{
	syscall_set_ps_dump();
	PsInfo info;

	bool ok = true;
	CString self = "GUI.EX5";
	dword tid = MonAPI::System::getThreadID();

	while (syscall_read_ps_dump(&info) == 0)
	{
		if (ok && self == info.name && tid != info.tid) ok = false;
	}
	if (ok) return;

	printf("%s: already has executed!\n", GUI_SERVER_NAME);
	exit(1);
}

/*!
\brief MessageLoop
	 MonaFormsサーバ メッセージループ
\author 
\date   create: update:$Date$
*/
static void MessageLoop()
{
	for (MessageInfo msg;;)
	{
		if (Message::receive(&msg)) continue;
		
		switch (msg.header)
		{
			// フォント取得要求
			case MSG_GUISERVER_GETFONT:
				// フォントのハンドル、フォントのサイズ
				Message::reply(&msg, default_font->Handle, default_font->Size);
				break;
			// 壁紙ロード要求
			case MSG_GUISERVER_SETWALLPAPER:
				// ファイル名、表示位置、透明度、背景色
				DrawWallPaper(msg.str, msg.arg1, msg.arg2, msg.arg3);
				Message::reply(&msg);
				break;
			// 画面更新要求
			case MSG_GUISERVER_DRAWSCREEN:
				// x, y, w, h
				DrawScreen(msg.arg1, msg.arg2, GET_X_DWORD(msg.arg3), GET_Y_DWORD(msg.arg3));
				Message::reply(&msg);
				break;
			// 登録要求
			case MSG_REGISTER_TO_SERVER:
				// threadID
				clients.add(msg.arg1);
				Message::reply(&msg);
				break;
			// 削除要求
			case MSG_UNREGISTER_FROM_SERVER:
				// threadID
				clients.remove(msg.arg1);
				Message::reply(&msg);
				break;
			// ESCキーによる終了チェック
			case MSG_KEY_VIRTUAL_CODE:
			{
				if (msg.arg1 == Keys::Escape) {
					DisposeAllWindow();
					return;
				}
				if (ImageHandler(&msg)) break;
				if (WindowHandler(&msg)) break;
				break;
			}
			default:
				if (ImageHandler(&msg)) break;
				if (WindowHandler(&msg)) break;
				break;
		}
	}
}

static dword my_tid, stdout_tid;

/** 標準出力を握りつぶすスレッド */
static void StdoutMessageLoop()
{
	MonAPI::Message::send(my_tid, MSG_SERVER_START_OK);

	for (MessageInfo msg;;)
	{
		if (MonAPI::Message::receive(&msg) != 0) continue;

		switch (msg.header)
		{
			case MSG_PROCESS_STDOUT_DATA:
			{
				MonAPI::Message::reply(&msg);
				dword tid1 = MonAPI::Message::lookupMainThread("GSHELL.EX5");
				if (tid1 != THREAD_UNKNOWN) {
					Message::send(tid1, MSG_PROCESS_STDOUT_DATA, 0, 0, 0, msg.str);
				}
				break;
			}
		}
	}
}

/** 標準出力用のスレッドを作成 */
static void InitThread()
{
	my_tid = syscall_get_tid();
	dword id = syscall_mthread_create((dword)StdoutMessageLoop);
	syscall_mthread_join(id);
	MessageInfo msg, src;
	src.header = MSG_SERVER_START_OK;
	MonAPI::Message::receive(&msg, &src, MonAPI::Message::equalsHeader);
	stdout_tid = msg.from;
}

/*!
\brief MonaMain
	MonaFormsサーバ メイン
\param List<char*>* pekoe [in] コマンドライン引数
\author
\date   create: update:$Date$
*/
int MonaMain(List<char*>* pekoe)
{
	// 引数チェック
	if (pekoe->size() != 1)
	{
		printf("%s: usage GUI.EX5 [.INI FILE]");
		exit(1);
	}

	// 2重起動チェック
	CheckGUIServer();
	if (!InitScreen()) exit(1);

	MessageInfo msg_cp;
	if (Message::sendReceive(&msg_cp, monapi_get_server_thread_id(ID_PROCESS_SERVER), MSG_PROCESS_GET_COMMON_PARAMS) != 0)
	{
		printf("%s: can not get common parameters!\n", GUI_SERVER_NAME);
		exit(1);
	}
	commonParams = (CommonParameters*)MemoryMap::map(msg_cp.arg2);
	
	if (!monapi_register_to_server(ID_MOUSE_SERVER, MONAPI_TRUE)) exit(1);
	if (!monapi_register_to_server(ID_KEYBOARD_SERVER, MONAPI_TRUE)) exit(1);

	// フォントのロード
	ReadFont("/APPS/MONA12.MF5");
	if (default_font == NULL) exit(1);
	
	// 設定ファイルのロード
	ReadConfig(pekoe);
	if (startup != NULL)
	{
		for (int i = 0; i < startup->size(); i++) {
			monapi_call_process_execute_file(startup->get(i), MONAPI_FALSE);
		}
	}

	// MONITORサーバへの正常起動通知
	Message::send(Message::lookupMainThread("MONITOR.BIN"), MSG_SERVER_START_OK);

	InitThread();
	dword process = monapi_get_server_thread_id(ID_PROCESS_SERVER);
	if (process != THREAD_UNKNOWN)
	{
		MonAPI::Message::sendReceive(NULL, process + 1, MSG_PROCESS_GRAB_STDOUT, stdout_tid);
	}

	// メッセージループ
	MessageLoop();

	if (process != THREAD_UNKNOWN)
	{
		MonAPI::Message::sendReceive(NULL, process + 1, MSG_PROCESS_UNGRAB_STDOUT, stdout_tid);
	}
	syscall_kill_thread(stdout_tid);

	// 共有メモリにロードしているフォントの解放
	monapi_cmemoryinfo_dispose(default_font);
	monapi_cmemoryinfo_delete(default_font);
	
	// 壁紙の開放
	if (wallpaper != NULL) DisposeBitmap(wallpaper->Handle);
	DisposeScreen();
	
	monapi_register_to_server(ID_MOUSE_SERVER, MONAPI_FALSE);
	monapi_register_to_server(ID_KEYBOARD_SERVER, MONAPI_FALSE);
	MemoryMap::unmap(msg_cp.arg2);
	
	monapi_call_mouse_set_cursor(0);
	syscall_set_cursor(0,0);
	syscall_clear_screen();
	monapi_call_mouse_set_cursor(1);
	
	return 0;
}
