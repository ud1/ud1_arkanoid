#include "level.h"
#include "stream_op.h"

#include <fstream>
#include <sstream>
#include <cstring>

Level::Level() {
	reserve_balls_number = 5;

	platform_scale = Vector(1.5f, 0.3f);
	short_platform_scale = Vector(0.75f, 0.2f);
	long_platform_scale = Vector(3.0f, 0.5f);

	gravity = Vector(0.0f, -7.0f);
	gravity_left = Vector(-3.0f, -7.0f);
	gravity_right = Vector(3.0f, 7.0f);

	clouds_speed = Vector(0.1f, 0.1f);
	clouds_alpha = 0.5f;

	platform_bonus_time = gravity_bonus_time = time_bonus_time = 2.0;

	velocity_loss_wall = 0.05f;
	velocity_loss_platform = 0.2f;

	surf_friction_coef_wall = 0.05f;
	surf_friction_coef_platform = 0.15f;
}

bool Level::LoadFromFile(const char *filename) {
	bricks.clear();
	balls.clear();
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
			if (!(iss >> info.type >> info.life >> info.velocity_loss >> info.surf_friction_coef >> info.scale
				>> info.pos >> info.angle >> info.points
				>> info.is_static >> info.bonus).fail())
			{
				info.angle *= (float) M_PI / 180.0f;
				bricks.push_back(info);
			}
		} else if (cmd == "pos_lin") {
			if (bricks.size()) {
				Level::brick_info &info = bricks[bricks.size() - 1];
				Mover::pos_lin p_lin;
				if (!(iss >> p_lin).fail()) {
					info.mover.pos_lin_funcs.push_back(p_lin);
				}
			}
		} else if (cmd == "pos_sine") {
			if (bricks.size()) {
				Level::brick_info &info = bricks[bricks.size() - 1];
				Mover::pos_sine p_sine;
				if (!(iss >> p_sine).fail()) {
					info.mover.pos_sine_funcs.push_back(p_sine);
				}
			}
		} else if (cmd == "angle_lin") {
			if (bricks.size()) {
				Level::brick_info &info = bricks[bricks.size() - 1];
				Mover::angle_lin a_lin;
				if (!(iss >> a_lin).fail()) {
					info.mover.angle_lin_funcs.push_back(a_lin);
				}
			}
		} else if (cmd == "angle_sine") {
			if (bricks.size()) {
				Level::brick_info &info = bricks[bricks.size() - 1];
				Mover::angle_sine a_sine;
				if (!(iss >> a_sine).fail()) {
					info.mover.angle_sine_funcs.push_back(a_sine);
				}
			}
		} else if (cmd == "ball") {
			ball_info info;
			if (!(iss >> info.position >> info.velocity).fail()) {
				balls.push_back(info);
			}
		} else if (cmd == "balls") {
			iss >> reserve_balls_number;

		} else if (cmd == "platform_scale") {
			iss >> platform_scale;

		} else if (cmd == "short_platform_scale") {
			iss >> short_platform_scale;

		} else if (cmd == "long_platform_scale") {
			iss >> long_platform_scale;

		} else if (cmd == "gravity") {
			iss >> gravity;

		} else if (cmd == "gravity_left") {
			iss >> gravity_left;

		} else if (cmd == "gravity_right") {
			iss >> gravity_right;

		} else if (cmd == "bonus_time") {
			iss >> platform_bonus_time >> gravity_bonus_time >> time_bonus_time;

		} else if (cmd == "velocity_loss_wall") {
			iss >> velocity_loss_wall;

		} else if (cmd == "velocity_loss_platform") {
			iss >> velocity_loss_platform;

		} else if (cmd == "surf_friction_coef_wall") {
			iss >> surf_friction_coef_wall;

		} else if (cmd == "surf_friction_coef_platform") {
			iss >> surf_friction_coef_platform;

		} else if (cmd == "clouds") {
			iss >> clouds_alpha >> clouds_speed;
		}
	}

	return true;
}
