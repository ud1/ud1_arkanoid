#pragma once

#include "vector2d.h"
#include "physical_object.h"

struct PlayerPlatform : public PhysicalObject {
	Vector ClampPosition(Vector pos) const {
		Vector left_bottom_ = left_bottom + scale*0.5;
		Vector right_top_ = right_top - scale*0.5;

		if (pos.x < left_bottom_.x)
			pos.x = left_bottom_.x;
		if (pos.y < left_bottom_.y)
			pos.y = left_bottom_.y;
		if (pos.x > right_top_.x)
			pos.x = right_top_.x;
		if (pos.y > right_top_.y)
			pos.y = right_top_.y;

		return pos;
	}

	const Vector &SetTarget(Vector pos) {
		target = ClampPosition(pos);
		return target;
	}

	void SetTargetAngle(float angle_) {
		target_angle = angle_;
	}

	void CalculateVelocity(float delta_t) {
		PhysicalObject::SetTarget(target, target_angle, delta_t);
	}

	void InitPosition(const Vector &pos, float angle_ = 0.0f) {
		target = ClampPosition(pos);
		target_angle = angle_;
		PhysicalObject::InitPosition(target, angle_);
	}

	const Vector &GetTarget() const {
		return target;
	}

	void SetArea(const Vector &left_bottom_, const Vector &right_top_) {
		left_bottom = left_bottom_;
		right_top = right_top_;
	}

	virtual void Collide(){}

	void SetNoVelocityLoss(bool b) {
		if (b)
			collision_object.velocity_loss = 0.0f;
		else collision_object.velocity_loss = velocity_loss_saved;
	}

	void SetSurfaceParams(float velocity_loss, float surf_friction_coef) {
		collision_object.velocity_loss = velocity_loss_saved = velocity_loss;
		collision_object.surf_friction_coef = surf_friction_coef;
	}
	
private:
	Vector left_bottom, right_top;

	Vector target;
	float target_angle;

	float velocity_loss_saved;
};
