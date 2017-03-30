/************************************************************************************\
 * Record - An Andrew Shurney Production                                         *
\************************************************************************************/

/*! \file		DirectInputAHook.cpp
 *  \author		Andrew Shurney
 *  \brief		Defines DirectInputAHook class
 */

#include "DirectInputHooks.h"
#include "FuncCallSerializer.h"
#include "FuncHookerFactory.h"
#include "ThreadData.h"
#include "Mutex.h"
#include <unordered_map>
#include "DirectInputDeviceHooks.h"

extern ThreadData *threadData;

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

typedef std::pair<unsigned, IDirectInputDeviceA*> IDDevicePair;
static Mutex createdDevicesMutex;
static std::unordered_map<IDirectInputDeviceA*, IDDevicePair> createdDevices;

HRESULT WINAPI DirectInputAHook::CreateDevice(REFGUID rguid,
			                                  LPDIRECTINPUTDEVICEA *lplpDirectInputDevice,
			                                  LPUNKNOWN pUnkOuter)
{
	HRESULT ret = di->CreateDevice(rguid, lplpDirectInputDevice, pUnkOuter);
	serializer->Push(ret).Finalize();

	if(SUCCEEDED(ret))
	{
		IDirectInputDeviceA *oldDevice = reinterpret_cast<IDirectInputDeviceA*>(*lplpDirectInputDevice);
		IDirectInputDeviceA *device = new DirectInputDeviceAHook(oldDevice);
		*lplpDirectInputDevice = device;

		Lock lock(createdDevicesMutex);
		createdDevices.insert(std::make_pair(oldDevice, IDDevicePair(createdDevices.size(), device)));
	}

	return ret;
}

static BOOL PASCAL EnumDevicesCallback(LPCDIDEVICEINSTANCEA instance, LPVOID pvRef)
{
	LPDIENUMDEVICESCALLBACKA internalCallback = (LPDIENUMDEVICESCALLBACKA)threadData->GetData("deviceCallback");
	LPVOID internalPVRef = (LPVOID)threadData->GetData("devicePVRef");
	
	++(*reinterpret_cast<unsigned*>(pvRef));
	BOOL ret = internalCallback(instance, internalPVRef);
	serializer->Push(*instance);
	// Note, no finalize. These are all called by EnumDevices. That call will finalize.
	return ret;
}

HRESULT WINAPI DirectInputAHook::EnumDevices(DWORD dwDevType,
			                                 LPDIENUMDEVICESCALLBACKA lpCallback,
			                                 LPVOID pvRef,
			                                 DWORD dwFlags)
{
	threadData->AddData("deviceCallback", lpCallback);
	threadData->AddData("devicePVRef", pvRef);

	unsigned numCalls = 0;
	HRESULT ret = di->EnumDevices(dwDevType, EnumDevicesCallback, &numCalls, dwFlags);
	serializer->Push(ret).Prepend(numCalls).Finalize();

	return ret;
}
								  
HRESULT WINAPI DirectInputAHook::GetDeviceStatus(REFGUID rguidInstance)
{
	HRESULT ret = di->GetDeviceStatus(rguidInstance);
	serializer->Push(ret).Finalize();

	return ret;
}
