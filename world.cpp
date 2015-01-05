#include "world.h"
#include "sounds.h"

template<typename T>
T Min(T a, T b) { return a < b ? a : b; }

const float ball_inertia_moment = 0.4f;
const float ball_to_ball_surf_friction_coef = 0.4f;

template<typename PhysObj>
float CollideBallToPhysObj(Ball &ball, PhysObj &obj, float delta_t) {
	DistanceInfo dst = BallToPhysObjDistance(ball, obj);
	float time_to_collide = dst.distance / dst.velocity;
	if (time_to_collide > delta_t)
		time_to_collide = delta_t;
	if (time_to_collide < 0.0f)
		time_to_collide = 0.0f;
	ball.Move(time_to_collide);
	float vel_coef = 2.0f - obj.GetVelocityLoss();
	float res = std::abs(vel_coef*dst.velocity);
	ball.velocity = ball.velocity + dst.normal * (vel_coef*dst.velocity);
	float delta_rot_speed = Min(std::abs(vel_coef*dst.velocity*obj.GetSurfaceFrictionCoef()/ball_inertia_moment), std::abs(dst.rotation_speed/(1.0f+ball_inertia_moment)));
	if (dst.rotation_speed < 0.0f)
		delta_rot_speed = -delta_rot_speed;
	ball.rotation_speed += delta_rot_speed;
	ball.velocity = ball.velocity - dst.normal.RotateHalfPi()*(delta_rot_speed*ball_inertia_moment);
	ball.Move(delta_t - time_to_collide);

	dst = BallToPhysObjDistance(ball, obj);
	if (dst.distance < 0.0f) {
		//	inside obj, move to surface
		ball.position = dst.closest_point + dst.normal*ball.rad;
	}

	ball.Collide();
	obj.Collide();
	ball.pos_updated = true;
	return res;
}

float CollideBallToBall(Ball &b1, Ball &b2, float delta_t) {
	DistanceInfo dst = BallToPhysObjDistance(b1, b2);
	float time_to_collide = dst.distance / dst.velocity;
	if (time_to_collide > delta_t)
		time_to_collide = delta_t;
	if (time_to_collide < 0.0f)
		time_to_collide = 0.0f;
	b1.Move(time_to_collide);
	b2.Move(time_to_collide);
	float res = std::abs(dst.velocity);
	b1.velocity = b1.velocity + dst.normal * (dst.velocity);
	b2.velocity = b2.velocity - dst.normal * (dst.velocity);
	float delta_rot_speed = std::min(std::abs(dst.velocity*ball_to_ball_surf_friction_coef/ball_inertia_moment), std::abs(dst.rotation_speed/(2.0f*(1.0f+ball_inertia_moment))));
	if (dst.rotation_speed < 0.0f)
		delta_rot_speed = -delta_rot_speed;
	b1.rotation_speed += delta_rot_speed;
	b2.rotation_speed += delta_rot_speed;
	Vector tangent = dst.normal.RotateHalfPi();
	b1.velocity = b1.velocity - tangent*(delta_rot_speed*ball_inertia_moment);
	b2.velocity = b2.velocity + tangent*(delta_rot_speed*ball_inertia_moment);
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
	return res;
}

const float min_delta_t = 1.0f/1500.0f;
const float max_delta_t = 1.0f/500.0f;

