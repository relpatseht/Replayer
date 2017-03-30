/************************************************************************************\
 * ReplayInjector - An Andrew Shurney Production                                    *
\************************************************************************************/

/*! \file		ReplayInjector.h
 *  \author		Andrew Shurney
 *  \brief		C Dll designed to give C# applications access to a RemoteExecution
 *              frontend.
 */

#ifndef REPLAY_INJECTOR_H
#define REPLAY_INJECTOR_H

#ifdef BUILDING_DLL
# define DLL_EXPORT __declspec(dllexport)
#else
# define DLL_EXPORT __declspec(dllimport)
#endif

extern "C"
{
	typedef struct _PROCESS_INFORMATION PROCESS_INFORMATION;

	struct ReplayInjector;

	DLL_EXPORT extern ReplayInjector* __cdecl CreateReplayInjector(const PROCESS_INFORMATION* procInfo);
	DLL_EXPORT extern void __cdecl DestroyReplayInjector(ReplayInjector *injector);

	DLL_EXPORT extern bool __cdecl StartProcWithDLL(ReplayInjector *injector, const char *dllDir, const char *dllName, const char *functionName, const char *argument);

	DLL_EXPORT extern int __cdecl GetProcId(ReplayInjector *injector);
}

#undef DLL_EXPORT

#endif
