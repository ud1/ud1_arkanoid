#pragma once

#include "segment.h"

struct Wall : public Segment {
	float velocity_loss, surf_friction_coef;

	float GetVelocityLoss() const {
		return velocity_loss;
	}

	float GetSurfaceFrictionCoef() const {
		return surf_friction_coef;
	}

	void Collide(){}
};
