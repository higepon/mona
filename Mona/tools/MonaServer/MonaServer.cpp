// Kernel.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#define WIN32_LEAN_AND_MEAN		// Windows ヘッダーから使用されていない部分を除外します。
// Windows ヘッダー ファイル :
#include <windows.h>
#include <windowsx.h>
#include <list>
#include <string>

#include "stdafx.h"
#include "Window.h"
#include "resource.h"
#include "MonaMessage.h"

#define MAX_LOADSTRING 100

// グローバル変数 :
HINSTANCE hInst;								// 現在のインターフェイス
TCHAR szTitle[MAX_LOADSTRING];					// タイトル バーのテキスト
TCHAR szWindowClass[MAX_LOADSTRING];			// メイン ウィンドウ クラス名
HANDLE __mona_message_mutex;
MessageInfo* __mona_messages;

struct MonaProcess
{
	dword tid;
	std::string name;

	MonaProcess(dword tid, const std::string& name) : tid(tid), name(name) {}
	bool operator==(const MonaProcess& mp) const { return this->tid == mp.tid; }
};

static std::list<MonaProcess> processes;
static DWORD server_tid = THREAD_UNKNOWN;
static HWND hMainWindow;
static BITMAPINFO bitmapInfo;
static HANDLE hBuffer, hMessages;
static unsigned int* buffer;
static bool nodraw = false;

// mouse
static dword mouse_server = THREAD_UNKNOWN, keyboard_server = THREAD_UNKNOWN;
static int mouseButtons = 0;

static const int WindowWidth = 800, WindowHeight = 600;

// このコード モジュールに含まれる関数の宣言を転送します :
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	// グローバル変数にインスタンス処理を格納します。
	hInst = hInstance;

	// グローバル文字列を初期化しています。
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_KERNEL, szWindowClass, MAX_LOADSTRING);

	__mona_message_mutex = CreateMutex(NULL, FALSE, MONASERVER_MESSAGE_MUTEX);
	if (GetLastError() != 0)
	{
		MessageBox(NULL, TEXT("既に起動しています。"), szTitle, MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}
	server_tid = GetCurrentThreadId();

 	// TODO: ここにコードを挿入してください。
	bitmapInfo.bmiHeader.biSize          = sizeof(BITMAPINFOHEADER);
	bitmapInfo.bmiHeader.biWidth         = WindowWidth;
	bitmapInfo.bmiHeader.biHeight        = -WindowHeight;
	bitmapInfo.bmiHeader.biPlanes        = 1;
	bitmapInfo.bmiHeader.biCompression   = BI_RGB;
	bitmapInfo.bmiHeader.biSizeImage     = 0;
	bitmapInfo.bmiHeader.biXPelsPerMeter = 0;
	bitmapInfo.bmiHeader.biYPelsPerMeter = 0;
	bitmapInfo.bmiHeader.biClrUsed       = 0;
	bitmapInfo.bmiHeader.biClrImportant  = 0;
	bitmapInfo.bmiHeader.biBitCount = 32;

	int len = WindowWidth * WindowHeight;
	hBuffer = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, len * 4, MONASERVER_VRAM);
	if (hBuffer == NULL)
	{
		MessageBox(NULL, TEXT("共有メモリの作成に失敗しました。"), szTitle, MB_OK | MB_ICONEXCLAMATION);
		CloseHandle(__mona_message_mutex);
		return FALSE;
	}
	buffer = (unsigned int*)MapViewOfFile(hBuffer, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if (buffer == NULL)
	{
		MessageBox(NULL, TEXT("共有メモリのマップに失敗しました。"), szTitle, MB_OK | MB_ICONEXCLAMATION);
		CloseHandle(hBuffer);
		CloseHandle(__mona_message_mutex);
		return FALSE;
	}
	memset(buffer, 0xff, len * sizeof(unsigned int));

	hMessages = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0,
		MONASERVER_MESSAGE_LIMIT * sizeof(MessageInfo), MONASERVER_MESSAGES);
	if (hMessages == NULL)
	{
		MessageBox(NULL, TEXT("共有メモリの作成に失敗しました。"), szTitle, MB_OK | MB_ICONEXCLAMATION);
		CloseHandle(hBuffer);
		CloseHandle(__mona_message_mutex);
		return FALSE;
	}
	__mona_messages = (MessageInfo*)MapViewOfFile(hMessages, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if (__mona_messages == NULL)
	{
		MessageBox(NULL, TEXT("共有メモリのマップに失敗しました。"), szTitle, MB_OK | MB_ICONEXCLAMATION);
		CloseHandle(hMessages);
		CloseHandle(hBuffer);
		CloseHandle(__mona_message_mutex);
		return FALSE;
	}
	memset(__mona_messages, 0, MONASERVER_MESSAGE_LIMIT * sizeof(MessageInfo));

	g_vesaDetail.bytesPerScanLine = WindowWidth * 4;
	g_vesaDetail.xResolution      = WindowWidth;
	g_vesaDetail.yResolution      = WindowHeight;
	g_vesaDetail.bitsPerPixel     = 32;
	g_vesaDetail.physBasePtr      = (byte*)buffer;

	// アプリケーションの初期化を実行します。:
	Window::RegisterClass(WndProc, hInstance, LoadIcon(hInstance, (LPCTSTR)IDI_KERNEL), (LPCTSTR)IDC_KERNEL, szWindowClass);
	hMainWindow = Window::CreateForm(hInstance, szWindowClass, szTitle, nCmdShow, WindowWidth, WindowHeight);
	if (hMainWindow == NULL) return FALSE;

	HACCEL hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_KERNEL);

	startKernel();

	// メイン メッセージ ループ :
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	UnmapViewOfFile(__mona_messages);
	CloseHandle(hMessages);
	UnmapViewOfFile(buffer);
	CloseHandle(hBuffer);
	CloseHandle(__mona_message_mutex);

	return (int) msg.wParam;
}

