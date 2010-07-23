#pragma once
#include "vector2d.h"

#include <vector>

struct RenderVertex {
	Vector coord;
	Vector tex_coord;
};

struct RenderTriangle {
	RenderVertex vertexes[3];
};

struct RenderObjectPrototype {
	std::vector<RenderTriangle> render_triangles;
};

struct RenderObject {
	// must be the first function called
	void SetPrototype(const RenderObjectPrototype &proto, Vector scale);
	void SetPosition(Vector pos, float angle);

	const std::vector<RenderTriangle> &GetTriangles() const {
		return render_triangles;
	}

protected:
	std::vector<RenderTriangle> render_triangles, proto_render_triangles;
};
