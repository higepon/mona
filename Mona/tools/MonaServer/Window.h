#ifndef __WINDOW_H__
#define __WINDOW_H__

class Window
{
public:
	static ATOM RegisterClass(WNDPROC lpfnWndProc, HINSTANCE hInstance, HICON hIcon, LPCTSTR lpszMenuName, LPCTSTR szClassName)
	{
		WNDCLASSEX wcex;

		wcex.cbSize = sizeof(WNDCLASSEX); 

		wcex.style			= CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc	= lpfnWndProc;
		wcex.cbClsExtra		= 0;
		wcex.cbWndExtra		= 0;
		wcex.hInstance		= hInstance;
		wcex.hIcon			= hIcon;
		wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
		wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
		wcex.lpszMenuName	= lpszMenuName;
		wcex.lpszClassName	= szClassName;
		wcex.hIconSm		= hIcon;

		return RegisterClassEx(&wcex);
	}

	static HWND CreateForm(HINSTANCE hInstance, LPCTSTR szWindowClass, LPCTSTR szText, int nCmdShow, int width, int height)
	{
		DWORD dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
		RECT r = { 0, 0, width, height };
		AdjustWindowRect(&r, dwStyle, TRUE);
		HWND hWnd = CreateWindow(szWindowClass, szText, dwStyle,
			CW_USEDEFAULT, 0, r.right - r.left, r.bottom - r.top, NULL, NULL, hInstance, NULL);
		if (hWnd == NULL) return NULL;

		ShowWindow(hWnd, nCmdShow);
		UpdateWindow(hWnd);
		return hWnd;
	}
};

#endif
