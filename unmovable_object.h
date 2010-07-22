#pragma once

#include "object_prototype.h"
#include "vector2d.h"
#include "segment.h"

struct UnmovableObject {
	virtual ~UnmovableObject(){}

	void SetPrototype(const ObjectPrototype *p) {
		proto = p;
	}

	// must be called after SetPrototype
	void SetScale(const Vector &scale);

	// must be called after SetScale
	void SetPosition(const Vector &pos, float angle_);

	// must be called after SetScale
	void SetPositionCollData(const Vector &pos, float angle_);

	virtual void Collide(){}

	const Vector &GetPosition() const {
		return position;
	}

	const std::vector<Segment> &GetSegments() const {
		return segs;
	}

	std::vector<RenderTriangle> render_triangles, proto_render_triangles;

	// bounding circle radius
	float GetMaxRad() const {
		return max_radius;
	}

	float velocity_loss, surf_friction_koef;

protected:
	const ObjectPrototype *proto;
	std::vector<Segment> segs, proto_segs;
	Vector position;
	float angle;
	Vector scale;
	float max_radius;
};
