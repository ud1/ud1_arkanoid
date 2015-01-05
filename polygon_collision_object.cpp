#include "polygon_collision_object.h"
#include <cstdlib>

void PolygonCollisionObject::SetPrototype(const PlygonColObjPrototype &proto, Vector scale) {
	max_radius = 0.0f;
	size_t segs_size = proto.segs.size();
	segs.resize(segs_size);
	proto_segs.resize(segs_size);
	for (size_t i = 0; i < segs_size; ++i) {
		Segment &seg = proto_segs[i];
		const SegmentProto &prot_seg = proto.segs[i];
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

void PolygonCollisionObject::SetPosition(Vector pos, float angle, Vector velocity, float angular_vel) {
	float sina = sin(angle);
	float cosa = cos(angle);

	size_t segs_size = proto_segs.size();
	for (size_t i = 0; i < segs_size; ++i) {
		Segment &seg = segs[i];
		const Segment &prot_seg = proto_segs[i];
		seg.p1 = pos + prot_seg.p1.Rotate(sina, cosa);
		seg.length = prot_seg.length;
		seg.angle = prot_seg.angle + angle;
		seg.angular_velocity = angular_vel;
		seg.velocity = velocity + prot_seg.p1.RotateHalfPi()*angular_vel;
	}
}

