#pragma once

#include "segment.h"

struct Wall : public Segment {
	float velocity_loss, surf_friction_koef;

	virtual void Collide(){}
};
