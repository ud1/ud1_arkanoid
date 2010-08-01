#include "window.h"

#include <gl/GL.h>

Window *Window::window = NULL;

void Reshape() {
	Window *window = Window::GetWindowInstance();
	glViewport (0, 0, (GLsizei)(window->width), (GLsizei)(window->height));
	glDisable(GL_DEPTH_TEST);
	glDepthMask(0);
}

void close_main_window();
void lbdown(int x, int y);
void lbup(int x, int y);
void key_down(int key);
void key_up(int key);
void mmove(int x, int y);
void on_char(char ch);

void on_deactivate();

LRESULT CALLBACK WindowProc (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	Window *window = Window::GetWindowInstance();
	switch (uMsg) {
	case WM_CLOSE:
		close_main_window();
		break;

	case WM_GETMINMAXINFO: 
		{
			PMINMAXINFO info = (PMINMAXINFO) lParam;
			info->ptMinTrackSize.x = window->real_width;
			info->ptMinTrackSize.y = window->real_height;
			info->ptMaxTrackSize.x = window->real_width;
			info->ptMaxTrackSize.y = window->real_height;
			break;
		}

	case WM_ACTIVATE:
		if (wParam == WA_INACTIVE)
			on_deactivate();
		break;

	case WM_ACTIVATEAPP:
		if (wParam == FALSE)
			on_deactivate();
		break;

	case WM_SIZE:
		window->width = LOWORD(lParam);
		window->height = HIWORD(lParam);
		Reshape();
		return 0;

	case WM_LBUTTONDOWN:
		lbdown(LOWORD(lParam), HIWORD(lParam));
		break;

	case WM_LBUTTONUP:
		lbup(LOWORD(lParam), HIWORD(lParam));
		break;

	case WM_MOUSEMOVE:
		mmove(LOWORD(lParam), HIWORD(lParam));
		break;

	case WM_CHAR:
		on_char(wParam);
		break;

	case WM_KEYDOWN:
		key_down(wParam);
		break;

	case WM_KEYUP:
		key_up(wParam);
		break;
	}
	return DefWindowProc (hWnd, uMsg, wParam, lParam);
}

bool Window::RegisterWindowClass() {
	static bool already_registered = false;
	if (already_registered)
		return true;

	WNDCLASSEX windowClass;
	ZeroMemory( &windowClass, sizeof( WNDCLASSEX ) );
	windowClass.cbSize			= sizeof (WNDCLASSEX);
	windowClass.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	windowClass.lpfnWndProc		= (WNDPROC)(WindowProc);
	windowClass.hInstance		= GetModuleHandle(NULL);
	windowClass.hbrBackground	= (HBRUSH)(COLOR_APPWORKSPACE);
	windowClass.hCursor			= LoadCursor(NULL, IDC_ARROW);
	windowClass.lpszClassName	= TEXT("ud1_arkanoid");

	if (RegisterClassEx (&windowClass) == 0) {
		return false;
	}

	already_registered = true;
	return true;
}

bool Window::InitializeOpenGL() {
	PIXELFORMATDESCRIPTOR pfd;
	pfd.nSize			= sizeof (PIXELFORMATDESCRIPTOR);	
	pfd.nVersion		= 1;
	pfd.dwFlags			= PFD_DRAW_TO_WINDOW |
		PFD_SUPPORT_OPENGL |
		PFD_DOUBLEBUFFER;
	pfd.iPixelType		= PFD_TYPE_RGBA;
	pfd.cColorBits		= 24;
	pfd.cRedBits		=
		pfd.cRedShift	=
		pfd.cGreenBits	=
		pfd.cGreenShift	=
		pfd.cBlueBits	=
		pfd.cBlueShift	= 0;
	pfd.cAlphaBits		=
		pfd.cAlphaShift	= 0;
	pfd.cAccumBits		= 0;
	pfd.cAccumRedBits	=
		pfd.cAccumGreenBits =
		pfd.cAccumBlueBits	= 
		pfd.cAccumAlphaBits	= 0;

	pfd.cDepthBits		= 0;
	pfd.cStencilBits	= disable_effects ? 0 : 1;
	pfd.cAuxBuffers		= 0;
	pfd.iLayerType		= 0;
	pfd.bReserved		= 0;
	pfd.dwLayerMask		= 0;
	pfd.dwVisibleMask	= 0;
	pfd.dwDamageMask	= 0;

	int pixelFormat = ChoosePixelFormat (hDC, &pfd);
	if (pixelFormat == 0) {
		return false;
	}

	if (SetPixelFormat (hDC, pixelFormat, NULL) == FALSE) {
		return false;
	}

	hRC = wglCreateContext (hDC);
	if (hRC == 0) {
		return false;
	}

	if (wglMakeCurrent (hDC, hRC) == FALSE) {
		wglDeleteContext (hRC);
		return false;
	}

	Reshape();
	return true;
}

Window *Window::CreateWindowInstance(int w, int h, bool disable_effects) {
	if ( !RegisterWindowClass() ) {
		MessageBox (HWND_DESKTOP, TEXT("RegisterClassEx Failed!"), TEXT("Error"), MB_OK | MB_ICONEXCLAMATION);
		return NULL;
	}

	if (window == NULL)
		window = new Window;

	window->disable_effects = disable_effects;

	if (!window->Create(w, h)) {
		delete window;
		window = NULL;
		return NULL;
	}

	return window;
}

Window::Window() {
	
}

bool Window::Create(int w, int h) {
	DWORD	windowStyle			= WS_POPUP | WS_OVERLAPPEDWINDOW;
	DWORD	windowExtendedStyle = WS_EX_APPWINDOW;

	int wnd_x = 100, wnd_y = 100;
	tagRECT scrRect = {wnd_x, wnd_y, wnd_x + w, wnd_y + h};
	AdjustWindowRectEx(&scrRect, windowStyle, false, windowExtendedStyle);

	hwnd = CreateWindowEx (
		windowExtendedStyle,
		TEXT("ud1_arkanoid"),	// Class Name
		TEXT("ud1 arkanoid (Для конкурса арканоидов gamedev.ru)"),	// Window Title
		windowStyle,
		scrRect.left, scrRect.top,				// Window x,y Position
		real_width = scrRect.right - scrRect.left,		// Window Width
		real_height = scrRect.bottom - scrRect.top,		// Window Height
		HWND_DESKTOP,			// Desktop Is Window's Parent
		0,						// No Menu
		GetModuleHandle(NULL),	// Pass The Window Instance
		NULL);

	if ( !hwnd ) {
		MessageBox (HWND_DESKTOP, TEXT("Create window failed!"), TEXT("Error"), MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	ShowWindow(hwnd, TRUE);

	hDC = GetDC(hwnd);
	if (!InitializeOpenGL()) {
		MessageBox (HWND_DESKTOP, TEXT("GL initialization failed!"), TEXT("Error"), MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	return true;
}
