/************************************************************************************\
 * Record - An Andrew Shurney Production                                         *
\************************************************************************************/

/*! \file		DirectInputWHook.cpp
 *  \author		Andrew Shurney
 *  \brief		Defines DirectInputWHook class
 */

#include "DirectInputHooks.h"
#include "FuncCallSerializer.h"
#include "FuncHookerFactory.h"
#include "ThreadData.h"
#include "Mutex.h"
#include <unordered_map>
#include "DirectInputDeviceHooks.h"

extern ThreadData *threadData;

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

typedef std::pair<unsigned, IDirectInputDeviceW*> IDDevicePair;
static Mutex createdDevicesMutex;
static std::unordered_map<IDirectInputDeviceW*, IDDevicePair> createdDevices;

HRESULT WINAPI DirectInputWHook::CreateDevice(REFGUID rguid,
			                                  LPDIRECTINPUTDEVICEW *lplpDirectInputDevice,
			                                  LPUNKNOWN pUnkOuter)
{
	HRESULT ret = di->CreateDevice(rguid, lplpDirectInputDevice, pUnkOuter);
	serializer->Push(ret).Finalize();

	if(SUCCEEDED(ret))
	{
		IDirectInputDeviceW *oldDevice = reinterpret_cast<IDirectInputDeviceW*>(*lplpDirectInputDevice);
		IDirectInputDeviceW *device = new DirectInputDeviceWHook(oldDevice);
		*lplpDirectInputDevice = device;

		Lock lock(createdDevicesMutex);
		createdDevices.insert(std::make_pair(oldDevice, IDDevicePair(createdDevices.size(), device)));
	}

	return ret;
}

static BOOL PASCAL EnumDevicesCallback(LPCDIDEVICEINSTANCEW instance, LPVOID pvRef)
{
	LPDIENUMDEVICESCALLBACKW internalCallback = (LPDIENUMDEVICESCALLBACKW)threadData->GetData("deviceCallback");
	LPVOID internalPVRef = (LPVOID)threadData->GetData("devicePVRef");
	
	++(*reinterpret_cast<unsigned*>(pvRef));
	BOOL ret = internalCallback(instance, internalPVRef);
	serializer->Push(*instance);
	// Note, no finalize. These are all called by EnumDevices. That call will finalize.
	return ret;
}

HRESULT WINAPI DirectInputWHook::EnumDevices(DWORD dwDevType,
			                                 LPDIENUMDEVICESCALLBACKW lpCallback,
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
								  
HRESULT WINAPI DirectInputWHook::GetDeviceStatus(REFGUID rguidInstance)
{
	HRESULT ret = di->GetDeviceStatus(rguidInstance);
	serializer->Push(ret).Finalize();

	return ret;
}
