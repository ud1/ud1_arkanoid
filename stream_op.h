#pragma once

#include "vector2d.h"

#include <istream>

std::istream &operator >> (std::istream &s, Vector &v) {
	s >> v.x >> v.y;
	return s;
}