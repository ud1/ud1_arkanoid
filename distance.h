#pragma once
#include "vector2d.h"
#include "segment.h"

struct DistanceInfo {
	float distance;
	float velocity, rotation_speed;
	Vector closest_point, normal;
};

DistanceInfo Distance(const Vector &point, const Vector &point_velocity, float rotation_speed, const Segment &s);
float DistanceLight(const Vector &point, const Segment &s);

struct Ball;
template <typename PhysObj>
DistanceInfo BallToPhysObjDistance(const Ball &b, const PhysObj &obj);

template <typename PhysObj>
float BallToPhysObjDistanceLight(const Ball &b, const PhysObj &obj);

template <typename PhysObj>
bool BallToPhysObjCollided(const Ball &b, const PhysObj &obj);

template <typename PhysObj>
DistanceInfo BallToPhysObjNewDistance(const Ball &b, const PhysObj &obj, float delta_t);

template <typename PhysObj>
float BallToPhysObjNewDistanceLight(const Ball &b, const PhysObj &obj, float delta_t);

template <typename PhysObj>
bool BallToPhysObjNewCollided(const Ball &b, const PhysObj &obj, float delta_t);

