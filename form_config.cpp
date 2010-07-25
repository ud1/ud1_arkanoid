#include "form_config.h"

#include <fstream>
#include <sstream>

FormConfig::FormConfig() {
	window.width = 800;
	window.height = 600;

	field.width = 600;
	field.height = 600;
	field.x = 0;
	field.y = 0;
	field.logic_width = 10.0f;
	field.logic_height = 10.0f;

	stat_panel.left = 600;
	stat_panel.top = 0;
	stat_panel.right = 800;
	stat_panel.bottom = 600;

	score_position.left = 612;
	score_position.top = 74;
	score_position.right = 787;
	score_position.bottom = 109;

	mouse_smoothing = 0.07f;

	background_volume = 0.8f;
	velocity_volume_factor = 0.05f;
}

bool FormConfig::LoadFromFile(const char *filename) {
	std::ifstream file(filename);
	if (!file)
		return false;

	const int buf_len = 1024;
	char buf[buf_len];

	while (!file.getline(buf, buf_len).fail()) {
		if (strlen(buf) < 3)
			continue;
		if (buf[0] == '/' && buf[1] == '/')
			continue;
		std::istringstream iss(buf);
		std::string cmd;
		if ((iss >> cmd).fail())
			continue;
		if (cmd == "window") {
			if ((iss >> window.width >> window.height).fail())
				return false;
		} else if (cmd == "field") {
			if ((iss >> field.width >> field.height >> field.x >> field.y >> field.logic_width >> field.logic_height).fail())
				return false;
		} else if (cmd == "stat_panel") {
			if ((iss >> stat_panel.left >> stat_panel.top >> stat_panel.right >> stat_panel.bottom).fail())
				return false;
		} else if (cmd == "score_position") {
			if ((iss >> score_position.left >> score_position.top >> score_position.right >> score_position.bottom).fail())
				return false;
		} else if (cmd == "level_position") {
			if ((iss >> level_position.left >> level_position.top >> level_position.right >> level_position.bottom).fail())
				return false;
		} else if (cmd == "mouse_smoothing") {
			iss >> mouse_smoothing;

		} else if (cmd == "background_volume") {
			iss >> background_volume;

		} else if (cmd == "velocity_volume_factor") {
			iss >> velocity_volume_factor;
		}
	}
	return true;
}
