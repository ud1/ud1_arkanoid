#pragma once

#include "vector2d.h"
#include <vector>
#include <string>

struct Level {
	Level();
	struct brick_info {
		Vector scale, pos;
		float angle;
		float velocity_loss, surf_friction_koef;
		int life, points, bonus;
		bool is_static;
		std::string type;
	};

	std::vector<brick_info> bricks;
	int reserve_balls_number;
	Vector platform_scale;
	Vector short_platform_scale;
	Vector long_platform_scale;

	Vector gravity;
	Vector gravity_left;
	Vector gravity_right;

	double platform_bonus_time, gravity_bonus_time, time_bonus_time;

	float velocity_loss_wall, velocity_loss_platform;
	float surf_friction_koef_wall, surf_friction_koef_platform;

	bool LoadFromFile(const char *filename);
};
