#pragma once

#include "polygon_collision_object.h"
#include "render_object.h"

#include <map>
#include <string>


struct ObjectPrototype {
	bool LoadFromFile(const char *filename);

	PlygonColObjPrototype col_obj_proto;
	RenderObjectPrototype render_obj_proto;
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

