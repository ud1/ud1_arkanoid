#pragma once
#include "segment.h"

#include <vector>

struct SegmentProto {
	Vector p1, p2;
};

struct PlygonColObjPrototype {
	std::vector<SegmentProto> segs;
};

struct PolygonCollisionObject {
	// must be the first function called
	void SetPrototype(const PlygonColObjPrototype &proto, Vector scale);

	void SetPosition(Vector pos, float angle, Vector velocity, float angular_vel);

	const std::vector<Segment> &GetSegments() const {
		return segs;
	}

	float GetBoundingRad() const {
		return max_radius;
	}
	
	float velocity_loss, surf_friction_coef;

protected:
	std::vector<Segment> segs, proto_segs;
	float max_radius;
};
