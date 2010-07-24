#pragma once

#include "vector2d.h"
#include "mover.h"

#include <istream>

std::istream &operator >> (std::istream &s, Vector &v) {
	s >> v.x >> v.y;
	return s;
}

std::istream &operator >> (std::istream &s, Mover::pos_lin &p_lin) {
	s >> p_lin.period >> p_lin.start_time >> p_lin.end_time >> p_lin.time_offset >> p_lin.symmetric >> p_lin.delta_pos;
	return s;
}

std::istream &operator >> (std::istream &s, Mover::pos_sine &p_sine) {
	s >> p_sine.period >> p_sine.start_time >> p_sine.end_time >> p_sine.time_offset >> p_sine.symmetric >> p_sine.delta_pos;
	return s;
}

std::istream &operator >> (std::istream &s, Mover::angle_lin &a_lin) {
	s >> a_lin.period >> a_lin.start_time >> a_lin.end_time >> a_lin.time_offset >> a_lin.symmetric >> a_lin.delta_angle;
	a_lin.delta_angle *= (float) M_PI/180.f;
	return s;
}

std::istream &operator >> (std::istream &s, Mover::angle_sine &a_sine) {
	s >> a_sine.period >> a_sine.start_time >> a_sine.end_time >> a_sine.time_offset >> a_sine.symmetric >> a_sine.delta_angle;
	a_sine.delta_angle *= (float) M_PI/180.f;
	return s;
}
