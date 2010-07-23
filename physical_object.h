#pragma once

#include "object_prototype.h"
#include "vector2d.h"
#include "mover.h"

struct PhysicalObject {
	virtual ~PhysicalObject(){}

	void SetPrototype(const ObjectPrototype &p, Vector scale_) {
		scale = scale_;
		collision_object.SetPrototype(p.col_obj_proto, scale);
		render_object.SetPrototype(p.render_obj_proto, scale);
	}

	void InitPosition(Vector pos, float angle_) {
		position = render_position = test_position = pos;
		angle = render_angle = test_angle = angle_;
		velocity = Vector(0.0f, 0.0f);
		angular_velocity = 0.0f;
		collision_object.SetPosition(position, angle, velocity, angular_velocity);
		render_object.SetPosition(position, angle);
	}

	void SetTarget(Vector pos, float angle_, float delta_t) {
		velocity = (pos - position)/delta_t;
		angular_velocity = (angle_ - angle)/delta_t;
	}

	void CalculateVelocity(float abs_t, float delta_t) {
		SetTarget(position + mover.GetPositionDelta(abs_t), angle + mover.GetAngleDelta(abs_t), delta_t);
	}

	void TestMove(float delta_t) {
		if (angular_velocity == 0.0f && velocity.IsNull())
			return; // do nothing

		test_position = position + velocity*delta_t;
		test_angle = angle + angular_velocity*delta_t;
		collision_object.SetPosition(test_position, test_angle, velocity, angular_velocity);
	}

	void ApplyMove() {
		position = test_position;
		angle = test_angle;
	}

	void Move(float delta_t) {
		TestMove(delta_t);
		ApplyMove();
	}

	const PolygonCollisionObject &GetCollObject() const {
		return collision_object;
	}

	const RenderObject &GetRenderObject() {
		// Check if something has changed
		if (angle != render_angle || position != render_position) {
			render_position = position;
			render_angle = angle;
			render_object.SetPosition(position, angle);
		}
		
		return render_object;
	}

	void SetSurfaceParams(float velocity_loss, float surf_friction_coef) {
		collision_object.velocity_loss = velocity_loss;
		collision_object.surf_friction_coef = surf_friction_coef;
	}

	virtual void Collide(){}

	Vector GetPosition() const {
		return test_position;
	}

	float GetVelocityLoss() const {
		return collision_object.velocity_loss;
	}

	float GetSurfaceFrictionCoef() const {
		return collision_object.surf_friction_coef;
	}

	bool pos_updated;
	Mover mover;
protected:
	PolygonCollisionObject collision_object;
	RenderObject render_object;

	Vector position, velocity;
	float angle, angular_velocity;

	Vector test_position;
	float test_angle;

	Vector render_position;
	float render_angle;

	Vector scale;
};
