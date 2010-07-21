#pragma once

#include "ball.h"

struct Game;
struct BonusInfo;

struct Bonus : public Ball {
	Bonus(const Vector &pos, const BonusInfo &info, int type, Game *game_);
	void Move(float delta_t);
	void Collide();

	int bonus_type;
	Game *game;
};
