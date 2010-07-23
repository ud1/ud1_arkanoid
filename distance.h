#pragma once
#include "vector2d.h"

struct DistanceInfo {
	float distance;
	float velocity, rotation_speed;
	Vector closest_point, normal;
};

struct Ball;

// returns a distance between a ball and an object
template <typename PhysObj>
DistanceInfo BallToPhysObjDistance(const Ball &b, const PhysObj &obj);

// check if a ball collided with an object
// it's faster than the BallToPhysObjDistance()
template <typename PhysObj>
bool BallToPhysObjCollided(const Ball &b, const PhysObj &obj);
