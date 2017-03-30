/************************************************************************************\
 * ReplayInjector - An Andrew Shurney Production                                    *
\************************************************************************************/

/*! \file		ReplayInjector.cpp
 *  \author		Andrew Shurney
 *  \brief		C Dll designed to give C# applications access to a RemoteExecution
 *              frontend.
 */

#include "ReplayInjector.h"
#include "RemoteCode.h"
#include "RemoteFunction.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

struct ReplayInjector
{
	PROCESS_INFORMATION procInfo;
	RemoteCode remoteCode;

	ReplayInjector(const PROCESS_INFORMATION& proc) : procInfo(proc), remoteCode(procInfo.dwProcessId)
	{
		remoteCode.Initialize();
	}

	~ReplayInjector()
	{
		CloseHandle(procInfo.hThread);
		CloseHandle(procInfo.hProcess);
	}
};

__declspec(dllexport) ReplayInjector* __cdecl CreateReplayInjector(const PROCESS_INFORMATION* procInfo)
{
	return new ReplayInjector(*procInfo);
}

__declspec(dllexport) void __cdecl DestroyReplayInjector(ReplayInjector *injector)
{
	delete injector;
}

__declspec(dllexport) bool __cdecl StartProcWithDLL(ReplayInjector *injector, const char *dllDir, const char *dllName, const char *functionName, const char *argument)
{
	RemoteFunction dllDirectoryHandle = injector->remoteCode.GetFunction("Kernel32.dll", "SetDllDirectoryA");
	if(!dllDirectoryHandle.IsValid())
		return false;

	dllDirectoryHandle.SetCC(RemoteFunction::CC_STDCALL);
	dllDirectoryHandle.Call<unsigned>(dllDir);

	RemoteFunction functionHandle = injector->remoteCode.GetFunction(dllName, functionName);
	if(!functionHandle.IsValid())
		return false;

	unsigned functionSuccessful = functionHandle.Call<unsigned>(argument);
	if(!functionSuccessful)
		return false;

	ResumeThread(injector->procInfo.hThread);
	return true;
}

__declspec(dllexport) int __cdecl GetProcId(ReplayInjector *injector)
{
	return injector->procInfo.dwProcessId;
}
