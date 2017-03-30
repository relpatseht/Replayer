/************************************************************************************\
 * Replay - An Andrew Shurney Production                                         *
\************************************************************************************/

/*! \file		DirectInput8AHook.cpp
 *  \author		Andrew Shurney
 *  \brief		Defines DirectInput8AHook.
 */

#include "DirectInputHooks.h"
#include "PacketSerializer.h"
#include "FuncHookerFactory.h"
#include "DirectInputDeviceHooks.h"
#include "Mutex.h"
#include <vector>

#pragma warning (disable : 4100)

DirectInput8AHook::DirectInput8AHook(IDirectInput8A *di) : di(di) {}

HRESULT WINAPI DirectInput8AHook::QueryInterface(REFIID riid, LPVOID* ppvObj)
{
	return di->QueryInterface(riid, ppvObj);
}

ULONG WINAPI DirectInput8AHook::AddRef()
{
	return di->AddRef();
}

ULONG WINAPI DirectInput8AHook::Release()
{
	return di->Release();
}


HRESULT WINAPI DirectInput8AHook::RunControlPanel(THIS_ HWND a1,DWORD a2) 
{
	return di->RunControlPanel(a1, a2);
}

HRESULT WINAPI DirectInput8AHook::Initialize(THIS_ HINSTANCE a1,DWORD a2)
{
	return di->Initialize(a1, a2);
}

HRESULT WINAPI DirectInput8AHook::ConfigureDevices(THIS_ LPDICONFIGUREDEVICESCALLBACK a1,LPDICONFIGUREDEVICESPARAMSA a2,DWORD a3,LPVOID a4)
{
	return di->ConfigureDevices(a1, a2, a3, a4);
}

static Mutex createdDevicesMutex;
static std::vector<IDirectInputDevice8A*> createdDevices;

HRESULT WINAPI DirectInput8AHook::CreateDevice(REFGUID rguid,
			                                  LPDIRECTINPUTDEVICE8A *lplpDirectInputDevice,
			                                  LPUNKNOWN pUnkOuter)
{
	di->CreateDevice(rguid, lplpDirectInputDevice, pUnkOuter);

	HRESULT ret;
	serializer->Read(ret);

	if(SUCCEEDED(ret))
	{
		IDirectInputDevice8A *oldDevice = reinterpret_cast<IDirectInputDevice8A*>(*lplpDirectInputDevice);
		IDirectInputDevice8A *device = new DirectInputDevice8AHook(oldDevice);
		*lplpDirectInputDevice = device;

		Lock lock(createdDevicesMutex);
		createdDevices.push_back(device);
	}

	return ret;
}

HRESULT WINAPI DirectInput8AHook::EnumDevices(DWORD dwDevType,
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

HRESULT WINAPI DirectInput8AHook::EnumDevicesBySemantics(LPCSTR ptszUserName,
			                                             LPDIACTIONFORMATA lpdiActionFormat,
			                                             LPDIENUMDEVICESBYSEMANTICSCBA lpCallback,
			                                             LPVOID pvRef,
			                                             DWORD dwFlags)
{
	unsigned numCalls = serializer->Get<unsigned>();
	for(unsigned i=0; i<numCalls; ++i)
	{
		DIDEVICEINSTANCEA ddi;
		serializer->Read(ddi);

		IDirectInputDevice8A *did;
		{
			Lock lock(createdDevicesMutex);
			did = createdDevices.at(serializer->Get<unsigned>());
		}

		DWORD flags = serializer->Get<DWORD>();
		DWORD remaining = serializer->Get<DWORD>();
	
		lpCallback(&ddi, did, flags, remaining, pvRef);
	}

	return serializer->Get<HRESULT>();
}

HRESULT WINAPI DirectInput8AHook::FindDevice(REFGUID rguidClass,
			                                LPCSTR ptszName,
							                LPGUID pguidInstance)
{
	HRESULT ret = serializer->Get<HRESULT>();
		
	serializer->Read(*pguidInstance);

	return ret;
}
								  
HRESULT WINAPI DirectInput8AHook::GetDeviceStatus(REFGUID rguidInstance)
{
	return serializer->Get<HRESULT>();
}
