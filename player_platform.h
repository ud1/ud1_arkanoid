#pragma once

#include "vector2d.h"
#include "unmovable_object.h"

struct PlayerPlatform : public UnmovableObject {
	PlayerPlatform() {
		new_position = Vector(0.0, 0.0);
		prev_t = 0.0;
		angle = 0.0f;
	}

	// Copy collision detection information only
	void LightCopy(const PlayerPlatform &o) {
		position = o.position;
		velocity = o.velocity;
		scale = o.scale;
		angle = o.angle;
		angular_speed = o.angular_speed;
		max_radius = o.max_radius;
		proto_segs = o.proto_segs;
		segs.resize(proto_segs.size());
	}

	// Update only collision data
	void MoveCollData(float delta_t) {
		angle += angular_speed*delta_t;
		position = position + velocity*delta_t;
		SetPositionCollData(position, angle);

		for (size_t i = 0; i < segs.size(); ++i) {
			Segment &seg = segs[i];
			seg.velocity = velocity;
		}
	}

	void Move(float delta_t) {
		angle += angular_speed*delta_t;
		MoveTo(position + velocity*delta_t);
	}

	const Vector &SetTarget(const Vector &pos) {
		Vector left_bottom_ = left_bottom + scale*0.5;
		Vector right_top_ = right_top - scale*0.5;

		target = pos;
		if (target.x < left_bottom_.x)
			target.x = left_bottom_.x;
		if (target.y < left_bottom_.y)
			target.y = left_bottom_.y;
		if (target.x > right_top_.x)
			target.x = right_top_.x;
		if (target.y > right_top_.y)
			target.y = right_top_.y;

		return target;
	}

	void InitPosition(const Vector &pos) {
		SetTarget(pos);
		position = target;
		SetPosition(position, 0.0f);
		velocity = Vector(0.0f, 0.0f);
		angular_speed = 0.0f;
	}

	void CalculateVelocity(float delta_t) {
		if (delta_t <= 0.0f) {
			velocity = 0.0f;
			angular_speed = 0.0f;
			MoveTo(target);
		} else {
			velocity = (target - position) * (1.0f / delta_t);
			angular_speed = (angle_target - angle) / delta_t;
		}
	}

	const Vector &GetTarget() const {
		return target;
	}

	void SetArea(const Vector &left_bottom_, const Vector &right_top_) {
		left_bottom = left_bottom_;
		right_top = right_top_;
	}

	virtual void Collide(){}
	Vector new_position, velocity;

	void SetAngle(float angle_) {
		angle_target = angle_;
	}

	void SetNoVelocityLoss(bool b) {
		if (b)
			velocity_loss = 0.0f;
		else velocity_loss = velocity_loss_saved;
	}

	void SetVelocityLoss(float v) {
		velocity_loss = velocity_loss_saved = v;
	}
private:
	const Vector &MoveTo(const Vector &pos) {
		position = pos;
		SetPosition(position, angle);

		for (size_t i = 0; i < segs.size(); ++i) {
			Segment &seg = segs[i];
			seg.velocity = velocity;
		}
		return position;
	}

	Vector left_bottom, right_top;
	Vector target;
	double prev_t;
	float angle, angle_target, angular_speed;
	float velocity_loss_saved;
};
