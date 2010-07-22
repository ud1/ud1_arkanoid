#pragma once

struct Score {
	Score() {
		Reset();
	}

	void Reset() {
		score = 0.0f;
		speed_bonus = 1.0f;
		platform_bonus = 1.0f;
		gravity_bonus = 1.0f;
		time_bonus = 1.0f;
	}

	void Add(int s) {
		score += s * speed_bonus * platform_bonus * gravity_bonus * time_bonus;
	}

	int Get() const {
		return (int) score;
	}

	float speed_bonus, platform_bonus, gravity_bonus, time_bonus;

protected:
	float score;
};
