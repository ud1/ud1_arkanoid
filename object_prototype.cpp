#include "object_prototype.h"
#include <fstream>
#include <sstream>

bool ObjectPrototype::LoadFromFile(const char *filename) {
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
		if (cmd == "seg") {
			SegmentProto seg;
			if (!(iss >> seg.p1.x >> seg.p1.y >> seg.p2.x >> seg.p2.y).fail()) {
				segs.push_back(seg);
			}
		} else if (cmd == "tri") {
			RenderTriangle tri;
			if (!(iss 
				>> tri.vertexes[0].coord.x >> tri.vertexes[0].coord.y 
				>> tri.vertexes[0].tex_coord.x >> tri.vertexes[0].tex_coord.y
				>> tri.vertexes[1].coord.x >> tri.vertexes[1].coord.y 
				>> tri.vertexes[1].tex_coord.x >> tri.vertexes[1].tex_coord.y
				>> tri.vertexes[2].coord.x >> tri.vertexes[2].coord.y 
				>> tri.vertexes[2].tex_coord.x >> tri.vertexes[2].tex_coord.y).fail())
			{
				render_triangles.push_back(tri);
			}
		}
	}
	return true;
}
