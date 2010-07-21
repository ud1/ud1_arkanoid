#pragma once

#include "vector2d.h"

#include <windows.h>

struct Window {
	HWND hwnd;
	HDC hDC;
	HGLRC hRC;
	int width, height;
	int real_width, real_height;

	Vector ToFieldCoords(const Vector &window_coords, const Vector &field_log_size) const {
		return Vector(
			window_coords.x * field_log_size.x / width,
			(height - window_coords.y) * field_log_size.y / height);
	}

	Vector ToWindowCoords(const Vector &fieldcoords, const Vector &field_log_size) const {
		return Vector(
			fieldcoords.x * width / field_log_size.x,
			height - fieldcoords.y * height / field_log_size.y);
	}

	void MoveCursorTo(const Vector &window_coords) const {
		POINT p;
		p.x = (LONG) window_coords.x;
		p.y = (LONG) window_coords.y;
		ClientToScreen(hwnd, &p);
		SetCursorPos(p.x, p.y);
	}

	static Window *CreateWindowInstance(int width, int height);
	
	static Window *GetWindowInstance() {
		return window;
	}
protected:
	static Window *window;
	Window();
	static bool RegisterWindowClass();
	bool InitializeOpenGL();
	bool Create(int width, int height);
};
