#pragma once

#include "vector2d.h"
#include <GL/glew.h>
#include <SDL2/SDL.h>

class Game;

struct Window {
	int width, height;

	static Window *CreateWindowInstance(int width, int height, bool disable_effects);

	static Window *GetWindowInstance() {
		return s_window;
	}

	void ProcessMessages();
	~Window();

protected:
	friend Game;
	static Window *s_window;
	SDL_Window *window;
	SDL_GLContext glContext;
	bool disable_effects;
	Window();
};
