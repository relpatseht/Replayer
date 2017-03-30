/************************************************************************************\
 * Record - An Andrew Shurney Production                                         *
\************************************************************************************/

/*! \file		Record.cpp
 *  \author		Andrew Shurney
 *  \brief		Defines all exports for the Record dll. Also installs all hooks.
 */

#include "FuncHookerFactory.h"
#include "Pipe.h"
#include "ThreadManager.h"
#include "ThreadData.h"
#include "FuncCallSerializer.h"
#include "TimeHooks.h"
#include "WinAPIHooks.h"
#include "WinSockHooks.h"
#include "XInputHooks.h"
#include "RandomHooks.h"
#include "DirectInputHooks.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

FuncCallSerializer *serializer;
ThreadData *threadData;

extern "C"
{
static Pipe *pipe; 
static ThreadManager threadManager;

void *createThreadPtr = NULL;

HANDLE WINAPI CreateThreadHook(LPSECURITY_ATTRIBUTES lpThreadAttributes, SIZE_T dwStackSize, 
	                           LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter, DWORD dwCreationFlags, 
							   LPDWORD lpThreadId)
{
	auto createThread = FuncHookerCast(createThreadPtr, CreateThreadHook);

	HANDLE threadHandle = createThread->CallInjectee(lpThreadAttributes, dwStackSize, lpStartAddress, lpParameter,
		                                             dwCreationFlags, lpThreadId);

	threadManager.AddThreadToCurrent(threadHandle);

	return threadHandle;
}

extern __declspec(dllexport) unsigned StartRecording(const char *pipename)
{
	pipe = new Pipe(pipename);
	if(!pipe->IsValid())
		return 0;

	serializer = new FuncCallSerializer(threadManager, *pipe);
	threadData = new ThreadData(threadManager);

	auto createThread = CreateFuncHooker("CreateThread", CreateThreadHook); 
	if(createThread)
	{
		createThread->InstallHook();
		createThreadPtr = createThread;
	}

	InstallTimeHooks();
	InstallWinAPIHooks();
	//InstallWinSockHooks(); This is currently broken. Disabled.
	InstallXInputHooks();
	InstallRandomHooks();
	InstallDirectInputHooks();

	return 1;
}

}
