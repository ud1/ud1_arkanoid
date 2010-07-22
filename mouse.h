#pragma once
#include "vector2d.h"
#include <cmath>

struct Mouse {
	Mouse() {
		abs_x = abs_y = 0.0f;
		discr_abs_x = discr_abs_y = 0.0f;
		tau = 0.07f;
	}

	void SetDeltaPos(float dx, float dy) {
		discr_abs_x += dx;
		discr_abs_y += dy;
	}

	void Update(float delta_t) {
		float exp_dt = exp(-delta_t/tau);

		float dx = discr_abs_x - abs_x;
		abs_x = discr_abs_x - dx*exp_dt;

		float dy = discr_abs_y - abs_y;
		abs_y = discr_abs_y - dy*exp_dt;
	}

	void Set(Vector coords) {
		float dx = coords.x - abs_x;
		float dy = coords.y - abs_y;
		abs_x = coords.x;
		abs_y = coords.y;
		discr_abs_x += dx;
		discr_abs_y += dy;
	}

	void AbsSet(Vector coords) {
		abs_x = coords.x;
		abs_y = coords.y;
		discr_abs_x = coords.x;
		discr_abs_y = coords.y;
	}

	Vector Get() const {
		return Vector(abs_x, abs_y);
	}

	void SetTau(float seconds) {
		tau = seconds;
	}
protected:
	float discr_abs_x, discr_abs_y;
	float abs_x, abs_y;
	float tau;
};