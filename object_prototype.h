#pragma once

#include "vector2d.h"
#include "segment.h"
#include <map>
#include <vector>
#include <string>

struct SegmentProto {
	Vector p1, p2;
};

struct RenderVertex {
	Vector coord;
	Vector tex_coord;
};

struct RenderTriangle {
	RenderVertex vertexes[3];
};

struct ObjectPrototype {
	bool LoadFromFile(const char *filename);

	ObjectPrototype &operator = (const ObjectPrototype &p) {
		segs = p.segs;
		render_triangles = p.render_triangles;
		return *this;
	}
	std::vector<SegmentProto> segs;
	std::vector<RenderTriangle> render_triangles;
};

struct ObjectPrototypeManager {
	~ObjectPrototypeManager() {
		for (auto it = prototypes.begin(); it != prototypes.end(); ++it)
			delete it->second;
	}

	const ObjectPrototype *Get(const std::string &type) {
		if (prototypes.count(type))
			return prototypes[type];

		ObjectPrototype *proto = new ObjectPrototype;
		if (!proto->LoadFromFile((std::string("conf/") + type + ".txt").c_str())) {
			delete proto;
			return NULL;
		}

		prototypes.insert(std::make_pair(type, proto));
		return proto;
	}

protected:
	std::map<std::string, ObjectPrototype *> prototypes;
};

