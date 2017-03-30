/************************************************************************************\
 * Replay - An Andrew Shurney Production                                         *
\************************************************************************************/

/*! \file		WinAPIHooks.cpp
 *  \author		Andrew Shurney
 *  \brief		Hooks non-deterministic windows api functions
 */

#include "WinAPIHooks.h"
#include "PacketSerializer.h"
#include "FuncHookerFactory.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#pragma warning (disable : 4100)

void *GetWindowRectPtr = NULL;
void *GetCursorPosPtr = NULL;
void *GetInputStatePtr = NULL;
void *GetMessageAPtr = NULL;
void *GetMessageWPtr = NULL;
void *GetMessageExtraInfoPtr = NULL;
void *GetMessagePosPtr = NULL;
void *GetMessageTimePtr = NULL;
void *GetQueueStatusPtr = NULL;
void *PeekMessageAPtr = NULL;
void *PeekMessageWPtr = NULL;
void *WaitMessagePtr = NULL;

BOOL WINAPI GetWindowRectHook(__in   HWND hWnd,
                              __out  LPRECT lpRect)
{
	try
	{
		BOOL ret;
		serializer->Read(ret).Read(*lpRect);

		return ret;
	}
	catch(...)
	{
		auto GetWindowRectHooker = FuncHookerCast(GetWindowRectPtr, GetWindowRectHook);
		return GetWindowRectHooker->CallInjectee(hWnd, lpRect);
	}
}

BOOL WINAPI GetCursorPosHook(__out LPPOINT lpPoint)
{
	try
	{
		BOOL ret;
		serializer->Read(ret).Read(*lpPoint);

		return ret;
	}
	catch(...)
	{
		auto GetCursorPosHooker = FuncHookerCast(GetCursorPosPtr, GetCursorPosHook);
		return GetCursorPosHooker->CallInjectee(lpPoint);
	}
}

BOOL WINAPI GetInputStateHook(void)
{
	try
	{
		return serializer->Get<BOOL>();
	}
	catch(...)
	{
		auto GetInputStateHooker = FuncHookerCast(GetInputStatePtr, GetInputStateHook);
		return GetInputStateHooker->CallInjectee();
	}
}

static void KillAWindowsMessageIfExists(HWND hWnd)
{
	// We need to keep the windows messages from piling up.
	auto PeekMessageAHooker = FuncHookerCast(PeekMessageAPtr, PeekMessageA);

	MSG tmp;
	PeekMessageAHooker->CallInjectee(&tmp, hWnd, 0, 0, PM_REMOVE | PM_NOYIELD);
}

BOOL WINAPI GetMessageAHook(__out    LPMSG lpMsg,
	                        __in_opt HWND hWnd,
	                        __in     UINT wMsgFilterMin,
	                        __in     UINT wMsgFilterMax)
{
	try
	{
		Sleep(serializer->Get<unsigned>());
		BOOL ret;
		serializer->Read(ret).Read(*lpMsg);
	
		KillAWindowsMessageIfExists(hWnd);

		return ret;
	}
	catch(...)
	{
		auto GetMessageAHooker = FuncHookerCast(GetMessageAPtr, GetMessageAHook);
		return GetMessageAHooker->CallInjectee(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax);
	}
}

BOOL WINAPI GetMessageWHook(__out    LPMSG lpMsg,
	                        __in_opt HWND hWnd,
	                        __in     UINT wMsgFilterMin,
	                        __in     UINT wMsgFilterMax)
{
	try
	{
		Sleep(serializer->Get<unsigned>());
		BOOL ret;
		serializer->Read(ret).Read(*lpMsg);
	
		KillAWindowsMessageIfExists(hWnd);

		return ret;
	}
	catch(...)
	{
		auto GetMessageWHooker = FuncHookerCast(GetMessageWPtr, GetMessageWHook);
		return GetMessageWHooker->CallInjectee(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax);
	}
}

LPARAM WINAPI GetMessageExtraInfoHook(void)
{
	try
	{
		return serializer->Get<LPARAM>();
	}
	catch(...)
	{
		auto GetMessageExtraInfoHooker = FuncHookerCast(GetMessageExtraInfoPtr, GetMessageExtraInfoHook);
		return GetMessageExtraInfoHooker->CallInjectee();
	}
}

