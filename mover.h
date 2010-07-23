#pragma once
#include "vector2d.h"

struct Mover {
	Vector GetPositionDelta(float t) {
		return Vector(0.0f, 0.0f);
	}

	float GetAngleDelta(float t) {
		return 0.0f;
	}
};
