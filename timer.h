#pragma once

// ����� �������
class Timer {
public:
	Timer(void);
	~Timer(void);
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
	__int64 startTime;
	__int64 globalStartTime;
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

// ������� �������� ��������
__int64 Time();
double ConvertTicksToSeconds(__int64 ticks);