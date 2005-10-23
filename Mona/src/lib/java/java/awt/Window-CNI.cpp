// This file was created by `gcjh -stubs'. -*- c++ -*-
//
// This file is intended to give you a head start on implementing native
// methods using CNI.
// Be aware: running `gcjh -stubs ' once more for this class may
// overwrite any edits you have made to this file.

#include <java/awt/Color.h>
#include <java/awt/Window.h>
#include <gcj/cni.h>
#ifdef MONA
#include <monapi.h>
#include <monapi/messages.h>
#include <servers/gui.h>

static guiserver_window* _window;
static guiserver_bitmap* _bitmap;
static dword _timerID;
static MonAPI::Screen screen;
static jint* buffer = NULL;
static int _width = 0, _height = 0;

/*static char* get_string(jstring s) {
	int len = s->length();
	char* cstr = new char[len + 1];
	jchar* ss = JvGetStringChars(s);
	for (int i = 0; i < len; i++)
		cstr[i] = (char)ss[i];
	cstr[len] = '\0';
	return cstr;
}*/
#else
#include <windows.h>
#include <windowsx.h>
#include <stdlib.h>

static const char* className = "SimpleCanvas";
static HINSTANCE hInst = NULL;
static HWND hWnd = NULL;
static BITMAPINFO bmpInfo;
static bool alive = true;
static jint* buffer = NULL;
static int _width = 0, _height = 0;
static int shift = 0;

static char* get_string(jstring s) {
	wchar_t* ss = (wchar_t*)JvGetStringChars(s);
	int mlen = wcstombs(NULL, ss, 0) + 1;
	char* cstr = new char[mlen];
	wcstombs(cstr, ss, mlen);
	return cstr;
}

static void paint(HWND hwnd) {
	if (hwnd != hWnd || buffer == NULL) return;

	PAINTSTRUCT ps;
	HDC hDC = BeginPaint(hwnd, &ps);
	SetDIBitsToDevice(hDC, 0, 0, _width, _height, 0, 0, 0, _height,
		buffer, &bmpInfo, DIB_RGB_COLORS);
	EndPaint(hWnd, &ps);
}

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
		case WM_PAINT:
			paint(hwnd);
			break;
		case WM_LBUTTONDOWN:
			//if (SimpleCanvas::handler != NULL)
			//	SimpleCanvas::handler->onMouseDown(1, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			break;
		case WM_RBUTTONDOWN:
			//if (SimpleCanvas::handler != NULL)
			//	SimpleCanvas::handler->onMouseDown(2, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			break;
		case WM_LBUTTONUP:
			//if (SimpleCanvas::handler != NULL)
			//	SimpleCanvas::handler->onMouseUp(1, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			break;
		case WM_RBUTTONUP:
			//if (SimpleCanvas::handler != NULL)
			//	SimpleCanvas::handler->onMouseUp(2, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			break;
		case WM_MOUSEMOVE:
			//if (SimpleCanvas::handler != NULL)
			//	SimpleCanvas::handler->onMouseMove(wParam & 3, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			break;
		case WM_KEYDOWN:
			if (wParam == VK_SHIFT)
				shift |= 1;
			else if (wParam == VK_CONTROL)
				shift |= 2;
			//if (SimpleCanvas::handler != NULL)
			//	SimpleCanvas::handler->onKeyDown(wParam, shift);
			break;
		case WM_KEYUP:
			if (wParam == VK_SHIFT)
				shift &= ~1;
			else if (wParam == VK_CONTROL)
				shift &= ~2;
			//if (SimpleCanvas::handler != NULL)
			//	SimpleCanvas::handler->onKeyUp(wParam, shift);
			break;
		case WM_CHAR:
			//if (SimpleCanvas::handler != NULL)
			//	SimpleCanvas::handler->onKeyPress(wParam);
			break;
		case WM_DESTROY:
			alive = false;
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}
#endif

