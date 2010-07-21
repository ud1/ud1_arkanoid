#pragma once

#include "segment.h"

struct Wall : public Segment {
	virtual void Collide(){}
};
