#include "window.h"
#include <iostream>

Window *Window::s_window = NULL;

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

void on_deactivate();

void Window::ProcessMessages() {
	SDL_Event e;
	while (SDL_PollEvent(&e))
	{
		if (e.type == SDL_QUIT) {
			close_main_window();
		} else if (e.type == SDL_MOUSEMOTION) {
			mmove(e.motion.xrel, e.motion.yrel);
		} else if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
			lbdown(e.button.x, e.button.y);
		} else if (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT) {
			lbup(e.button.x, e.button.y);
		} else if (e.type == SDL_KEYDOWN) {
			key_down(e.key.keysym.sym);
		} else if (e.type == SDL_KEYUP) {
			key_up(e.key.keysym.sym);
		} else if (e.type == SDL_WINDOWEVENT && (e.window.event == SDL_WINDOWEVENT_FOCUS_LOST ||
			e.window.event== SDL_WINDOWEVENT_HIDDEN || e.window.event== SDL_WINDOWEVENT_MINIMIZED)) {
			on_deactivate();
		}
	}
}

Window *Window::CreateWindowInstance(int w, int h, bool disable_effects) {
	if (s_window)
		return s_window;

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
        return NULL;

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, disable_effects ? 0 : 1);

	SDL_Window *window = SDL_CreateWindow("ud1 arkanoid",
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          w, h,
                                          SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    if (glContext == NULL)
    {
        printf("There was an error creating the OpenGL context!\n");
        return NULL;
    }

    const unsigned char *version = glGetString(GL_VERSION);
    if (version == NULL)
    {
        printf("There was an error creating the OpenGL context!\n");
        return NULL;
    }

    SDL_GL_MakeCurrent(window, glContext);

    //MUST make a context AND make it current BEFORE glewInit()!
    glewExperimental = GL_TRUE;
    GLenum glew_status = glewInit();
    if (glew_status != 0)
    {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(glew_status));
        return NULL;
    }

	s_window = new Window;
	s_window->width = w;
	s_window->height = h;
	s_window->disable_effects = disable_effects;
	s_window->window = window;
	s_window->glContext = glContext;
	return s_window;
}

Window::Window() {

}

Window::~Window()
{
	s_window = NULL;
	SDL_GL_DeleteContext(glContext);
    SDL_Quit();
}
