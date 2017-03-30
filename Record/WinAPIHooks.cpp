/************************************************************************************\
 * Record - An Andrew Shurney Production                                         *
\************************************************************************************/

/*! \file		WinAPIHooks.cpp
 *  \author		Andrew Shurney
 *  \brief		Hooks non-deterministic windows api calls
 */

#include "WinAPIHooks.h"
#include "FuncCallSerializer.h"
#include "FuncHookerFactory.h"
#include "Timer.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

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
	auto GetWindowRectHooker = FuncHookerCast(GetWindowRectPtr, GetWindowRectHook);

	BOOL ret = GetWindowRectHooker->CallInjectee(hWnd, lpRect);
	serializer->Push(ret).Push(*lpRect).Finalize();

	return ret;
}

BOOL WINAPI GetCursorPosHook(__out LPPOINT lpPoint)
{
	auto GetCursorPosHooker = FuncHookerCast(GetCursorPosPtr, GetCursorPosHook);

	BOOL ret = GetCursorPosHooker->CallInjectee(lpPoint);
	serializer->Push(ret).Push(*lpPoint).Finalize();

	return ret;
}

BOOL WINAPI GetInputStateHook(void)
{
	auto GetInputStateHooker = FuncHookerCast(GetInputStatePtr, GetInputStateHook);

	BOOL ret = GetInputStateHooker->CallInjectee();
	serializer->Push(ret).Finalize();

	return ret;
}

BOOL WINAPI GetMessageAHook(__out    LPMSG lpMsg,
	                        __in_opt HWND hWnd,
	                        __in     UINT wMsgFilterMin,
	                        __in     UINT wMsgFilterMax)
{
	auto GetMessageAHooker = FuncHookerCast(GetMessageAPtr, GetMessageAHook);

	Timer t;
	BOOL ret = GetMessageAHooker->CallInjectee(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax);
	serializer->Push(t.Elapsed()).Push(ret).Push(*lpMsg).Finalize();

	return ret;
}

BOOL WINAPI GetMessageWHook(__out    LPMSG lpMsg,
	                        __in_opt HWND hWnd,
	                        __in     UINT wMsgFilterMin,
	                        __in     UINT wMsgFilterMax)
{
	auto GetMessageWHooker = FuncHookerCast(GetMessageWPtr, GetMessageWHook);

	Timer t;
	BOOL ret = GetMessageWHooker->CallInjectee(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax);
	serializer->Push(t.Elapsed()).Push(ret).Push(*lpMsg).Finalize();

	return ret;
}

LPARAM WINAPI GetMessageExtraInfoHook(void)
{
	auto GetMessageExtraInfoHooker = FuncHookerCast(GetMessageExtraInfoPtr, GetMessageExtraInfoHook);

	LPARAM ret = GetMessageExtraInfoHooker->CallInjectee();
	serializer->Push(ret).Finalize();

	return ret;
}

DWORD WINAPI GetMessagePosHook(void)
{
	auto GetMessagePosHooker = FuncHookerCast(GetMessagePosPtr, GetMessagePosHook);

	DWORD ret = GetMessagePosHooker->CallInjectee();
	serializer->Push(ret).Finalize();

	return ret;
}

LONG WINAPI GetMessageTimeHook(void)
{
	auto GetMessageTimeHooker = FuncHookerCast(GetMessageTimePtr, GetMessageTimeHook);

	LONG ret = GetMessageTimeHooker->CallInjectee();
	serializer->Push(ret).Finalize();

	return ret;
}

DWORD WINAPI GetQueueStatusHook(__in UINT flags)
{
	auto GetQueueStatusHooker = FuncHookerCast(GetQueueStatusPtr, GetQueueStatusHook);

	DWORD ret = GetQueueStatusHooker->CallInjectee(flags);
	serializer->Push(ret).Finalize();

	return ret;
}

BOOL WINAPI PeekMessageAHook(__out    LPMSG lpMsg,
	                        __in_opt HWND hWnd,
	                        __in     UINT wMsgFilterMin,
	                        __in     UINT wMsgFilterMax,
	                        __in     UINT wRemoveMsg)
{
	auto PeekMessageAHooker = FuncHookerCast(PeekMessageAPtr, PeekMessageAHook);

	BOOL ret = PeekMessageAHooker->CallInjectee(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg);
	serializer->Push(ret).Push(*lpMsg).Finalize();

	return ret;
}

BOOL WINAPI PeekMessageWHook(__out    LPMSG lpMsg,
	                         __in_opt HWND hWnd,
	                         __in     UINT wMsgFilterMin,
	                         __in     UINT wMsgFilterMax,
	                         __in     UINT wRemoveMsg)
{
	auto PeekMessageWHooker = FuncHookerCast(PeekMessageWPtr, PeekMessageWHook);

	BOOL ret = PeekMessageWHooker->CallInjectee(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg);
	serializer->Push(ret).Push(*lpMsg).Finalize();

	return ret;
}

BOOL WINAPI WaitMessageHook(void)
{
	auto WaitMessageHooker = FuncHookerCast(WaitMessagePtr, WaitMessageHook);

	Timer t;
	BOOL ret = WaitMessageHooker->CallInjectee();
	serializer->Push(t.Elapsed()).Push(ret).Finalize();

	return ret;
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
