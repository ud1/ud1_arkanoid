#pragma once

// Класс таймера
class Timer {
public:
	Timer(void);
	~Timer(void);
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

// Текущее значения счетчика
__int64 Time();
double ConvertTicksToSeconds(__int64 ticks);