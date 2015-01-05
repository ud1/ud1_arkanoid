#include "timer.h"
#include <assert.h>

//
// CTimer
//

Timer::Timer() {
	startTime.start();
	globalStartTime.start();
}

// Засекаем текущее время
void Timer::StartTiming()
{
	startTime.start();
}

// Возвращает прошедшее время
double Timer::TimeElapsed() {
	const boost::timer::cpu_times elapsed_times = startTime.elapsed();
	boost::timer::nanosecond_type const elapsed(elapsed_times.wall);
	return (double) elapsed / 1.0e9;
}

double Timer::GlobalTime() {
	const boost::timer::cpu_times elapsed_times = globalStartTime.elapsed();
	globalStartTime.start();
	boost::timer::nanosecond_type const elapsed(elapsed_times.wall);
	globalTime += (double) elapsed / 1.0e9;
	return globalTime;
}

void Timer::ResetGlobalTime(double time) {
	globalStartTime.start();
	globalTime = time;
}

//
// CAdvancedTimer
//

AdvancedTimer::AdvancedTimer() {
	timeAcceleration = 1.0;
}

double AdvancedTimer::TimeElapsed() {
	return Timer::TimeElapsed()*timeAcceleration;
}

double AdvancedTimer::GlobalTime() {
	const boost::timer::cpu_times elapsed_times = globalStartTime.elapsed();
	globalStartTime.start();
	boost::timer::nanosecond_type const elapsed(elapsed_times.wall);
	globalTime += (double) elapsed / 1.0e9 * timeAcceleration;
	return globalTime;
}

void AdvancedTimer::SetTimeAcceleration(double accel) {
	GlobalTime();
	timeAcceleration = accel;
}
