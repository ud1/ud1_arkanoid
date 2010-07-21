#include "bonus_info.h"

#include <fstream>
#include <sstream>

bool BonusInfo::LoadFromFile(const char *filename) {
	info.resize(BONUS_MAX);
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
		std::string cmd;
		if ((iss >> cmd).fail())
			continue;
		if (cmd == "bonus") {
			BonusRect rect;
			int type;
			if (!(iss >> type >> rect.left >> rect.top >> rect.right >> rect.bottom >> 
				rect.red >> rect.green >> rect.blue).fail())
			{
				if (type >= BONUS_MIN && type < BONUS_MAX) {
					info[type] = rect;
				}
			}
		} else if (cmd == "radius") {
			iss >> bonus_rad;
		} else if (cmd == "speed") {
			iss >> speed;
		}
	}

	return true;
}
