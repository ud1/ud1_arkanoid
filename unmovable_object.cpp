#include "unmovable_object.h"

void UnmovableObject::SetScale(const Vector &scale_) {
	scale = scale_;
	if (!proto)
		return;
	size_t render_triangles_size = proto->render_triangles.size();
	render_triangles.resize(render_triangles_size);
	proto_render_triangles.resize(render_triangles_size);

	for (size_t i = 0; i < proto_render_triangles.size(); ++i) {
		RenderTriangle &tri = proto_render_triangles[i];
		const RenderTriangle &prot_tri = proto->render_triangles[i];
		for (size_t j = 0; j < 3; ++j) {
			tri.vertexes[j].coord = prot_tri.vertexes[j].coord * scale;
			tri.vertexes[j].tex_coord = prot_tri.vertexes[j].tex_coord;
		}
	}

	max_radius = 0.0f;
	size_t segs_size = proto->segs.size();
	segs.resize(segs_size);
	proto_segs.resize(segs_size);
	for (size_t i = 0; i < segs_size; ++i) {
		Segment &seg = proto_segs[i];
		const SegmentProto &prot_seg = proto->segs[i];
		Vector p1 = prot_seg.p1 * scale;
		Vector p2 = prot_seg.p2 * scale;
		seg.Init(p1, p2);

		float p1_rad2 = p1.Length2();
		float p2_rad2 = p2.Length2();
		if (p1_rad2 < p2_rad2)
			p1_rad2 = p2_rad2;
		if (max_radius < p1_rad2)
			max_radius = p1_rad2;
	}

	max_radius = sqrt(max_radius);
}

void UnmovableObject::SetPosition(const Vector &pos, float angle_) {
	SetPositionCollData(pos, angle_);

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

void UnmovableObject::SetPositionCollData(const Vector &pos, float angle_) {
	position = pos;
	angle = angle_;
	float sina = sin(angle);
	float cosa = cos(angle);

	size_t segs_size = proto_segs.size();
	for (size_t i = 0; i < segs_size; ++i) {
		Segment &seg = segs[i];
		const Segment &prot_seg = proto_segs[i];
		seg.p1 = pos + prot_seg.p1.Rotate(sina, cosa);
		seg.length = prot_seg.length;
		seg.angle = prot_seg.angle + angle;
		seg.angular_velocity = 0;
		seg.velocity = Vector(0.0f, 0.0f);
	}
}

