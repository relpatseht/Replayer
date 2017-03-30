/************************************************************************************\
 * Replay - An Andrew Shurney Production                                         *
\************************************************************************************/

/*! \file		DirectInputAHook.cpp
 *  \author		Andrew Shurney
 *  \brief		Defines DirectInputAHook
 */

#include "DirectInputHooks.h"
#include "PacketSerializer.h"
#include "FuncHookerFactory.h"
#include "DirectInputDeviceHooks.h"
#include "Mutex.h"
#include <vector>

#pragma warning (disable : 4100)

DirectInputAHook::DirectInputAHook(IDirectInputA *di) : di(di) {}

HRESULT WINAPI DirectInputAHook::QueryInterface(REFIID riid, LPVOID* ppvObj)
{
	return di->QueryInterface(riid, ppvObj);
}

ULONG WINAPI DirectInputAHook::AddRef()
{
	return di->AddRef();
}

ULONG WINAPI DirectInputAHook::Release()
{
	return di->Release();
}


HRESULT WINAPI DirectInputAHook::RunControlPanel(THIS_ HWND a1,DWORD a2) 
{
	return di->RunControlPanel(a1, a2);
}

HRESULT WINAPI DirectInputAHook::Initialize(THIS_ HINSTANCE a1,DWORD a2)
{
	return di->Initialize(a1, a2);
}

static Mutex createdDevicesMutex;
static std::vector<IDirectInputDeviceA*> createdDevices;

HRESULT WINAPI DirectInputAHook::CreateDevice(REFGUID rguid,
			                                  LPDIRECTINPUTDEVICEA *lplpDirectInputDevice,
			                                  LPUNKNOWN pUnkOuter)
{
	di->CreateDevice(rguid, lplpDirectInputDevice, pUnkOuter);

	HRESULT ret;
	serializer->Read(ret);

	if(SUCCEEDED(ret))
	{
		IDirectInputDeviceA *oldDevice = reinterpret_cast<IDirectInputDeviceA*>(*lplpDirectInputDevice);
		IDirectInputDeviceA *device = new DirectInputDeviceAHook(oldDevice);
		*lplpDirectInputDevice = device;

		Lock lock(createdDevicesMutex);
		createdDevices.push_back(device);
	}

	return ret;
}

HRESULT WINAPI DirectInputAHook::EnumDevices(DWORD dwDevType,
			                                 LPDIENUMDEVICESCALLBACKA lpCallback,
			                                 LPVOID pvRef,
			                                 DWORD dwFlags)
{
	unsigned numCalls = serializer->Get<unsigned>();

	for(unsigned i=0; i<numCalls; ++i)
	{
		DIDEVICEINSTANCEA ddi = serializer->Get<DIDEVICEINSTANCEA>();
		lpCallback(&ddi, pvRef);
	}

	return serializer->Get<HRESULT>();
}
								  
HRESULT WINAPI DirectInputAHook::GetDeviceStatus(REFGUID rguidInstance)
{
	return serializer->Get<HRESULT>();
}
