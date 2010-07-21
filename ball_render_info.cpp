#include "ball_render_info.h"

#include <fstream>
#include <sstream>

bool BallRenderData::LoadFromFile(const char *filename) {
	std::ifstream file(filename);
	if (!file)
		return false;

	const int buf_len = 1024;
	char buf[buf_len];

	while (!file.getline(buf, buf_len).fail()) {
		if (strlen(buf) < 3)
			continue;
		if (buf[0] == '/' && buf[1] == '/')
			continue;
		std::istringstream iss(buf);
		BallRect rect;
		if ((iss >> rect.left >> rect.top >> rect.right >> rect.bottom).fail())
			continue;
		balls.push_back(rect);
	}
	return true;
}
