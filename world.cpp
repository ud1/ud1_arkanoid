#include "world.h"

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

#ifdef abs
#undef abs
#endif

const float surf_friction_koef = 0.3f;
const float ball_inertia_moment = 0.4f;

template<typename PhysObj>
bool CollideBallToPhysObj(Ball &ball, PhysObj &obj, float delta_t) {
	DistanceInfo dst_new = BallToPhysObjNewDistance(ball, obj, delta_t);
	if (dst_new.distance < 0.0f) {
		// Collision detected
		DistanceInfo dst = BallToPhysObjDistance(ball, obj);
		float time_to_collide = dst.distance / dst.velocity;
		if (time_to_collide > delta_t)
			time_to_collide = delta_t;
		if (time_to_collide < 0.0f)
			time_to_collide = 0.0f;
		ball.Move(time_to_collide);
		ball.velocity = ball.velocity + dst.normal * (2*dst.velocity);
		float delta_rot_speed = std::min(std::abs(2*dst.velocity*surf_friction_koef/ball_inertia_moment), std::abs(dst.rotation_speed));
		if (dst.rotation_speed < 0.0f)
			delta_rot_speed = -delta_rot_speed;
		ball.rotation_speed += delta_rot_speed;
		ball.velocity = ball.velocity + dst.normal.RotateHalfPi()*(delta_rot_speed*ball_inertia_moment);
		ball.Move(delta_t - time_to_collide);

		dst = BallToPhysObjDistance(ball, obj);
		if (dst.distance < 0.0f) {
			//	inside obj, move to surface
			ball.position = dst.closest_point + dst.normal*ball.rad;
		}

		ball.Collide();
		obj.Collide();
		return true;
	}
	return false;
}

bool CollideBallToBall(Ball &b1, Ball &b2, float delta_t) {
	DistanceInfo dst_new = BallToPhysObjNewDistance(b1, b2, delta_t);
	if (dst_new.distance < 0.0f) {
		// Collision detected
		DistanceInfo dst = BallToPhysObjDistance(b1, b2);
		float time_to_collide = dst.distance / dst.velocity;
		if (time_to_collide > delta_t)
			time_to_collide = delta_t;
		if (time_to_collide < 0.0f)
			time_to_collide = 0.0f;
		b1.Move(time_to_collide);
		b2.Move(time_to_collide);
		b1.velocity = b1.velocity + dst.normal * (dst.velocity);
		b2.velocity = b2.velocity - dst.normal * (dst.velocity);
		float delta_rot_speed = std::min(std::abs(dst.velocity*surf_friction_koef/ball_inertia_moment), std::abs(dst.rotation_speed/2.0f));
		if (dst.rotation_speed < 0.0f)
			delta_rot_speed = -delta_rot_speed;
		b1.rotation_speed += delta_rot_speed;
		b2.rotation_speed -= delta_rot_speed;
		Vector tangent = dst.normal.RotateHalfPi();
		b1.velocity = b1.velocity + tangent*(delta_rot_speed*ball_inertia_moment);
		b2.velocity = b2.velocity - tangent*(delta_rot_speed*ball_inertia_moment);
		b1.Move(delta_t - time_to_collide);
		b2.Move(delta_t - time_to_collide);

		dst = BallToPhysObjDistance(b1, b2);
		if (dst.distance < 0.0f) {
			//	inside obj, move to surface
			b1.position = dst.closest_point + dst.normal*b1.rad;
		}

		b1.Collide();
		b2.Collide();

		b1.pos_updated = true;
		b2.pos_updated = true;

		return true;
	}
	return false;
}

const float min_delta_t = 1.0f/2500.0f;
const float max_delta_t = 1.0f/500.0f;

void World::SimulateUntil(float t) {
	if (t <= current_time)
		return;
	player_platform.CalculateVelocity(t - current_time);
	size_t cnt = (size_t) ((t - current_time) / max_delta_t);
	for (size_t i = 0; i < cnt; ++i) {
		SimulateDelta(max_delta_t);
		current_time += max_delta_t;
	}
	SimulateDelta(t - current_time);
	current_time = t;

	RemoveOutBalls();
	RemoveOutBonuses();
	RemoveUnusedObjects();
	AddNewBalls();
}

bool World::TryToSimulate(float delta_t) {
	size_t balls_size = balls.size();
	// Check for collisions
	if (delta_t > min_delta_t) {
		for (size_t i = 0; i < balls_size; ++i) {
			for (size_t j = 0; j < walls.size(); ++j) {
				if (BallToPhysObjNewCollided(balls[i], walls[j], delta_t))
					return false;
			}
		}

		for (size_t i = 0; i < balls_size; ++i) {
			for (auto it = unmvbl_objs.begin(); it != unmvbl_objs.end(); ++it) {
				if (BallToPhysObjNewCollided(balls[i], **it, delta_t))
					return false;
			}
		}

		for (size_t i = 0; i < balls_size; ++i) {
			if (BallToPhysObjNewCollided(balls[i], player_platform, delta_t))
				return false;
		}

		for (size_t i = 0; i < balls_size; ++i) {
			for (size_t j = i+1; j < balls_size; ++j) {
				if (BallToPhysObjNewCollided(balls[i], balls[j], delta_t))
					return false;
			}
		}

		for (size_t i = 0; i < bonuses.size(); ++i) {
			if (BallToPhysObjNewCollided((const Ball &)bonuses[i], player_platform, delta_t))
				return false;
		}
	}

	// Update positions
	for (size_t i = 0; i < balls_size; ++i) {
		balls[i].pos_updated = false;
	}

	for (size_t i = 0; i < balls_size; ++i) {
		Ball &ball = balls[i];

		for (size_t j = 0; j < walls.size(); ++j) {
			if (BallToPhysObjNewCollided(ball, walls[j], delta_t))
				ball.pos_updated |= CollideBallToPhysObj(ball, walls[j], delta_t);
		}

		for (auto it = unmvbl_objs.begin(); it != unmvbl_objs.end(); ++it) {
			if (BallToPhysObjNewCollided(ball, **it, delta_t))
				ball.pos_updated |= CollideBallToPhysObj(ball, **it, delta_t);
		}

		for (size_t j = i+1; j < balls_size; ++j) {
			if (BallToPhysObjNewCollided(ball, balls[j], delta_t))
				CollideBallToBall(ball, balls[j], delta_t);
		}

		if (BallToPhysObjNewCollided(ball, player_platform, delta_t))
			ball.pos_updated |= CollideBallToPhysObj(ball, player_platform, delta_t);


		if (!ball.pos_updated) {
			ball.Move(delta_t);
		}
	}

	for (auto it = bonuses.begin(); it != bonuses.end();) {
		Bonus &bonus = *it;
		if (BallToPhysObjNewCollided((const Ball &)bonus, player_platform, delta_t)) {
			bonus.Collide();
			it = bonuses.erase(it);
		} else {
			bonus.Move(delta_t);
			++it;
		}
	}

	player_platform.Move(delta_t);

	return true;
}

void World::RemoveOutBalls() {
	for (auto it = balls.begin(); it != balls.end();) {
		Ball &b = *it;
		if (b.position.y < -b.rad) {
			it = balls.erase(it);
		} else ++it;
	}
}

void World::RemoveOutBonuses() {
	for (auto it = bonuses.begin(); it != bonuses.end();) {
		Bonus &b = *it;
		if (b.position.y < -b.rad) {
			it = bonuses.erase(it);
		} else ++it;
	}
}

void World::AddNewBalls() {
	balls.insert(balls.end(), new_balls.begin(), new_balls.end());
	new_balls.clear();
}
