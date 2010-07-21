#pragma once

#include <map>

struct CharsInfo {
	struct CharRect {
		float left, top, right, bottom;
	};

	const CharRect *Get(char ch) const {
		auto it = chars.find(ch);
		if (it == chars.end())
			return NULL;
		return &it->second;
	}

	float GetWidth() const {
		return max_width;
	}

	float GetHeight() const {
		return max_height;
	}

	bool LoadFromFile(const char *filename);

protected:
	std::map<char, CharRect> chars;
	float max_width, max_height;
};