void RefreshMainWindow()
{
	if (!nodraw) InvalidateRect(hMainWindow, NULL, TRUE);
}

LRESULT WndProcSystemCall(WPARAM wParam)
{
	MessageInfo mi;
	MonaMessage::Receive(&mi, (int)wParam);
	switch (mi.header)
	{
	case 0xffff: //SYSTEM_CALL_REGISTER_PROCESS:
		if (mi.arg1 == 1)
		{
			bool ok = true;
			for (std::list<MonaProcess>::iterator it = processes.begin(); it != processes.end(); it++)
			{
				if (it->tid == mi.from)
				{
					ok = false;
					break;
				}
			}
			if (ok) processes.push_back(MonaProcess(mi.from, mi.str));
			//g_console->printf("Process Started: %d, %s\r\n", mi.from, mi.str);
			if (strcmp(mi.str, "MouseServer") == 0)
			{
				g_console->printf("Loading Mouse Server...OK!\n");
				mouse_server = mi.from;
			}
			else if (strcmp(mi.str, "KeyboardServer") == 0)
			{
				g_console->printf("Loading Keyboard Server...OK!\n");
				keyboard_server = mi.from;
			}
		}
		else
		{
			for (std::list<MonaProcess>::iterator it = processes.begin(); it != processes.end(); it++)
			{
				if (it->tid == mi.from)
				{
					processes.remove(*it);
					break;
				}
			}
			//g_console->printf("Process Terminated: %d, %s\r\n", mi.from, mi.str);
		}
		return 1;
	case SYSTEM_CALL_LOOKUP_MAIN_THREAD:
		for (std::list<MonaProcess>::iterator it = processes.begin(); it != processes.end(); it++)
		{
			if (it->name == mi.str) return (LRESULT)it->tid;
		}
		return THREAD_UNKNOWN;
    case SYSTEM_CALL_SET_CURSOR:
		g_console->setCursor((int)mi.arg1, (int)mi.arg2);
		return 0;
    case SYSTEM_CALL_GET_CURSOR:
		{
			int x, y;
			g_console->getCursor(&x, &y);
			return MAKELRESULT(x, y);
		}
	case SYSTEM_CALL_CLEAR_SCREEN:
		g_console->clearScreen();
		return 0;
	}

	return 0;
}

void WndProcMessage(WPARAM wParam)
{
	MessageInfo mi;
	MonaMessage::Receive(&mi, (int)wParam);
}

//
//  関数 : WndProc(HWND, unsigned, WORD, LONG)
//
//  目的 :  メイン ウィンドウのメッセージを処理します。
//
//  WM_COMMAND	- アプリケーション メニューの処理
//  WM_PAINT	- メイン ウィンドウの描画
//  WM_DESTROY	- 中止メッセージを表示して戻る
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	static bool nodraw2 = false;

	switch (message) 
	{
	case WM_MONA_REFRESH_SCREEN:
		if (!nodraw2) RefreshMainWindow();
		nodraw2 = true;
		return 1;
	case WM_MONA_SYSTEM_CALL:
		return WndProcSystemCall(wParam);
	case WM_MONA_MESSAGE:
		WndProcMessage(wParam);
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam); 
		wmEvent = HIWORD(wParam); 
		// 選択されたメニューの解析 :
		switch (wmId)
		{
		case IDM_ABOUT:
			nodraw = true;
			DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_ERASEBKGND:
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: 描画コードをここに追加してください...
		SetDIBitsToDevice(hdc, 0, 0, WindowWidth, WindowHeight, 0, 0, 0, WindowHeight, buffer, &bitmapInfo, DIB_RGB_COLORS);
		EndPaint(hWnd, &ps);
		nodraw2 = false;
		break;
	case WM_SETTEXT:
		g_console->printf((LPCTSTR)lParam);
		return FALSE;
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
		switch (message)
		{
			case WM_LBUTTONDOWN:
				mouseButtons |= 1;
				break;
			case WM_RBUTTONDOWN:
				mouseButtons |= 2;
				break;
			case WM_LBUTTONUP:
				mouseButtons &= ~1;
				break;
			case WM_RBUTTONUP:
				mouseButtons &= ~2;
				break;
		}
		if (mouse_server != THREAD_UNKNOWN)
		{
			///MonaMessage::Send(mouse_server, server_tid, MSG_MOUSE_INFO, mouseButtons, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		}
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
		if (keyboard_server != THREAD_UNKNOWN)
		{
			dword scancode = HIWORD(lParam) & 0xff;
			if (message == WM_KEYUP) scancode |= 0x80;
			///MonaMessage::Send(keyboard_server, server_tid, MSG_KEY_SCANCODE, scancode);
		}
		break;
	case WM_DESTROY:
		for (std::list<MonaProcess>::iterator it = processes.begin(); it != processes.end(); it++)
		{
			PostThreadMessage(it->tid, WM_QUIT, 0, 0);
		}
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// バージョン情報ボックスのメッセージ ハンドラです。
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		return TRUE;

	case WM_SHOWWINDOW:
		nodraw = false;
		RefreshMainWindow();
		break;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
		{
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
	return FALSE;
}
