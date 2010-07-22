#include "level.h"

#include <fstream>
#include <sstream>

bool Level::LoadFromFile(const char *filename) {
	bricks.clear();
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
		if (cmd == "brick") {
			brick_info info;
			if (!(iss >> info.type >> info.life >> info.velocity_loss >> info.surf_friction_koef >> info.scale.x >> info.scale.y
				>> info.pos.x >> info.pos.y >> info.angle >> info.points 
				>> info.is_static >> info.bonus).fail())
			{
				info.angle *= (float) M_PI / 180.0f;
				bricks.push_back(info);
			}
		} else if (cmd == "balls") {
			iss >> reserve_balls_number;

		} else if (cmd == "platform_scale") {
			iss >> platform_scale.x >> platform_scale.y;

		} else if (cmd == "short_platform_scale") {
			iss >> short_platform_scale.x >> short_platform_scale.y;

		} else if (cmd == "long_platform_scale") {
			iss >> long_platform_scale.x >> long_platform_scale.y;

		} else if (cmd == "gravity") {
			iss >> gravity.x >> gravity.y;

		} else if (cmd == "gravity_left") {
			iss >> gravity_left.x >> gravity_left.y;

		} else if (cmd == "gravity_right") {
			iss >> gravity_right.x >> gravity_right.y;

		} else if (cmd == "bonus_time") {
			iss >> platform_bonus_time >> gravity_bonus_time >> time_bonus_time;

		} else if (cmd == "velocity_loss_wall") {
			iss >> velocity_loss_wall;

		} else if (cmd == "velocity_loss_platform") {
			iss >> velocity_loss_platform;

		} else if (cmd == "surf_friction_koef_wall") {
			iss >> surf_friction_koef_wall;

		} else if (cmd == "surf_friction_koef_platform") {
			iss >> surf_friction_koef_platform;
		}
	}

	return true;
}
