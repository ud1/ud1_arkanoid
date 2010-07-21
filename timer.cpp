#include <windows.h>
#include "Timer.h"
#include <assert.h>

// Частота биения таймера
UINT64 freq;

//
// CTimer
//

Timer::Timer(void) {
	LARGE_INTEGER s;
	// получаем частоту, предполагаем что это всегда возможно
	QueryPerformanceFrequency(&s);
	// сохраняем её
	freq = s.QuadPart;
	startTime = Time();
	globalStartTime = startTime;
}

Timer::~Timer(void)
{
}

// Засекаем текущее время
void Timer::StartTiming()
{
	LARGE_INTEGER s;
	// Получаем счетчик
	QueryPerformanceCounter(&s);
	// Запоминаем время
	startTime = s.QuadPart;
}

// Возвращает прошедшее время
double Timer::TimeElapsed() {
	elapsedTime = ((double)( Time() - startTime )/freq);
	return elapsedTime;
}

double Timer::GlobalTime() {
	__int64 temp = globalStartTime;
	globalStartTime = Time();
	globalTime += ((double)( globalStartTime - temp )/freq);
	return globalTime;
}

void Timer::ResetGlobalTime(double time) {
	globalStartTime = Time();
	globalTime = time;
}

//
// CAdvancedTimer
//

AdvancedTimer::AdvancedTimer() {
	timeAcceleration = 1.0;
}

double AdvancedTimer::TimeElapsed() {
	elapsedTime=((double)( Time() - startTime)/freq )*timeAcceleration;
	return elapsedTime;
}

double AdvancedTimer::GlobalTime() {
	UINT64 temp = globalStartTime;
	globalStartTime = Time();
	globalTime+=((double)( globalStartTime - temp )/freq)*timeAcceleration;
	return globalTime;
}

void AdvancedTimer::SetTimeAcceleration(double accel) {
	GlobalTime();
	timeAcceleration = accel;
}

// Текущее значение глобального счетчика
inline __int64 Time() {
	LARGE_INTEGER s;
	// Получаем счетчик
	QueryPerformanceCounter(&s);
	// Возвращаем только четвёртую часть
	return s.QuadPart;
}

// Преобразуем тики в секунды
double ConvertTicksToSeconds(__int64 ticks) {
	return (double)( ticks/freq );
}