#pragma once

#include "vector2d.h"

struct Ball {
	Vector position, velocity, gravity;
	int collide_number;
	float rotation_speed; // omega*rad, >0 ccw
	float rad, friction;

	Ball() {
		collide_number = 0;
		friction = 0.0f;
		rotation_speed = 0.0f;
	}

	void Move(float delta_t) {
		Vector acceleration = gravity - velocity*(velocity.Length()*friction);
		position = position + velocity*delta_t + acceleration*(delta_t*delta_t/2.0f);
		velocity = velocity + acceleration*delta_t;
	}

	// Calculates speed bonus
	float SpeedBonus(const Vector &field_logic_size) const {
		float bottom_speed = sqrt(abs(2*gravity.y*position.y) + velocity.Length2());
		return bottom_speed / field_logic_size.y;
	}

	void Collide() {
		++collide_number;
	}
};
