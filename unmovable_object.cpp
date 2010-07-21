#include "unmovable_object.h"

void UnmovableObject::SetPosition(const Vector &pos, const Vector &scale_, float angle_) {
	SetPositionCollData(pos, scale_, angle_);
	render_triangles.resize(proto->render_triangles.size());

	for (size_t i = 0; i < render_triangles.size(); ++i) {
		RenderTriangle &tri = render_triangles[i];
		const RenderTriangle &prot_tri = proto->render_triangles[i];
		for (size_t j = 0; j < 3; ++j) {
			tri.vertexes[j].coord = pos + (prot_tri.vertexes[j].coord * scale).Rotate(angle);
			tri.vertexes[j].tex_coord = prot_tri.vertexes[j].tex_coord;
		}
	}
}

void UnmovableObject::SetPositionCollData(const Vector &pos, const Vector &scale_, float angle_) {
	position = pos;
	angle = angle_;
	scale = scale_;
	max_radius = 0.0f;

	segs.resize(proto->segs.size());
	for (size_t i = 0; i < segs.size(); ++i) {
		Segment &seg = segs[i];
		const SegmentProto &prot_seg = proto->segs[i];
		Vector p1 = pos + (prot_seg.p1 * scale).Rotate(angle);
		Vector p2 = pos + (prot_seg.p2 * scale).Rotate(angle);
		seg.Init(p1, p2);

		float p1_rad2 = (p1 - pos).Length2();
		float p2_rad2 = (p2 - pos).Length2();
		if (p1_rad2 < p2_rad2)
			p1_rad2 = p2_rad2;
		if (max_radius < p1_rad2)
			max_radius = p1_rad2;
	}

	max_radius = sqrt(max_radius);
}

