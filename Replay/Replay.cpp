/************************************************************************************\
 * Replay - An Andrew Shurney Production                                         *
\************************************************************************************/

/*! \file		Replay.cpp
 *  \author		Andrew Shurney
 *  \brief		Defines Replay module exports. Also installs all hooks.
 */

#include "FuncHookerFactory.h"
#include "ThreadManager.h"
#include "PacketSerializer.h"
#include "XInputHooks.h"
#include "WinSockHooks.h"
#include "WinAPIHooks.h"
#include "TimeHooks.h"
#include "RandomHooks.h"
#include "DirectInputHooks.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

PacketSerializer *packetSerializer;

extern "C"
{
static ThreadManager threadManager;
void *CreateThreadPtr = NULL;

HANDLE WINAPI CreateThreadHook(LPSECURITY_ATTRIBUTES lpThreadAttributes, SIZE_T dwStackSize, 
	                           LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter, 
							   DWORD dwCreationFlags, LPDWORD lpThreadId)
{
	auto CreateThreadHooker = FuncHookerCast(CreateThreadPtr, CreateThreadHook);

	HANDLE threadHandle = CreateThreadHooker->CallInjectee(lpThreadAttributes, dwStackSize, lpStartAddress, 
		                                                   lpParameter, dwCreationFlags, lpThreadId);

	threadManager.AddThreadToCurrent(threadHandle);

	return threadHandle;
}

extern __declspec(dllexport) unsigned StartReplaying(const char * filename)
{
	try
	{
		packetSerializer = new PacketSerializer(threadManager, filename);
	}
	catch(...)
	{
		return 0;
	}

	auto CreateThreadHooker = CreateFuncHooker("CreateThread", CreateThreadHook);
	if(CreateThreadHooker)
	{
		CreateThreadHooker->InstallHook();
		CreateThreadPtr = CreateThreadHooker;
	}
	
	InstallTimeHooks();
	InstallWinAPIHooks();
	//InstallWinSockHooks(); Currently broken. Disabled.
	InstallXInputHooks();
	InstallRandomHooks();
	InstallDirectInputHooks();

	return 1;
}

}
