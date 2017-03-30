/************************************************************************************\
 * Record - An Andrew Shurney Production                                         *
\************************************************************************************/

/*! \file		DirectInput8AHook.cpp
 *  \author		Andrew Shurney
 *  \brief		Defines DirectInput8AHook class.
 */

#include "DirectInputHooks.h"
#include "FuncCallSerializer.h"
#include "FuncHookerFactory.h"
#include "ThreadData.h"
#include "Mutex.h"
#include <unordered_map>
#include "DirectInputDeviceHooks.h"

extern ThreadData *threadData;

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

typedef std::pair<unsigned, IDirectInputDevice8A*> IDDevicePair;
static Mutex createdDevicesMutex;
static std::unordered_map<IDirectInputDevice8A*, IDDevicePair> createdDevices;

HRESULT WINAPI DirectInput8AHook::CreateDevice(REFGUID rguid,
			                                  LPDIRECTINPUTDEVICE8A *lplpDirectInputDevice,
			                                  LPUNKNOWN pUnkOuter)
{
	HRESULT ret = di->CreateDevice(rguid, lplpDirectInputDevice, pUnkOuter);
	serializer->Push(ret).Finalize();

	if(SUCCEEDED(ret))
	{
		IDirectInputDevice8A *oldDevice = reinterpret_cast<IDirectInputDevice8A*>(*lplpDirectInputDevice);
		IDirectInputDevice8A *device = new DirectInputDevice8AHook(oldDevice);
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

HRESULT WINAPI DirectInput8AHook::EnumDevices(DWORD dwDevType,
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

static BOOL PASCAL EnumDevicesBySemanticsCallback(LPCDIDEVICEINSTANCEA lpddi,
	                                              LPDIRECTINPUTDEVICE8A lpdid,
	                                              DWORD dwFlags,
	                                              DWORD dwRemaining,
	                                              LPVOID pvRef)
{
	LPDIENUMDEVICESBYSEMANTICSCBA internalCallback = (LPDIENUMDEVICESBYSEMANTICSCBA)threadData->GetData("semanticCallback");
	LPVOID internalPVRef = (LPVOID)threadData->GetData("semanticsPVRef");
	
	++(*reinterpret_cast<unsigned*>(pvRef));

	IDDevicePair idDevice;
	{
		Lock lock(createdDevicesMutex);
		idDevice = createdDevices.find(lpdid)->second;
	}

	BOOL ret = internalCallback(lpddi, idDevice.second, dwFlags, dwRemaining, internalPVRef);
	serializer->Push(*lpddi).Push(idDevice.first).Push(dwFlags).Push(dwRemaining);

	return ret;
}

HRESULT WINAPI DirectInput8AHook::EnumDevicesBySemantics(LPCSTR ptszUserName,
			                                             LPDIACTIONFORMATA lpdiActionFormat,
			                                             LPDIENUMDEVICESBYSEMANTICSCBA lpCallback,
			                                             LPVOID pvRef,
			                                             DWORD dwFlags)
{
	threadData->AddData("semanticsCallback", lpCallback);
	threadData->AddData("semanticsPVRef", pvRef);

	unsigned numCalls = 0;
	HRESULT ret = di->EnumDevicesBySemantics(ptszUserName, lpdiActionFormat, EnumDevicesBySemanticsCallback, &numCalls, dwFlags);
	serializer->Push(ret).Prepend(numCalls).Finalize();

	return ret;
}

HRESULT WINAPI DirectInput8AHook::FindDevice(REFGUID rguidClass,
			                                LPCSTR ptszName,
							                LPGUID pguidInstance)
{
	HRESULT ret = di->FindDevice(rguidClass, ptszName, pguidInstance);
	serializer->Push(ret).Push(*pguidInstance).Finalize();

	return ret;
}
								  
HRESULT WINAPI DirectInput8AHook::GetDeviceStatus(REFGUID rguidInstance)
{
	HRESULT ret = di->GetDeviceStatus(rguidInstance);
	serializer->Push(ret).Finalize();

	return ret;
}
