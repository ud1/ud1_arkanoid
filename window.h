#pragma once

#include "vector2d.h"

#include <windows.h>

struct Window {
	HWND hwnd;
	HDC hDC;
	HGLRC hRC;
	int width, height;
	int real_width, real_height;

	void MoveCursorTo(int x, int y) const {
		POINT p;
		p.x = (LONG) x;
		p.y = (LONG) y;
		ClientToScreen(hwnd, &p);
		SetCursorPos(p.x, p.y);
	}

	static Window *CreateWindowInstance(int width, int height, bool disable_effects);
	
	static Window *GetWindowInstance() {
		return window;
	}
protected:
	static Window *window;
	bool disable_effects;
	Window();
	static bool RegisterWindowClass();
	bool InitializeOpenGL();
	bool Create(int width, int height);
};
