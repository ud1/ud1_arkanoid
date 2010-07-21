#include "distance.h"
#include "ball.h"
#include "wall.h"
#include "unmovable_object.h"
#include "player_platform.h"
#include <cassert>

DistanceInfo Distance(const Vector &point, const Vector &point_velocity, float rotation_speed, const Segment &s) {
	DistanceInfo res;
	Vector relp = point - s.p1;
	Vector tau = Vector(s.angle);
	Vector p2 = s.p1 + tau*s.length;
	Vector closest_point_vel;
	float proj = relp.Dot(tau);
	if (proj < 0) {
		res.distance = relp.Length();
		res.closest_point = s.p1;
		res.normal = relp;
		res.normal.Normalize();
		closest_point_vel = s.velocity;
		if (tau.Cross(relp) < 0.0f) {
			res.distance = -res.distance;
			res.normal = res.normal*(-1.0f);
		}
	} else if (proj > s.length) {
		res.distance = (point - p2).Length();
		res.closest_point = p2;
		res.normal = point - p2;
		res.normal.Normalize();
		closest_point_vel = s.velocity + tau.RotateHalfPi() * (s.length * s.angular_velocity);
		if (tau.Cross(relp) < 0.0f) {
			res.distance = -res.distance;
			res.normal = res.normal*(-1.0f);
		}
	} else {
		res.distance = tau.Cross(relp);
		res.closest_point = s.p1 + tau*proj;
		res.normal = tau.RotateHalfPi();
		closest_point_vel = s.velocity + tau.RotateHalfPi() * (proj * s.angular_velocity);
	}

	res.velocity = res.normal.Dot(closest_point_vel - point_velocity);
	res.rotation_speed = res.normal.Cross(closest_point_vel - point_velocity) - rotation_speed;
	return res;
}

float DistanceLight(const Vector &point, const Segment &s) {
	Vector relp = point - s.p1;
	Vector tau = Vector(s.angle);
	float proj = relp.Dot(tau);
	float res;
	if (proj < 0) {
		res = relp.Length();
		if (tau.Cross(relp) < 0.0f) {
			res = -res;
		}
	} else if (proj > s.length) {
		Vector p2 = s.p1 + tau*s.length;
		res = (point - p2).Length();
		if (tau.Cross(relp) < 0.0f) {
			res = -res;
		}
	} else {
		res = tau.Cross(relp);
	}
	return res;
}

template<>
DistanceInfo BallToPhysObjDistance(const Ball &b, const Wall &w) {
	DistanceInfo res = Distance(b.position, b.velocity, b.rotation_speed, w);
	res.distance -= b.rad;
	return res;
}

template<>
float BallToPhysObjDistanceLight(const Ball &b, const Wall &w) {
	return DistanceLight(b.position, w) - b.rad;
}

template <>
bool BallToPhysObjCollided(const Ball &b, const Wall &w) {
	return BallToPhysObjDistanceLight(b, w) <= 0.0f;
}

template<>
DistanceInfo BallToPhysObjNewDistance(const Ball &b, const Wall &w, float delta_t) {
	Ball tmp_ball;
	tmp_ball = b;
	tmp_ball.Move(delta_t);
	return BallToPhysObjDistance(tmp_ball, w);
}

template<>
float BallToPhysObjNewDistanceLight(const Ball &b, const Wall &w, float delta_t) {
	Ball tmp_ball;
	tmp_ball = b;
	tmp_ball.Move(delta_t);
	return BallToPhysObjDistanceLight(tmp_ball, w);
}

template<>
bool BallToPhysObjNewCollided(const Ball &b, const Wall &w, float delta_t) {
	Ball tmp_ball;
	tmp_ball = b;
	tmp_ball.Move(delta_t);
	return BallToPhysObjDistanceLight(tmp_ball, w) <= 0.0f;
}

template<>
DistanceInfo BallToPhysObjDistance(const Ball &b, const UnmovableObject &obj) {
	const std::vector<Segment> &segs = obj.GetSegments();
	assert(segs.size() > 0);
	DistanceInfo dst_min_positive, dst_max_negative;
	bool has_positive_values = false;
	bool has_negative_values = false;
	for (size_t i = 0; i < segs.size(); ++i) {
		DistanceInfo dst = Distance(b.position, b.velocity, b.rotation_speed, segs[i]);
		if (dst.distance > 0.0f) {
			if (has_positive_values) {
				if (dst_min_positive.distance > dst.distance)
					dst_min_positive = dst;
			} else {
				dst_min_positive = dst;
			}
			has_positive_values = true;
		} else {
			if (has_negative_values) {
				if (dst_max_negative.distance < dst.distance)
					dst_max_negative = dst;
			} else {
				dst_max_negative = dst;
			}
			has_negative_values = true;
		}
	}

	if (has_positive_values) {
		dst_min_positive.distance -= b.rad;
		return dst_min_positive;
	}
	dst_max_negative.distance -= b.rad;
	return dst_max_negative;
}

template<>
bool BallToPhysObjCollided(const Ball &b, const UnmovableObject &obj) {
	float dist2 = (b.position - obj.GetPosition()).Length2();
	float sum_rad = b.rad + obj.GetMaxRad();
	if (dist2 > sum_rad*sum_rad)
		return false;

	const std::vector<Segment> &segs = obj.GetSegments();
	assert(segs.size() > 0);
	for (size_t i = 0; i < segs.size(); ++i) {
		float dst = DistanceLight(b.position, segs[i]);
		if (dst > b.rad)
			return false;
	}

	return true;
}

template<>
DistanceInfo BallToPhysObjNewDistance(const Ball &b, const UnmovableObject &obj, float delta_t) {
	Ball tmp_ball;
	tmp_ball = b;
	tmp_ball.Move(delta_t);
	return BallToPhysObjDistance(tmp_ball, obj);
}

template<>
bool BallToPhysObjNewCollided(const Ball &b, const UnmovableObject &obj, float delta_t) {
	Ball tmp_ball;
	tmp_ball = b;
	tmp_ball.Move(delta_t);
	return BallToPhysObjCollided(tmp_ball, obj);
}

template<>
DistanceInfo BallToPhysObjDistance(const Ball &b, const PlayerPlatform &obj) {
	return BallToPhysObjDistance(b, (const UnmovableObject &) obj);
}

template<>
bool BallToPhysObjCollided(const Ball &b, const PlayerPlatform &obj) {
	return BallToPhysObjCollided(b, (const UnmovableObject &) obj);
}

template<>
DistanceInfo BallToPhysObjNewDistance(const Ball &b, const PlayerPlatform &obj, float delta_t) {
	Ball tmp_ball;
	tmp_ball.position = b.position;
	tmp_ball.velocity = b.velocity;
	tmp_ball.rad = b.rad;

	static PlayerPlatform tmp_platform;
	tmp_platform.LightCopy(obj);
	tmp_platform.MoveCollData(delta_t);

	return BallToPhysObjDistance(tmp_ball, tmp_platform);
}

template<>
bool BallToPhysObjNewCollided(const Ball &b, const PlayerPlatform &obj, float delta_t) {
	Ball tmp_ball;
	tmp_ball.position = b.position;
	tmp_ball.velocity = b.velocity;
	tmp_ball.rad = b.rad;

	static PlayerPlatform tmp_platform;
	tmp_platform.LightCopy(obj);
	tmp_platform.MoveCollData(delta_t);

	return BallToPhysObjCollided(tmp_ball, tmp_platform);
}
