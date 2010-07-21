#pragma once

#include "unmovable_object.h"
#include "level.h"

struct World;
struct Game;
struct ObjectPrototype;

struct GameBrick : public UnmovableObject {
	GameBrick(Game *game);
	void Initialize(const Level::brick_info &info, const ObjectPrototype *proto);
	void Collide();
	bool IsStatic() const {
		return is_static;
	}

protected:
	Game *game;
	int life, game_points, bonus;
	bool is_static;
};
