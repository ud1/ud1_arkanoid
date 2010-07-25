#pragma once

struct FormConfig {
	FormConfig();

	struct {
		int width, height;
	} window;

	struct {
		float width, height, x, y;
		float logic_width, logic_height;
	} field;

	struct {
		float left, top, right, bottom;
	} stat_panel;

	struct {
		int left, top, right, bottom;
	} score_position;

	struct {
		int left, top, right, bottom;
	} level_position;

	float mouse_smoothing;
	float background_volume;
	float velocity_volume_factor;

	bool LoadFromFile(const char *filename);
};