DWORD WINAPI GetMessagePosHook(void)
{
	try
	{
		return serializer->Get<DWORD>();
	}
	catch(...)
	{
		auto GetMessagePosHooker = FuncHookerCast(GetMessagePosPtr, GetMessagePosHook);
		return GetMessagePosHooker->CallInjectee();
	}
}

LONG WINAPI GetMessageTimeHook(void)
{
	try
	{
		return serializer->Get<LONG>();
	}
	catch(...)
	{
		auto GetMessageTimeHooker = FuncHookerCast(GetMessageTimePtr, GetMessageTimeHook);
		return GetMessageTimeHooker->CallInjectee();
	}
}

DWORD WINAPI GetQueueStatusHook(__in UINT flags)
{
	try
	{
		return serializer->Get<DWORD>();
	}
	catch(...)
	{
		auto GetQueueStatusHooker = FuncHookerCast(GetQueueStatusPtr, GetQueueStatusHook);
		return GetQueueStatusHooker->CallInjectee(flags);
	}
}

BOOL WINAPI PeekMessageAHook(__out    LPMSG lpMsg,
	                         __in_opt HWND hWnd,
	                         __in     UINT wMsgFilterMin,
	                         __in     UINT wMsgFilterMax,
	                         __in     UINT wRemoveMsg)
{
	try
	{
		BOOL ret;
		serializer->Read(ret).Read(*lpMsg);
	
		if(wRemoveMsg & PM_REMOVE)
			KillAWindowsMessageIfExists(hWnd);

		return ret;
	}
	catch(...)
	{
		auto PeekMessageAHooker = FuncHookerCast(PeekMessageAPtr, PeekMessageAHook);
		return PeekMessageAHooker->CallInjectee(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg);
	}
}

BOOL WINAPI PeekMessageWHook(__out    LPMSG lpMsg,
	                         __in_opt HWND hWnd,
	                         __in     UINT wMsgFilterMin,
	                         __in     UINT wMsgFilterMax,
	                         __in     UINT wRemoveMsg)
{
	try
	{
		BOOL ret;
		serializer->Read(ret).Read(*lpMsg);
	
		if(wRemoveMsg & PM_REMOVE)
			KillAWindowsMessageIfExists(hWnd);

		return ret;
	}
	catch(...)
	{
		auto PeekMessageWHooker = FuncHookerCast(PeekMessageWPtr, PeekMessageWHook);
		return PeekMessageWHooker->CallInjectee(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg);
	}
}

BOOL WINAPI WaitMessageHook(void)
{
	try
	{
		Sleep(serializer->Get<unsigned>());

		return serializer->Get<BOOL>();
	}
	catch(...)
	{
		auto WaitMessageHooker = FuncHookerCast(WaitMessagePtr, WaitMessageHook);
		return WaitMessageHooker->CallInjectee();
	}
}

bool InstallWinAPIHooks()
{
	bool ret = true;

#define INSTALL_HOOK(FUNC_NAME)											                      \
	auto FUNC_NAME ## Hooker = CreateFuncHooker(#FUNC_NAME, FUNC_NAME ## Hook, "User32.dll"); \
	FUNC_NAME ## Ptr = FUNC_NAME ## Hooker;                                                   \
	if(!(FUNC_NAME ## Hooker) || !(FUNC_NAME ## Hooker)->InstallHook())			              \
		ret &= false;
	
	INSTALL_HOOK(GetWindowRect)
	INSTALL_HOOK(GetCursorPos)
	INSTALL_HOOK(GetInputState)
	INSTALL_HOOK(GetMessageA)
	INSTALL_HOOK(GetMessageW)
	INSTALL_HOOK(GetMessageExtraInfo)
	INSTALL_HOOK(GetMessagePos)
	INSTALL_HOOK(GetMessageTime)
	INSTALL_HOOK(GetQueueStatus)
	INSTALL_HOOK(PeekMessageA)
	INSTALL_HOOK(PeekMessageW)
	INSTALL_HOOK(WaitMessage)

	return ret;
}
