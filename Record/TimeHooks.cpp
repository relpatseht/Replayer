/************************************************************************************\
 * Record - An Andrew Shurney Production                                         *
\************************************************************************************/

/*! \file		TimeHooks.cpp
 *  \author		Andrew Shurney
 *  \brief		Hooks functions used for timing
 */

#include "TimeHooks.h"
#include "FuncHookerFactory.h"
#include "FuncCallSerializer.h"
#include "ModuleExplorer.h"
#include "Module.h"
#include <time.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <intrin.h>

void *timePtr = NULL;
void *clockPtr = NULL;
void *timeGetTimePtr = NULL;
void *QueryPerformanceCounterPtr = NULL;
void *QueryPerformanceFrequencyPtr = NULL;

static ModuleExplorer::Modules ignoredDlls;

time_t timeHook(time_t *t)
{
	auto timeHooker = FuncHookerCast(timePtr, timeHook);

	time_t ret = timeHooker->CallInjectee(t);

	serializer->Push(ret).Finalize();

	return ret;
}

clock_t clockHook()
{
	auto clockHooker = FuncHookerCast(clockPtr, clockHook);

	clock_t ret = clockHooker->CallInjectee();

	serializer->Push(ret).Finalize();

	return ret;
}

DWORD WINAPI timeGetTimeHook(void)
{
	auto timeGetTimeHooker = FuncHookerCast(timeGetTimePtr, timeGetTimeHook);

	DWORD ret = timeGetTimeHooker->CallInjectee();
	
	for(auto it = ignoredDlls.begin(); it != ignoredDlls.end(); ++it)
		if(it->Contains(_ReturnAddress()))
			return ret;

	serializer->Push(ret).Finalize();

	return ret;
}

BOOL WINAPI QueryPerformanceCounterHook(__out LARGE_INTEGER *lpPerformanceCount)
{
	auto QueryPerformanceCounterHooker = FuncHookerCast(QueryPerformanceCounterPtr, QueryPerformanceCounterHook);

	BOOL ret = QueryPerformanceCounterHooker->CallInjectee(lpPerformanceCount);
	
	for(auto it = ignoredDlls.begin(); it != ignoredDlls.end(); ++it)
		if(it->Contains(_ReturnAddress()))
			return ret;
	
	serializer->Push(ret).Push(*lpPerformanceCount).Finalize();

	return ret;
}

BOOL WINAPI QueryPerformanceFrequencyHook(__out LARGE_INTEGER *lpFrequency)
{
	auto QueryPerformanceFrequencyHooker = FuncHookerCast(QueryPerformanceFrequencyPtr, QueryPerformanceFrequencyHook);

	BOOL ret = QueryPerformanceFrequencyHooker->CallInjectee(lpFrequency);
	
	for(auto it = ignoredDlls.begin(); it != ignoredDlls.end(); ++it)
		if(it->Contains(_ReturnAddress()))
			return ret;

	serializer->Push(ret).Push(*lpFrequency).Finalize();

	return ret;
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
