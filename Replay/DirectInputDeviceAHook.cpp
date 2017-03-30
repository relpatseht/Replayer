/************************************************************************************\
 * Replay - An Andrew Shurney Production                                         *
\************************************************************************************/

/*! \file		DirectInputDeviceAHook.cpp
 *  \author		Andrew Shurney
 *  \brief		Defines DirectInputDeviceAHook
 */

#include "DirectInputDeviceHooks.h"
#include "DirectInputEffectHooks.h"
#include "PacketSerializer.h"
#include "Mutex.h"
#include "ThreadData.h"
#include <vector>

#pragma warning (disable : 4100)

extern ThreadData *threadData;

DirectInputDeviceAHook::DirectInputDeviceAHook(IDirectInputDeviceA *did) : did(did)
{}

/*** IUnknown methods ***/
HRESULT WINAPI DirectInputDeviceAHook::QueryInterface(THIS_ REFIID riid, LPVOID * ppvObj)
{
	return did->QueryInterface(riid, ppvObj);
}

ULONG WINAPI DirectInputDeviceAHook::AddRef(THIS)
{
	return did->AddRef();
}

ULONG WINAPI DirectInputDeviceAHook::Release(THIS)
{
	return did->Release();
}

/*** IDirectInputDevice8A methods ***/
HRESULT WINAPI DirectInputDeviceAHook::GetCapabilities(THIS_ LPDIDEVCAPS lpDIDevCaps)
{
	HRESULT ret = serializer->Get<HRESULT>();

	serializer->Read(*lpDIDevCaps);

	return ret;
}

HRESULT WINAPI DirectInputDeviceAHook::EnumObjects(LPDIENUMDEVICEOBJECTSCALLBACKA lpCallback,
                                                    LPVOID pvRef,
                                                    DWORD dwFlags)
{
	unsigned numCalls = serializer->Get<unsigned>();
	for(unsigned i=0; i<numCalls; ++i)
	{
		DIDEVICEOBJECTINSTANCEA dodi = serializer->Get<DIDEVICEOBJECTINSTANCEA>();
		lpCallback(&dodi, pvRef);
	}

	return serializer->Get<HRESULT>();
}

HRESULT WINAPI DirectInputDeviceAHook::GetProperty(REFGUID rguidProp,
                                                    LPDIPROPHEADER pdiph)
{
	HRESULT ret;
	serializer->Read(ret).Read(*pdiph);

	return ret;
}

HRESULT WINAPI DirectInputDeviceAHook::SetProperty(THIS_ REFGUID rguidProp,LPCDIPROPHEADER pdiph)
{
	return serializer->Get<HRESULT>();
}

HRESULT WINAPI DirectInputDeviceAHook::Acquire(THIS)
{
	return serializer->Get<HRESULT>();
}

HRESULT WINAPI DirectInputDeviceAHook::Unacquire(THIS)
{
	return serializer->Get<HRESULT>();
}

HRESULT WINAPI DirectInputDeviceAHook::GetDeviceState(THIS_ DWORD cbData,LPVOID lpvData)
{
	HRESULT ret;
	serializer->Read(ret).Read(lpvData, static_cast<unsigned>(cbData));

	return ret;
}

HRESULT WINAPI DirectInputDeviceAHook::GetDeviceData(THIS_ DWORD cbObjectData,
	                                                  LPDIDEVICEOBJECTDATA rgdod,
													  LPDWORD pdwInOut,
													  DWORD dwFlags)
{
	HRESULT ret;
	serializer->Read(ret).Read(rgdod, static_cast<unsigned>(cbObjectData)).Read(pdwInOut);

	return ret;
}

HRESULT WINAPI DirectInputDeviceAHook::SetDataFormat(THIS_ LPCDIDATAFORMAT lpdf)
{
	HRESULT ret;
	serializer->Read(ret);

	return ret;
}

HRESULT WINAPI DirectInputDeviceAHook::SetEventNotification(THIS_ HANDLE handle)
{
	return serializer->Get<HRESULT>();
}

HRESULT WINAPI DirectInputDeviceAHook::SetCooperativeLevel(THIS_ HWND hwnd, DWORD dwFlags)
{
	return serializer->Get<HRESULT>();
}

HRESULT WINAPI DirectInputDeviceAHook::GetObjectInfo(THIS_ LPDIDEVICEOBJECTINSTANCEA pdidoi,DWORD dwObj,DWORD dwHow)
{
	HRESULT ret;
	serializer->Read(ret).Read(*pdidoi);

	return ret;
}

HRESULT WINAPI DirectInputDeviceAHook::GetDeviceInfo(THIS_ LPDIDEVICEINSTANCEA pdidio)
{
	HRESULT ret;
	serializer->Read(ret).Read(*pdidio);

	return ret;
}

HRESULT WINAPI DirectInputDeviceAHook::RunControlPanel(THIS_ HWND hwnd,DWORD dwFlags)
{
	return serializer->Get<HRESULT>();
}

HRESULT WINAPI DirectInputDeviceAHook::Initialize(THIS_ HINSTANCE hinst,DWORD dwVersion,REFGUID rguid)
{
	return serializer->Get<HRESULT>();
}
