#include "render_object.h"
#include <cstdlib>

void RenderObject::SetPrototype(const RenderObjectPrototype &proto, Vector scale) {
	size_t render_triangles_size = proto.render_triangles.size();
	render_triangles.resize(render_triangles_size);
	proto_render_triangles.resize(render_triangles_size);

	for (size_t i = 0; i < proto_render_triangles.size(); ++i) {
		RenderTriangle &tri = proto_render_triangles[i];
		const RenderTriangle &prot_tri = proto.render_triangles[i];
		for (size_t j = 0; j < 3; ++j) {
			tri.vertexes[j].coord = prot_tri.vertexes[j].coord * scale;
			tri.vertexes[j].tex_coord = prot_tri.vertexes[j].tex_coord;
		}
	}
}

void RenderObject::SetPosition(Vector pos, float angle) {
	size_t render_triangles_size = proto_render_triangles.size();
	if (render_triangles_size) {
		float sina = sin(angle);
		float cosa = cos(angle);
		for (size_t i = 0; i < render_triangles_size; ++i) {
			RenderTriangle &tri = render_triangles[i];
			const RenderTriangle &prot_tri = proto_render_triangles[i];
			for (size_t j = 0; j < 3; ++j) {
				tri.vertexes[j].coord = pos + prot_tri.vertexes[j].coord.Rotate(sina, cosa);
				tri.vertexes[j].tex_coord = prot_tri.vertexes[j].tex_coord;
			}
		}
	}
}

