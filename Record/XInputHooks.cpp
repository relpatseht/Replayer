/************************************************************************************\
 * Record - An Andrew Shurney Production                                         *
\************************************************************************************/

/*! \file		XInputHooks.cpp
 *  \author		Andrew Shurney
 *  \brief		Hooks XInput
 */

#include "XInputHooks.h"
#include "FuncCallSerializer.h"
#include "FuncHookerFactory.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <XInput.h>

void *XInputGetDSoundAudioDeviceGuidsPtr = NULL;
void *XInputGetAudioDeviceIdsPtr = NULL;
void *XInputGetBatteryInformationPtr = NULL;
void *XInputGetCapabilitiesPtr = NULL;
void *XInputGetKeystrokePtr = NULL;
void *XInputGetStatePtr = NULL;
void *XInputSetStatePtr = NULL;

DWORD WINAPI XInputGetDSoundAudioDeviceGuidsHook(__in  DWORD dwUserIndex,
                                                 __out GUID* pDSoundRenderGuid,
                                                 __out GUID* pDSoundCaptureGuid)
{
	auto XInputGetDSoundAudioDeviceGuidsHooker = FuncHookerCast(XInputGetDSoundAudioDeviceGuidsPtr, XInputGetDSoundAudioDeviceGuidsHook);

	DWORD ret = XInputGetDSoundAudioDeviceGuidsHooker->CallInjectee(dwUserIndex, pDSoundRenderGuid, pDSoundCaptureGuid);
	serializer->Push(ret).Push(*pDSoundRenderGuid).Push(*pDSoundCaptureGuid).Finalize();

	return ret;
}

DWORD WINAPI XInputGetAudioDeviceIdsHook(_In_        DWORD dwUserIndex,
	                                     _Out_opt_   LPWSTR pRenderDeviceId,
	                                     _Inout_opt_ UINT *pRenderCount,
	                                     _Out_opt_   LPWSTR pCaptureDeviceId,
	                                     _Inout_opt_ UINT *pCaptureCount)
{
	auto XInputGetAudioDeviceIdsHooker = FuncHookerCast(XInputGetAudioDeviceIdsPtr, XInputGetAudioDeviceIdsHook);

	DWORD ret = XInputGetAudioDeviceIdsHooker->CallInjectee(dwUserIndex, pRenderDeviceId, pRenderCount, pCaptureDeviceId, pCaptureCount);

	serializer->Push(ret);
	if(pRenderCount)
	{
		serializer->Push(*pRenderCount);
		serializer->Push(pRenderDeviceId, *pRenderCount * sizeof(WCHAR));
	}

	if(pCaptureCount)
	{
		serializer->Push(*pCaptureCount);
		serializer->Push(pCaptureDeviceId, *pCaptureCount * sizeof(WCHAR));
	}
	serializer->Finalize();

	return ret;
}

DWORD WINAPI XInputGetBatteryInformationHook(_In_  DWORD dwUserIndex,
	                                         _In_  BYTE devType,
	                                         _Out_ XINPUT_BATTERY_INFORMATION *pBatteryInformation)
{
	auto XInputGetBatteryInformationHooker = FuncHookerCast(XInputGetBatteryInformationPtr, XInputGetBatteryInformationHook);

	DWORD ret = XInputGetBatteryInformationHooker->CallInjectee(dwUserIndex, devType, pBatteryInformation);

	serializer->Push(ret).Push(*pBatteryInformation).Finalize();

	return ret;
}

DWORD WINAPI XInputGetCapabilitiesHook(_In_  DWORD dwUserIndex,
	                                   _In_  DWORD dwFlags,
	                                   _Out_ XINPUT_CAPABILITIES *pCapabilities)
{
	auto XInputGetCapabilitiesHooker = FuncHookerCast(XInputGetCapabilitiesPtr, XInputGetCapabilitiesHook);

	DWORD ret = XInputGetCapabilitiesHooker->CallInjectee(dwUserIndex, dwFlags, pCapabilities);

	serializer->Push(ret).Push(*pCapabilities).Finalize();

	return ret;
}

DWORD WINAPI XInputGetKeystrokeHook(_In_  DWORD dwUserIndex,
	                                _In_  DWORD dwReserved,
	                                _Out_ PXINPUT_KEYSTROKE pKeystroke)
{
	auto XInputGetKeystrokeHooker = FuncHookerCast(XInputGetKeystrokePtr, XInputGetKeystrokeHook);

	DWORD ret = XInputGetKeystrokeHooker->CallInjectee(dwUserIndex, dwReserved, pKeystroke);

	serializer->Push(ret).Push(*pKeystroke).Finalize();

	return ret;
}

DWORD WINAPI XInputGetStateHook(_In_  DWORD dwUserIndex,
	                            _Out_ XINPUT_STATE *pState)
{
	auto XInputGetStateHooker = FuncHookerCast(XInputGetStatePtr, XInputGetStateHook);

	DWORD ret = XInputGetStateHooker->CallInjectee(dwUserIndex, pState);

	serializer->Push(ret).Push(*pState).Finalize();

	return ret;
}

DWORD WINAPI XInputSetStateHook(_In_    DWORD dwUserIndex,
	                            _Inout_ XINPUT_VIBRATION *pVibration)
{
	auto XInputSetStateHooker = FuncHookerCast(XInputSetStatePtr, XInputSetStateHook);

	DWORD ret = XInputSetStateHooker->CallInjectee(dwUserIndex, pVibration);

	serializer->Push(ret).Push(*pVibration).Finalize();

	return ret;
}

bool InstallXInputHooks()
{
	bool ret = true;

#define INSTALL_HOOK(FUNC_NAME)													\
	auto FUNC_NAME ## Hooker = CreateFuncHooker(#FUNC_NAME, FUNC_NAME ## Hook);	\
	FUNC_NAME ## Ptr = FUNC_NAME ## Hooker;                                     \
	if(!(FUNC_NAME ## Hooker) || !(FUNC_NAME ## Hooker)->InstallHook())			\
		ret &= false;
	
	INSTALL_HOOK(XInputGetDSoundAudioDeviceGuids)
	INSTALL_HOOK(XInputGetAudioDeviceIds)
	INSTALL_HOOK(XInputGetBatteryInformation)
	INSTALL_HOOK(XInputGetCapabilities)
	INSTALL_HOOK(XInputGetKeystroke)
	INSTALL_HOOK(XInputGetState)
	INSTALL_HOOK(XInputSetState)

	return ret;
}