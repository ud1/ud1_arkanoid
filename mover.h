#pragma once
#include "vector2d.h"
#include <vector>

#define _USE_MATH_DEFINES
#include <cmath>

struct Mover {
	struct move_base {
		float period, start_time, end_time, time_offset;
		bool symmetric;
	};

	struct pos_lin : public move_base {
		Vector delta_pos;
	};

	struct pos_sine : public move_base {
		Vector delta_pos;
	};

	struct angle_lin : public move_base {
		float delta_angle;
	};

	std::vector<pos_lin> pos_lin_funcs;
	std::vector<pos_sine> pos_sine_funcs;
	std::vector<angle_lin> angle_lin_funcs;

	Vector GetPositionDelta(float t) {
		Vector res = Vector(0.0f, 0.0f);
		size_t pos_lin_funcs_size = pos_lin_funcs.size();
		for (size_t i = 0; i < pos_lin_funcs_size; ++i)
			res += Calc(pos_lin_funcs[i], t);

		size_t pos_sine_funcs_size = pos_sine_funcs.size();
		for (size_t i = 0; i < pos_sine_funcs_size; ++i)
			res += Calc(pos_sine_funcs[i], t);

		return res;
	}

	float GetAngleDelta(float t) {
		float res = 0.0f;

		size_t angle_lin_funcs_size = angle_lin_funcs.size();
		for (size_t i = 0; i < angle_lin_funcs_size; ++i)
			res += Calc(angle_lin_funcs[i], t);

		return res;
	}

	Mover &operator = (const Mover &o) {
		pos_lin_funcs = o.pos_lin_funcs;
		pos_sine_funcs = o.pos_sine_funcs;
		angle_lin_funcs = o.angle_lin_funcs;
		return *this;
	}

protected:
	float get_factor(const move_base &mv, float t) {
		t = fmod(t + mv.time_offset, mv.period);
		if (t <= mv.start_time)
			return 0.0f;

		if (t >= mv.end_time) {
			if (mv.symmetric)
				return 0.0f;
			return 1.0f;
		}

		if (mv.symmetric) {
			float mid = (mv.start_time + mv.end_time)/2.0f;
			float len = (mv.end_time - mv.start_time)/2.0f;
			return t < mid ? (t - mv.start_time)/len : (mv.end_time - t)/len;
		}

		float len = mv.end_time - mv.start_time;
		return (t - mv.start_time) / len;
	}

	Vector Calc(const pos_lin &p_lin, float t) {
		return p_lin.delta_pos*get_factor((const move_base &) p_lin, t);
	}

	Vector Calc(const pos_sine &p_sine, float t) {
		float c = (float) cos(get_factor((const move_base &) p_sine, t)*M_PI);
		return p_sine.delta_pos*( (1.0f - c)/2.0f );
	}

	float Calc(const angle_lin &a_lin, float t) {
		return a_lin.delta_angle*get_factor((const move_base &) a_lin, t);
	}
};
