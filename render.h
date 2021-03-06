#pragma once

#include "physical_object.h"
#include "ball.h"
#include "chars_info.h"
#include "ball_render_info.h"
#include "bonus.h"

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

inline int round_up_pos_of_2(int val) {
	for (int i = 0; i < 31; ++i) {
		int c = 1 << i;
		if (val <= c)
			return c;
	}
	return 0;
}

struct Game;
struct Bonus;

struct RenderData {
	~RenderData();
	struct Color {
		float r, g, b;
		Color() {}
		Color(float r_, float g_, float b_) {
			r = r_;
			g = g_;
			b = b_;
		}
	};

	void SetGame(Game *game_) {
		game = game_;
	}

	bool Init(bool disable_effects);

	void RenderField();
	void RenderStats();
	void RenderClouds(float dtx, float dty, float alpha);
	void RenderDark(float darkness);

	void PrintTextBig(const std::string &text, Vector pos, Color color) {
		PrintText(text, big_chars, pos, color);
	}

	void PrintTextSmall(const std::string &text, Vector pos, Color color) {
		PrintText(text, small_chars, pos, color);
	}

	CharsInfo big_chars, small_chars;

protected:
	Game *game;
	SDL_Surface *elements = nullptr;
	SDL_Surface *stat_img = nullptr;
	SDL_Surface *background = nullptr;
	SDL_Surface *clouds = nullptr;
	BallRenderData ball_rdata;
	bool disable_effects;

	int stat_tex_w, stat_tex_h;
	GLuint elements_textureID, stat_textureID, background_textureID, clouds_textureID;

	void RenderBackground();
	void RenderObjectsAndPlatform();
	void StartRenderField(const Vector &field_logic_size);
	void StartRenderStat() ;
	void RenderBall(Ball &b);
	void RenderBonus(const Bonus &b);
	void RenderStatsPanel();
	void RenderScore();
	void RenderLevelNumber();

	// pos - top left point
	void PrintText(const std::string &text, const CharsInfo &info, Vector pos, Color color);
};
