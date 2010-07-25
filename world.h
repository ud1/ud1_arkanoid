#pragma once

#include "vector2d.h"
#include "ball.h"
#include "distance.h"
#include "segment.h"
#include "wall.h"
#include "physical_object.h"
#include "player_platform.h"
#include "level.h"
#include "game_brick.h"
#include "bonus.h"
#include "sound_system.h"

#include <vector>
#include <set>
#include <map>
#include <cassert>

struct World {
	float current_time;
	std::set<PhysicalObject *> objects;
	std::vector<Wall> walls;
	std::vector<Ball> balls;
	std::vector<Bonus> bonuses;
	PlayerPlatform player_platform;

	World() {
		Clear();
	}

	void SetSoundSystem(SoundSystem *s, float vel_volume_factor_) {
		snd_system = s;
		vel_volume_factor = vel_volume_factor_;
	}

	void SimulateUntil(float t);

	void AddObj(PhysicalObject *o) {
		objects.insert(o);
	}

	void AddActiveBlock(PhysicalObject *o) {
		active_blocks.insert(o);
	}

	size_t ActiveBlockNumber() const {
		return active_blocks.size();
	}

	size_t BallsNumber() const {
		return balls.size();
	}

	void Clear() {
		current_time = 0.0f;

		for (auto it = objects.begin(); it != objects.end(); ++it)
			delete *it;
		objects.clear();

		walls.clear();
		balls.clear();
		bonuses.clear();
		for_deletion.clear();
		active_blocks.clear();
		new_balls.clear();
	}

	void SetGravity(const Vector &g) {
		for (size_t i = 0; i < balls.size(); ++i) {
			balls[i].gravity = g;
		}
	}

	void NewBall(const Ball &b) {
		new_balls.push_back(b);
	}

	float SpeedBonus(const Vector &field_logic_size) const {
		float sum_bonus = 0.0f;
		for (size_t i = 0; i < balls.size(); ++i) {
			sum_bonus += balls[i].SpeedBonus(field_logic_size);
		}
		return sum_bonus;
	}

	void RemoveObj(PhysicalObject *o) {
		for_deletion.insert(o);
	}

protected:
	void RemoveUnusedObjects() {
		for (auto it = for_deletion.begin(); it != for_deletion.end(); ++it) {
			objects.erase(*it);
			active_blocks.erase(*it);
			delete *it;
		}
		for_deletion.clear();
	}

	float ClampDeltaVel(float v);

	void SimulateDelta(float delta_t);
	bool TryToSimulate(float delta_t);

	void RemoveOutBalls();
	void RemoveOutBonuses();
	void AddNewBalls();

	std::set<PhysicalObject *> for_deletion;
	std::set<PhysicalObject *> active_blocks;
	std::vector<Ball> new_balls;
	SoundSystem *snd_system;
	float vel_volume_factor;
};


