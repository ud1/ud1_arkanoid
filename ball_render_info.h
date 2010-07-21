#pragma once

#include <vector>

struct BallRenderData {
	struct BallRect {
		float left, top, right, bottom;
	};

	std::vector <BallRect> balls;

	bool LoadFromFile(const char *filename);
};
