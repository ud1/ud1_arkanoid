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

	bool LoadFromFile(const char *filename);
};