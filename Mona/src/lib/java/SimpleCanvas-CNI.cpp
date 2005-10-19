#include "SimpleCanvas.h"
#include <gcj/cni.h>
#ifdef MONA
#include <monapi.h>

static MonAPI::Screen screen;
static jint* buffer = NULL;
static int width = 0, height = 0;

static char* get_string(jstring s) {
	int len = s->length();
	char* cstr = new char[len + 1];
	jchar* ss = JvGetStringChars(s);
	for (int i = 0; i < len; i++)
		cstr[i] = (char)ss[i];
	cstr[len] = '\0';
	return cstr;
}

jboolean SimpleCanvas::create(jintArray buf, jstring title, jint w, jint h) {
	buffer = elements(buf);
	width  = w;
	height = h;
	char* t = get_string(title);
	printf("%s\n", t);
	delete [] t;
	return true;
}

jboolean SimpleCanvas::doEvents() {
	//for(int y = 0; y < height; y++) {
	//	for(int x = 0; x < width; x++) {
	//		screen.putPixel16(x, y, buffer[y * width + x]);
	//	}
	//}
	byte* vram = screen.getVRAM();
	int bpp = screen.getBpp();
	int sw  = screen.getWidth();
	int bypp = bpp >> 3;
	
	for (int y = 0; y < height; y++) {
		jint* pBuff = &buffer[y * width];
		byte* pVram = &vram[y * sw * bypp];
		for (int x = 0; x < width; x++, pBuff++, pVram += bypp) {
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
	return true;
}

void SimpleCanvas::invalidate() {
}
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
static int width = 0, height = 0;
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
	SetDIBitsToDevice(hDC, 0, 0, width, height, 0, 0, 0, height,
		buffer, &bmpInfo, DIB_RGB_COLORS);
	EndPaint(hWnd, &ps);
}

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
		case WM_PAINT:
			paint(hwnd);
			break;
		case WM_LBUTTONDOWN:
			if (SimpleCanvas::handler != NULL)
				SimpleCanvas::handler->onMouseDown(1, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			break;
		case WM_RBUTTONDOWN:
			if (SimpleCanvas::handler != NULL)
				SimpleCanvas::handler->onMouseDown(2, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			break;
		case WM_LBUTTONUP:
			if (SimpleCanvas::handler != NULL)
				SimpleCanvas::handler->onMouseUp(1, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			break;
		case WM_RBUTTONUP:
			if (SimpleCanvas::handler != NULL)
				SimpleCanvas::handler->onMouseUp(2, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			break;
		case WM_MOUSEMOVE:
			if (SimpleCanvas::handler != NULL)
				SimpleCanvas::handler->onMouseMove(wParam & 3, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			break;
		case WM_KEYDOWN:
			if (wParam == VK_SHIFT)
				shift |= 1;
			else if (wParam == VK_CONTROL)
				shift |= 2;
			if (SimpleCanvas::handler != NULL)
				SimpleCanvas::handler->onKeyDown(wParam, shift);
			break;
		case WM_KEYUP:
			if (wParam == VK_SHIFT)
				shift &= ~1;
			else if (wParam == VK_CONTROL)
				shift &= ~2;
			if (SimpleCanvas::handler != NULL)
				SimpleCanvas::handler->onKeyUp(wParam, shift);
			break;
		case WM_CHAR:
			if (SimpleCanvas::handler != NULL)
				SimpleCanvas::handler->onKeyPress(wParam);
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

jboolean SimpleCanvas::create(jintArray buf, jstring title, jint w, jint h) {
	hInst  = GetModuleHandle(NULL);
	buffer = elements(buf);
	width  = w;
	height = h;

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

	DWORD dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;
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
}

void SimpleCanvas::run() {
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void SimpleCanvas::exit() {
	if (hWnd != NULL) DestroyWindow(hWnd);
}

jboolean SimpleCanvas::doEvents() {
	MSG msg;
	while (alive && PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return alive;
}

void SimpleCanvas::invalidate() {
	if (hWnd != NULL) InvalidateRect(hWnd, NULL, false);
}
#endif
