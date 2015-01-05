#pragma once

#include <boost/timer/timer.hpp>

// ����� �������
class Timer {
public:
	Timer();
	// ��������� �����
	void StartTiming();
	// ������� �������
	virtual double TimeElapsed();
	virtual double GlobalTime();
	virtual void ResetGlobalTime(double time = 0.0);

	// ��������� �����
	double elapsedTime;
	double globalTime;

protected:
	// ��� �������� �������
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
