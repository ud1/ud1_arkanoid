#pragma once

#include "object_prototype.h"
#include "vector2d.h"
#include "segment.h"

struct UnmovableObject {
	virtual ~UnmovableObject(){}
	void SetPrototype(const ObjectPrototype *p) {
		proto = p;
	}

	void SetPosition(const Vector &pos, const Vector &scale_, float angle_);
	void SetPositionCollData(const Vector &pos, const Vector &scale_, float angle_);

	virtual void Collide(){}

	const Vector &GetPosition() const {
		return position;
	}

	const std::vector<Segment> &GetSegments() const {
		return segs;
	}

	std::vector<RenderTriangle> render_triangles;

	// bounding circle radius
	float GetMaxRad() const {
		return max_radius;
	}

protected:
	const ObjectPrototype *proto;
	std::vector<Segment> segs;
	Vector position;
	float angle;
	Vector scale;
	float max_radius;
};
