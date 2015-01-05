#include "chars_info.h"

#include <fstream>
#include <sstream>
#include <cstring>

bool CharsInfo::LoadFromFile(const char *filename) {
	max_width = max_height = 0.0f;
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
		char ch;
		CharRect rect;
		if ((iss >> ch >> rect.left >> rect.top >> rect.right >> rect.bottom).fail())
			continue;
		chars.insert(std::make_pair(ch, rect));

		float w = rect.right - rect.left;
		if (w > max_width)
			max_width = w;

		float h = rect.bottom - rect.top;
		if (h > max_height)
			max_height = h;
	}
	return true;
}

