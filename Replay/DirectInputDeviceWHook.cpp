/************************************************************************************\
 * Replay - An Andrew Shurney Production                                         *
\************************************************************************************/

/*! \file		DirectInputDeviceWHook.cpp
 *  \author		Andrew Shurney
 *  \brief		Defines DirectInputDeviceWHook
 */

#include "DirectInputDeviceHooks.h"
#include "DirectInputEffectHooks.h"
#include "PacketSerializer.h"
#include "Mutex.h"
#include "ThreadData.h"
#include <vector>

#pragma warning (disable : 4100)

extern ThreadData *threadData;

DirectInputDeviceWHook::DirectInputDeviceWHook(IDirectInputDeviceW *did) : did(did)
{}

/*** IUnknown methods ***/
HRESULT WINAPI DirectInputDeviceWHook::QueryInterface(THIS_ REFIID riid, LPVOID * ppvObj)
{
	return did->QueryInterface(riid, ppvObj);
}

ULONG WINAPI DirectInputDeviceWHook::AddRef(THIS)
{
	return did->AddRef();
}

ULONG WINAPI DirectInputDeviceWHook::Release(THIS)
{
	return did->Release();
}

/*** IDirectInputDevice8A methods ***/
HRESULT WINAPI DirectInputDeviceWHook::GetCapabilities(THIS_ LPDIDEVCAPS lpDIDevCaps)
{
	HRESULT ret = serializer->Get<HRESULT>();

	serializer->Read(*lpDIDevCaps);

	return ret;
}

HRESULT WINAPI DirectInputDeviceWHook::EnumObjects(LPDIENUMDEVICEOBJECTSCALLBACKW lpCallback,
                                                    LPVOID pvRef,
                                                    DWORD dwFlags)
{
	unsigned numCalls = serializer->Get<unsigned>();
	for(unsigned i=0; i<numCalls; ++i)
	{
		DIDEVICEOBJECTINSTANCEW dodi = serializer->Get<DIDEVICEOBJECTINSTANCEW>();
		lpCallback(&dodi, pvRef);
	}

	return serializer->Get<HRESULT>();
}

HRESULT WINAPI DirectInputDeviceWHook::GetProperty(REFGUID rguidProp,
                                                    LPDIPROPHEADER pdiph)
{
	HRESULT ret;
	serializer->Read(ret).Read(*pdiph);

	return ret;
}

HRESULT WINAPI DirectInputDeviceWHook::SetProperty(THIS_ REFGUID rguidProp,LPCDIPROPHEADER pdiph)
{
	return serializer->Get<HRESULT>();
}

HRESULT WINAPI DirectInputDeviceWHook::Acquire(THIS)
{
	return serializer->Get<HRESULT>();
}

HRESULT WINAPI DirectInputDeviceWHook::Unacquire(THIS)
{
	return serializer->Get<HRESULT>();
}

HRESULT WINAPI DirectInputDeviceWHook::GetDeviceState(THIS_ DWORD cbData,LPVOID lpvData)
{
	HRESULT ret;
	serializer->Read(ret).Read(lpvData, static_cast<unsigned>(cbData));

	return ret;
}

HRESULT WINAPI DirectInputDeviceWHook::GetDeviceData(THIS_ DWORD cbObjectData,
	                                                  LPDIDEVICEOBJECTDATA rgdod,
													  LPDWORD pdwInOut,
													  DWORD dwFlags)
{
	HRESULT ret;
	serializer->Read(ret).Read(rgdod, static_cast<unsigned>(cbObjectData)).Read(pdwInOut);

	return ret;
}

HRESULT WINAPI DirectInputDeviceWHook::SetDataFormat(THIS_ LPCDIDATAFORMAT lpdf)
{
	HRESULT ret;
	serializer->Read(ret);

	return ret;
}

HRESULT WINAPI DirectInputDeviceWHook::SetEventNotification(THIS_ HANDLE handle)
{
	return serializer->Get<HRESULT>();
}

HRESULT WINAPI DirectInputDeviceWHook::SetCooperativeLevel(THIS_ HWND hwnd, DWORD dwFlags)
{
	return serializer->Get<HRESULT>();
}

HRESULT WINAPI DirectInputDeviceWHook::GetObjectInfo(THIS_ LPDIDEVICEOBJECTINSTANCEW pdidoi,DWORD dwObj,DWORD dwHow)
{
	HRESULT ret;
	serializer->Read(ret).Read(*pdidoi);

	return ret;
}

HRESULT WINAPI DirectInputDeviceWHook::GetDeviceInfo(THIS_ LPDIDEVICEINSTANCEW pdidio)
{
	HRESULT ret;
	serializer->Read(ret).Read(*pdidio);

	return ret;
}

HRESULT WINAPI DirectInputDeviceWHook::RunControlPanel(THIS_ HWND hwnd,DWORD dwFlags)
{
	return serializer->Get<HRESULT>();
}

HRESULT WINAPI DirectInputDeviceWHook::Initialize(THIS_ HINSTANCE hinst,DWORD dwVersion,REFGUID rguid)
{
	return serializer->Get<HRESULT>();
}
