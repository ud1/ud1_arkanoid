#pragma once

#define _USE_MATH_DEFINES
#include <math.h>

struct Vector {
	float x, y;

	Vector() {}
	Vector(float x_, float y_) : x(x_), y(y_) {}
	Vector(float angle) {
		x = cos(angle);
		y = sin(angle);
	}

	float Dot(const Vector &o) const {
		return x*o.x + y*o.y;
	}

	float Cross(const Vector &o) const {
		return x*o.y - y*o.x;
	}

	float Length() const {
		return sqrt(x*x + y*y);
	}

	float Length2() const {
		return x*x + y*y;
	}

	Vector Rotate(float angle) const {
		float sina = sin(angle);
		float cosa = cos(angle);
		return Vector(x*cosa - y*sina, x*sina + y*cosa);
	}

	// little optimization
	Vector Rotate(float sina, float cosa) const {
		return Vector(x*cosa - y*sina, x*sina + y*cosa);
	}

	Vector RotateHalfPi() const {
		return Vector(-y, x);
	}

	void Normalize() {
		float rlen = Length();
		if (rlen > 0.0f) {
			x /= rlen;
			y /= rlen;
		} else {
			x = 1.0f;
			y = 0.0f;
		}
	}

	Vector Reflect(const Vector &normal) const {
		float proj = Dot(normal);
		return *this - normal*(2.0f*proj);
	}

	Vector operator + (const Vector &o) const {
		return Vector(x + o.x, y + o.y);
	}

	Vector operator - (const Vector &o) const {
		return Vector(x - o.x, y - o.y);
	}

	Vector operator * (float factor) const {
		return Vector(x*factor, y*factor);
	}

	Vector operator * (const Vector &o) const {
		return Vector(x*o.x, y*o.y);
	}
};
