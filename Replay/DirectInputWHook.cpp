/************************************************************************************\
 * Replay - An Andrew Shurney Production                                         *
\************************************************************************************/

/*! \file		DirectInputWHook.cpp
 *  \author		Andrew Shurney
 *  \brief		Defines DirectInputWHook
 */

#include "DirectInputHooks.h"
#include "PacketSerializer.h"
#include "FuncHookerFactory.h"
#include "DirectInputDeviceHooks.h"
#include "Mutex.h"
#include <vector>

#pragma warning (disable : 4100)

DirectInputWHook::DirectInputWHook(IDirectInputW *di) : di(di) {}

HRESULT WINAPI DirectInputWHook::QueryInterface(REFIID riid, LPVOID* ppvObj)
{
	return di->QueryInterface(riid, ppvObj);
}

ULONG WINAPI DirectInputWHook::AddRef()
{
	return di->AddRef();
}

ULONG WINAPI DirectInputWHook::Release()
{
	return di->Release();
}


HRESULT WINAPI DirectInputWHook::RunControlPanel(THIS_ HWND a1,DWORD a2) 
{
	return di->RunControlPanel(a1, a2);
}

HRESULT WINAPI DirectInputWHook::Initialize(THIS_ HINSTANCE a1,DWORD a2)
{
	return di->Initialize(a1, a2);
}

static Mutex createdDevicesMutex;
static std::vector<IDirectInputDeviceW*> createdDevices;

HRESULT WINAPI DirectInputWHook::CreateDevice(REFGUID rguid,
			                                  LPDIRECTINPUTDEVICEW *lplpDirectInputDevice,
			                                  LPUNKNOWN pUnkOuter)
{
	di->CreateDevice(rguid, lplpDirectInputDevice, pUnkOuter);

	HRESULT ret;
	serializer->Read(ret);

	if(SUCCEEDED(ret))
	{
		IDirectInputDeviceW *oldDevice = reinterpret_cast<IDirectInputDeviceW*>(*lplpDirectInputDevice);
		IDirectInputDeviceW *device = new DirectInputDeviceWHook(oldDevice);
		*lplpDirectInputDevice = device;

		Lock lock(createdDevicesMutex);
		createdDevices.push_back(device);
	}

	return ret;
}

HRESULT WINAPI DirectInputWHook::EnumDevices(DWORD dwDevType,
			                                 LPDIENUMDEVICESCALLBACKW lpCallback,
			                                 LPVOID pvRef,
			                                 DWORD dwFlags)
{
	unsigned numCalls = serializer->Get<unsigned>();

	for(unsigned i=0; i<numCalls; ++i)
	{
		DIDEVICEINSTANCEW ddi = serializer->Get<DIDEVICEINSTANCEW>();
		lpCallback(&ddi, pvRef);
	}

	return serializer->Get<HRESULT>();
}
								  
HRESULT WINAPI DirectInputWHook::GetDeviceStatus(REFGUID rguidInstance)
{
	return serializer->Get<HRESULT>();
}
