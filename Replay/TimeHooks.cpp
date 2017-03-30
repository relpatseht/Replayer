/************************************************************************************\
 * Replay - An Andrew Shurney Production                                         *
\************************************************************************************/

/*! \file		TimeHooks.cpp
 *  \author		Andrew Shurney
 *  \brief		Hooks timing functions
 */

#include "TimeHooks.h"
#include "FuncHookerFactory.h"
#include "PacketSerializer.h"
#include "ModuleExplorer.h"
#include "Module.h"
#include <time.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#pragma warning (disable : 4100)

void *timePtr = NULL;
void *clockPtr = NULL;
void *timeGetTimePtr = NULL;
void *QueryPerformanceCounterPtr = NULL;
void *QueryPerformanceFrequencyPtr = NULL;

static ModuleExplorer::Modules ignoredDlls;

time_t timeHook(time_t *t)
{
	return serializer->Get<time_t>();
}

clock_t clockHook()
{
	return serializer->Get<clock_t>();
}

DWORD WINAPI timeGetTimeHook(void)
{
	auto timeGetTimeHooker = FuncHookerCast(timeGetTimePtr, timeGetTimeHook);
	
	for(auto it = ignoredDlls.begin(); it != ignoredDlls.end(); ++it)
		if(it->Contains(_ReturnAddress()))
			return timeGetTimeHooker->CallInjectee();

	return serializer->Get<DWORD>();
}

BOOL WINAPI QueryPerformanceCounterHook(__out LARGE_INTEGER *lpPerformanceCount)
{
	static LARGE_INTEGER offset;

	BOOL ret;
	auto QueryPerformanceCounterHooker = FuncHookerCast(QueryPerformanceCounterPtr, QueryPerformanceCounterHook);
	
	for(auto it = ignoredDlls.begin(); it != ignoredDlls.end(); ++it)
		if(it->Contains(_ReturnAddress()))
			return QueryPerformanceCounterHooker->CallInjectee(lpPerformanceCount);

	try
	{
		LARGE_INTEGER actualTime;
		QueryPerformanceCounterHooker->CallInjectee(&actualTime);

		serializer->Read(ret).Read(*lpPerformanceCount);

		offset.QuadPart = lpPerformanceCount->QuadPart - actualTime.QuadPart;
	}
	catch(...)
	{
		ret = QueryPerformanceCounterHooker->CallInjectee(lpPerformanceCount);
		lpPerformanceCount->QuadPart -= offset.QuadPart;
	}
	
	return ret;
}

BOOL WINAPI QueryPerformanceFrequencyHook(__out LARGE_INTEGER *lpFrequency)
{
	auto QueryPerformanceFrequencyHooker = FuncHookerCast(QueryPerformanceFrequencyPtr, QueryPerformanceFrequencyHook);
	
	for(auto it = ignoredDlls.begin(); it != ignoredDlls.end(); ++it)
		if(it->Contains(_ReturnAddress()))
			return QueryPerformanceFrequencyHooker->CallInjectee(lpFrequency);

	try
	{
		BOOL ret;
		serializer->Read(ret).Read(*lpFrequency);
		return ret;
	}
	catch(...)
	{
		return QueryPerformanceFrequencyHooker->CallInjectee(lpFrequency);
	}
}

bool InstallTimeHooks()
{
	bool ret = true;

	ModuleExplorer modExp;
	ignoredDlls = modExp.GetModules("d3d");

	ModuleExplorer::Modules fmodIgnores = modExp.GetModules("fmod");
	ignoredDlls.insert(ignoredDlls.end(), fmodIgnores.begin(), fmodIgnores.end());

#define INSTALL_HOOK(FUNC_NAME)													\
	auto FUNC_NAME ## Hooker = CreateFuncHooker(#FUNC_NAME, FUNC_NAME ## Hook);	\
	FUNC_NAME ## Ptr = FUNC_NAME ## Hooker;                                     \
	if(!(FUNC_NAME ## Hooker) || !(FUNC_NAME ## Hooker)->InstallHook())			\
	ret &= false;

	INSTALL_HOOK(time)
	INSTALL_HOOK(clock)
	INSTALL_HOOK(timeGetTime)
	INSTALL_HOOK(QueryPerformanceCounter)
	INSTALL_HOOK(QueryPerformanceFrequency)

	return ret;
}