jboolean
java::awt::Window::create (jintArray buf, jstring title, jint w, jint h)
{
#ifdef MONA
	buffer = elements(buf);
	_width  = w;
	_height = h;
	//char* t = get_string(title);
	//printf("%s\n", t);
	//delete [] t;
	
	// search GUI server
	dword guisvrID = monapi_get_server_thread_id(ID_GUI_SERVER);
	if (guisvrID == THREAD_UNKNOWN) {
		::printf("%s:%d:ERROR: can not connect to GUI server!\n", __FILE__, __LINE__);
		return false;
	}
	
	// register GUI server
	if (!monapi_register_to_server(ID_GUI_SERVER, MONAPI_TRUE)) {
		::printf("%s:%d:ERROR: can not register to GUI server!\n", __FILE__, __LINE__);
		return false;
	}
	
	// create window
	MessageInfo msg;
	if (MonAPI::Message::sendReceive(&msg, guisvrID, MSG_GUISERVER_CREATEWINDOW) != 0) {
		::printf("%s:%d:ERROR: can not create window!\n", __FILE__, __LINE__);
		return false;
	}
	
	// map gui_server_window
	_window = (guiserver_window *)MonAPI::MemoryMap::map(msg.arg2);
	if (_window == NULL) {
		::printf("%s:%d:ERROR: can not map guiserver_window!\n", __FILE__, __LINE__);
		return false;
	}
	
	// create bitmap
	if (MonAPI::Message::sendReceive(&msg, guisvrID, MSG_GUISERVER_CREATEBITMAP, w, h, ::java::awt::Color::lightGray)) {
		printf("%s:%d:ERROR: can not create bitmap!\n", __FILE__, __LINE__);
		return false;
	}
	if (msg.arg2 == 0) {
		return false;
	}
	
	// map guiserver_bitmap
	_bitmap = (guiserver_bitmap*)MonAPI::MemoryMap::map(msg.arg2);
	if (_bitmap == NULL) {
		printf("%s:%d:ERROR: can not map guiserver_bitmap!\n", __FILE__, __LINE__);
		return false;
	}
	
	// guiserver_window parameter
	_window->X = 0;
	_window->Y = 0;
	_window->Width  = w;
	_window->Height = w;
	_window->OffsetX = 0;
	_window->OffsetY = 0;
	_window->BufferHandle = _bitmap->Handle;
	_window->FormBufferHandle = _bitmap->Handle;
	_window->TransparencyKey = 0x00000000;
	_window->Visible = true;
	_window->Opacity = 0xff; // not transparancy
	_window->__internal2 = true;
	_window->Flags = WINDOWFLAGS_NOBORDER;
	
	// activate window
	if (MonAPI::Message::sendReceive(NULL, guisvrID, MSG_GUISERVER_ACTIVATEWINDOW, _window->Handle)) {
		::printf("%s:%d:ERROR: can not activate window!\n", __FILE__, __LINE__);
		return false;
	}
	
	return true;
#else
	hInst  = GetModuleHandle(NULL);
	buffer = elements(buf);
	_width  = w;
	_height = h;

	WNDCLASSEX wcex;
	memset(&wcex, 0, sizeof(wcex));
	wcex.cbSize        = sizeof(wcex);
	wcex.style         = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc   = WndProc;
	wcex.hInstance     = hInst;
	wcex.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszClassName = className;
	RegisterClassEx(&wcex);

	memset(&bmpInfo, 0, sizeof(bmpInfo));
	bmpInfo.bmiHeader.biSize          = sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biWidth         = w;
	bmpInfo.bmiHeader.biHeight        = -h;
	bmpInfo.bmiHeader.biPlanes        = 1;
	bmpInfo.bmiHeader.biCompression   = BI_RGB;
	bmpInfo.bmiHeader.biBitCount      = 32;

	DWORD dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU; // with border
	//DWORD dwStyle = WS_POPUP | WS_SYSMENU; // no border
	RECT r = { 0, 0, w, h };
	AdjustWindowRect(&r, dwStyle, false);
	char* t = get_string(title);
	hWnd = CreateWindow(className, t, dwStyle,
		CW_USEDEFAULT, 0, r.right - r.left, r.bottom - r.top,
		NULL, NULL, hInst, NULL);
	delete [] t;
	if (hWnd == NULL) {
		alive = false;
		return false;
	}
	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);
	return true;
#endif
}


void
java::awt::Window::run ()
{
#ifdef MONA
	MessageInfo info;
	while (this->isRunning) {
		if (!MonAPI::Message::receive(&info)) {
			switch(info.header) {
			case MSG_KEY_VIRTUAL_CODE:
				break;
			case MSG_MOUSE_INFO:
				break;
			case MSG_GUISERVER_ACTIVATED:
				break;
			case MSG_GUISERVER_DEACTIVATE:
				break;
			case MSG_GUISERVER_DISPOSEWINDOW:
				return;
			case MSG_TIMER:
				break;
			default:
				break;
			}
		}
	}
#else
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
#endif
}


void
java::awt::Window::exit ()
{
#ifdef MONA
	// dispose window
	dword guisvrID = monapi_get_server_thread_id(ID_GUI_SERVER);
	if (MonAPI::Message::sendReceive(NULL, guisvrID, MSG_GUISERVER_DISPOSEWINDOW, _window->Handle)) {
		printf("%s:%d:ERROR: can not connect to GUI server!\n", __FILE__, __LINE__);
	}
	
	// kill timer
	kill_timer(_timerID);
	
	// unregister GUI server
	monapi_register_to_server(ID_GUI_SERVER, MONAPI_FALSE);
#else
	if (hWnd != NULL) DestroyWindow(hWnd);
#endif
}


jboolean
java::awt::Window::doEvents ()
{
#ifdef MONA
	return true;
#else
	MSG msg;
	while (alive && PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return alive;
#endif
}


void
java::awt::Window::invalidate ()
{
#ifdef MONA
	//for(int y = 0; y < _height; y++) {
	//	for(int x = 0; x < _width; x++) {
	//		screen.putPixel16(x, y, buffer[y * _width + x]);
	//	}
	//}
	byte* vram = screen.getVRAM();
	int bpp = screen.getBpp();
	int sw  = screen.getWidth();
	int bypp = bpp >> 3;
	
	for (int y = 0; y < _height; y++) {
		jint* pBuff = &buffer[y * _width];
		byte* pVram = &vram[y * sw * bypp];
		for (int x = 0; x < _width; x++, pBuff++, pVram += bypp) {
			byte* p = (byte*)pBuff;
			switch (bpp) {
				case 8: // broken
					*pVram = (p[0] + p[1] + p[2]) / 3;
					break;
				case 16: // 565
					*(unsigned short*)pVram = MonAPI::Color::bpp24to565(p);
					break;
				case 24:
					pVram[0] = p[0];
					pVram[1] = p[1];
					pVram[2] = p[2];
					break;
				case 32:
					*(jint*)pVram = *pBuff;
					break;
			}
		}
	}
#else
	if (hWnd != NULL) InvalidateRect(hWnd, NULL, false);
#endif
}


