/************************************************************************************\
 * Replay - An Andrew Shurney Production                                         *
\************************************************************************************/

/*! \file		XInputHooks.cpp
 *  \author		Andrew Shurney
 *  \brief		Hooks XInput
 */

#include "XInputHooks.h"
#include "PacketSerializer.h"
#include "FuncHookerFactory.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <XInput.h>

#pragma warning (disable : 4100)

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
	try
	{
		DWORD ret;
		serializer->Read(ret).Read(*pDSoundRenderGuid).Read(*pDSoundCaptureGuid);
		return ret;
	}
	catch(...)
	{
		auto XInputGetDSoundAudioDeviceGuidsHooker = FuncHookerCast(XInputGetDSoundAudioDeviceGuidsPtr, XInputGetDSoundAudioDeviceGuidsHook);
		return XInputGetDSoundAudioDeviceGuidsHooker->CallInjectee(dwUserIndex, pDSoundRenderGuid, pDSoundCaptureGuid);
	}
}

DWORD WINAPI XInputGetAudioDeviceIdsHook(_In_        DWORD dwUserIndex,
	                                     _Out_opt_   LPWSTR pRenderDeviceId,
	                                     _Inout_opt_ UINT *pRenderCount,
	                                     _Out_opt_   LPWSTR pCaptureDeviceId,
	                                     _Inout_opt_ UINT *pCaptureCount)
{
	DWORD ret;
	serializer->Read(ret);

	if(pRenderCount)
	{
		serializer->Read(*pRenderCount);
		serializer->Read(pRenderDeviceId, *pRenderCount * sizeof(WCHAR));
	}

	if(pCaptureCount)
	{
		serializer->Read(*pCaptureCount);
		serializer->Read(pCaptureDeviceId, *pCaptureCount * sizeof(WCHAR));
	}

	return ret;
}

DWORD WINAPI XInputGetBatteryInformationHook(_In_  DWORD dwUserIndex,
	                                         _In_  BYTE devType,
	                                         _Out_ XINPUT_BATTERY_INFORMATION *pBatteryInformation)
{
	DWORD ret;

	serializer->Read(ret);
	serializer->Read(*pBatteryInformation);

	return ret;
}

DWORD WINAPI XInputGetCapabilitiesHook(_In_  DWORD dwUserIndex,
	                                   _In_  DWORD dwFlags,
	                                   _Out_ XINPUT_CAPABILITIES *pCapabilities)
{
	DWORD ret;
	serializer->Read(ret).Read(*pCapabilities);

	return ret;
}

DWORD WINAPI XInputGetKeystrokeHook(_In_  DWORD dwUserIndex,
	                                _In_  DWORD dwReserved,
	                                _Out_ PXINPUT_KEYSTROKE pKeystroke)
{
	DWORD ret;
	serializer->Read(ret).Read(*pKeystroke);

	return ret;
}

DWORD WINAPI XInputGetStateHook(_In_  DWORD dwUserIndex,
	                            _Out_ XINPUT_STATE *pState)
{
	DWORD ret = serializer->Get<DWORD>();

	serializer->Read(*pState);

	return ret;
}

DWORD WINAPI XInputSetStateHook(_In_    DWORD dwUserIndex,
	                            _Inout_ XINPUT_VIBRATION *pVibration)
{
	DWORD ret;
	serializer->Read(ret).Read(*pVibration);

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