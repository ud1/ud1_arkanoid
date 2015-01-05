#pragma once

#include <boost/timer/timer.hpp>

// Класс таймера
class Timer {
public:
	Timer();
	// Сохраняем время
	void StartTiming();
	// Считаем разницу
	virtual double TimeElapsed();
	virtual double GlobalTime();
	virtual void ResetGlobalTime(double time = 0.0);

	// Прошедшее время
	double elapsedTime;
	double globalTime;

protected:
	// Для хранения времени
	boost::timer::cpu_timer startTime;
	boost::timer::cpu_timer globalStartTime;
};

class AdvancedTimer: public Timer {
public:
	AdvancedTimer();
	~AdvancedTimer(){};
	double TimeElapsed();
	double GlobalTime();
	void SetTimeAcceleration(double accel);
	double GetTimeAcceleration() {return timeAcceleration;}

//private:
	double timeAcceleration;
};
