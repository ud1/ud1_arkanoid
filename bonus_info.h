#pragma once

#include <vector>

enum {
	BONUS_NEW_BALL = 1,
	BONUS_MIN = 2,
	BONUS_LONG_PLATFORM = 2,
	BONUS_SHORT_PLATFORM = 3,
	BONUS_GRAVITY_LEFT = 4,
	BONUS_GRAVITY_RIGHT = 5,
	BONUS_TIMER_SPEED_DOWN = 6,
	BONUS_TIMER_SPEED_UP = 7,
	BONUS_MAX
};

struct BonusInfo {
	struct BonusRect {
		float left, top, right, bottom;
		float red, green, blue;
	};

	bool LoadFromFile(const char *filename);

	float bonus_rad, speed;
	std::vector<BonusRect> info;
};
