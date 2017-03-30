/************************************************************************************\
 * Record - An Andrew Shurney Production                                         *
\************************************************************************************/

/*! \file		Timer.cpp
 *  \author		Andrew Shurney
 *  \brief		A timer whose calls don't get hooked
 */

#include "Timer.h"
#include "FuncHookerFactory.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

extern void *QueryPerformanceCounterPtr;
extern void *QueryPerformanceFrequencyPtr;

Timer::Timer() : startTime(0)
{
	auto QueryPerformanceFrequencyHooker = FuncHookerCast(QueryPerformanceFrequencyPtr, QueryPerformanceFrequency);

	LARGE_INTEGER freq;
	if(QueryPerformanceFrequencyHooker)
		QueryPerformanceFrequencyHooker->CallInjectee(&freq);
	else
		QueryPerformanceFrequency(&freq);

	frequency = static_cast<double>(freq.QuadPart);
	Start();
}

void Timer::Start()
{
	startTime = GetTime();
}

unsigned Timer::Elapsed()
{
	return static_cast<unsigned>((GetTime() - startTime) / frequency * 1000.0f);
}

unsigned Timer::GetTime() const
{
	auto QueryPerformanceCounterHooker = FuncHookerCast(QueryPerformanceCounterPtr, QueryPerformanceCounter);

	LARGE_INTEGER t;
	if(QueryPerformanceCounterHooker)
		QueryPerformanceCounterHooker->CallInjectee(&t);
	else
		QueryPerformanceCounter(&t);

	return static_cast<unsigned>(t.LowPart);
}
