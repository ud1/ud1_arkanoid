#include <windows.h>
#include "Timer.h"
#include <assert.h>

// ������� ������ �������
UINT64 freq;

//
// CTimer
//

Timer::Timer(void) {
	LARGE_INTEGER s;
	// �������� �������, ������������ ��� ��� ������ ��������
	QueryPerformanceFrequency(&s);
	// ��������� �
	freq = s.QuadPart;
	startTime = Time();
	globalStartTime = startTime;
}

Timer::~Timer(void)
{
}

// �������� ������� �����
void Timer::StartTiming()
{
	LARGE_INTEGER s;
	// �������� �������
	QueryPerformanceCounter(&s);
	// ���������� �����
	startTime = s.QuadPart;
}

// ���������� ��������� �����
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

// ������� �������� ����������� ��������
inline __int64 Time() {
	LARGE_INTEGER s;
	// �������� �������
	QueryPerformanceCounter(&s);
	// ���������� ������ �������� �����
	return s.QuadPart;
}

// ����������� ���� � �������
double ConvertTicksToSeconds(__int64 ticks) {
	return (double)( ticks/freq );
}