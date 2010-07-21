#pragma once

#include "vector2d.h"
#include "ball.h"
#include "distance.h"
#include "segment.h"
#include "wall.h"
#include "unmovable_object.h"
#include "player_platform.h"
#include "level.h"
#include "game_brick.h"
#include "bonus.h"

#include <vector>
#include <set>
#include <map>
#include <cassert>

struct World {
	float current_time;
	std::set<UnmovableObject *> unmvbl_objs;
	std::vector<Wall> walls;
	std::vector<Ball> balls;
	std::vector<Bonus> bonuses;
	PlayerPlatform player_platform;

	World() {
		Clear();
	}

	void SimulateUntil(float t);

	void AddObj(UnmovableObject *o) {
		unmvbl_objs.insert(o);
	}

	void AddActiveBlock(UnmovableObject *o) {
		active_blocks.insert(o);
	}

	size_t ActiveBlockNumber() const {
		return active_blocks.size();
	}

	size_t BallsNumber() const {
		return balls.size();
	}

	void RemoveObj(UnmovableObject *o) {
		for_deletion.insert(o);
	}

	void RemoveUnusedObjects() {
		for (auto it = for_deletion.begin(); it != for_deletion.end(); ++it) {
			unmvbl_objs.erase(*it);
			active_blocks.erase(*it);
			delete *it;
		}
		for_deletion.clear();
	}

	void Clear() {
		current_time = 0.0f;

		for (auto it = unmvbl_objs.begin(); it != unmvbl_objs.end(); ++it)
			delete *it;
		unmvbl_objs.clear();

		walls.clear();
		balls.clear();
	}

	float SpeedBonus(const Vector &field_logic_size) const {
		float sum_bonus = 0.0f;
		for (size_t i = 0; i < balls.size(); ++i) {
			sum_bonus += balls[i].SpeedBonus(field_logic_size);
		}
		return sum_bonus;
	}

	void SetGravity(const Vector &g) {
		for (size_t i = 0; i < balls.size(); ++i) {
			balls[i].gravity = g;
		}
	}

	void NewBall(const Ball &b) {
		new_balls.push_back(b);
	}

protected:
	void SimulateDelta(float delta_t) {
		while (delta_t > 0.0f) {
			float dt = delta_t;
			while (!TryToSimulate(dt))
				dt /= 2;

			delta_t -= dt;
		}
	}

	bool TryToSimulate(float delta_t);

	void RemoveOutBalls();
	void RemoveOutBonuses();
	void AddNewBalls();

	std::set<UnmovableObject *> for_deletion;
	std::set<UnmovableObject *> active_blocks;
	std::vector<Ball> new_balls;
};