void World::SimulateUntil(float t) {
	if (t <= current_time)
		return;

	player_platform.CalculateVelocity(t - current_time);
	for (auto it = objects.begin(); it != objects.end(); ++it)
		(*it)->CalculateVelocity(t, t - current_time);

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

void World::SimulateDelta(float delta_t) {
	while (delta_t > 0.0f) {
		float dt = delta_t;
		while (!TryToSimulate(dt))
			dt /= 2;

		delta_t -= dt;
	}
}

template <typename T>
void TestMoveObj(T &t, float delta_t) {
	if (t.pos_updated)
		return;
	t.TestMove(delta_t);
	t.pos_updated = true;
}

template <typename T>
void MoveObj(T &t, float delta_t) {
	if (t.pos_updated) {
		t.ApplyMove();
		return;
	}
	t.Move(delta_t);
	t.pos_updated = true;
}

bool World::TryToSimulate(float delta_t) {
	size_t balls_size = balls.size();

	player_platform.pos_updated = false;
	for (size_t i = 0; i < balls_size; ++i) {
		balls[i].pos_updated = false;
	}
	for (auto it = objects.begin(); it != objects.end(); ++it)
		(*it)->pos_updated = false;

	// Check for collisions
	if (delta_t > min_delta_t) {
		for (size_t i = 0; i < balls_size; ++i) {
			Ball tmp_ball;
			tmp_ball = balls[i];
			tmp_ball.Move(delta_t);

			// Check collision to walls
			for (size_t j = 0; j < walls.size(); ++j) {
				if (BallToPhysObjCollided(tmp_ball, walls[j]))
					return false;
			}

			// Check collision to objects
			for (auto it = objects.begin(); it != objects.end(); ++it) {
				PhysicalObject *obj = *it;
				TestMoveObj(*obj, delta_t);
				if (BallToPhysObjCollided(tmp_ball, *obj))
					return false;
			}

			// Check collision to player platform
			TestMoveObj(player_platform, delta_t);
			if (BallToPhysObjCollided(tmp_ball, player_platform))
				return false;

			// Check collision to other balls
			for (size_t j = i+1; j < balls_size; ++j) {
				Ball tmp_ball2;
				tmp_ball2 = balls[j];
				tmp_ball2.Move(delta_t);
				if (BallToPhysObjCollided(tmp_ball, tmp_ball2))
					return false;
			}
		}

		// Check collision the platform to bonuses
		for (size_t i = 0; i < bonuses.size(); ++i) {
			TestMoveObj(player_platform, delta_t);
			Bonus tmp_bounus = bonuses[i];
			tmp_bounus.Move(delta_t);
			if (BallToPhysObjCollided(tmp_bounus, player_platform))
				return false;
		}
	}

	// Update positions
	for (size_t i = 0; i < balls_size; ++i) {
		Ball &ball = balls[i];
		Ball tmp_ball;
		tmp_ball = ball;
		tmp_ball.Move(delta_t);

		// Check collision to walls
		for (size_t j = 0; j < walls.size(); ++j) {
			if (BallToPhysObjCollided(tmp_ball, walls[j])) {
				float dv = CollideBallToPhysObj(ball, walls[j], delta_t);
				snd_system->Play(BALL_TO_WALL_SOUND, ClampDeltaVel(dv));
			}
		}

		// Check collision to objects
		for (auto it = objects.begin(); it != objects.end(); ++it) {
			PhysicalObject *obj = *it;
			TestMoveObj(*obj, delta_t);
			if (BallToPhysObjCollided(tmp_ball, *obj)) {
				float dv = CollideBallToPhysObj(ball, *obj, delta_t);
				snd_system->Play(BALL_TO_BRICK_SOUND, ClampDeltaVel(dv));
			}
		}

		// Check collision to player platform
		TestMoveObj(player_platform, delta_t);
		if (BallToPhysObjCollided(tmp_ball, player_platform)) {
			float dv = CollideBallToPhysObj(ball, player_platform, delta_t);
			snd_system->Play(BALL_TO_PLATFORM_SOUND, ClampDeltaVel(dv));
		}

		// Check collision to other balls
		for (size_t j = i+1; j < balls_size; ++j) {
			Ball tmp_ball2;
			tmp_ball2 = balls[j];
			tmp_ball2.Move(delta_t);
			if (BallToPhysObjCollided(tmp_ball, tmp_ball2)) {
				float dv = CollideBallToBall(ball, balls[j], delta_t);
				snd_system->Play(BALL_TO_BALL_SOUND, ClampDeltaVel(dv));
			}
		}

		if (!ball.pos_updated) {
			ball.Move(delta_t);
		}
	}

	// Check collision the platform to bonuses
	TestMoveObj(player_platform, delta_t);
	for (auto it = bonuses.begin(); it != bonuses.end();) {
		Bonus &bonus = *it;
		bonus.Move(delta_t);
		if (BallToPhysObjCollided(bonus, player_platform)) {
			bonus.Collide();
			it = bonuses.erase(it);
		} else {
			++it;
		}
	}

	// Move all what is not already moved
	MoveObj(player_platform, delta_t);
	for (auto it = objects.begin(); it != objects.end(); ++it) {
		MoveObj(**it, delta_t);
	}

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

float World::ClampDeltaVel(float v) {
	v *= vel_volume_factor;
	if (v > 1.0f)
		v = 1.0f;
	return v;
}
