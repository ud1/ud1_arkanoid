#pragma once

#include "vector2d.h"
#include <cmath>

struct Segment {
	Segment() {
		p1 = Vector(0.0f, 0.0f);
		velocity = Vector(0.0f, 0.0f);
		length = 1.0f;
		angle = 0.0f;
		angular_velocity = 0.0f;
	}

	void Init(const Vector &p1_, const Vector &p2_) {
		p1 = p1_;
		Vector tau = p2_ - p1_;
		length = tau.Length();
		angle = std::atan2(tau.y, tau.x);
		angular_velocity = 0.0f;
		velocity = Vector(0.0f, 0.0f);

	}
	Vector p1, velocity;
	float length, angle, angular_velocity;
};
