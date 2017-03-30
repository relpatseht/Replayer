/************************************************************************************\
 * Record - An Andrew Shurney Production                                         *
\************************************************************************************/

/*! \file		DirectInputDeviceAHook.cpp
 *  \author		Andrew Shurney
 *  \brief		Defines DirectInputDeviceAHook class
 */

#include "DirectInputDeviceHooks.h"
#include "FuncCallSerializer.h"
#include "DirectInputEffectHooks.h"
#include "Mutex.h"
#include "ThreadData.h"
#include <unordered_map>

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

/*** IDirectInputDeviceA methods ***/
HRESULT WINAPI DirectInputDeviceAHook::GetCapabilities(THIS_ LPDIDEVCAPS lpDIDevCaps)
{
	HRESULT ret = did->GetCapabilities(lpDIDevCaps);
	serializer->Push(ret).Push(*lpDIDevCaps).Finalize();

	return ret;
}

static BOOL PASCAL EnumObjectsCallback(LPCDIDEVICEOBJECTINSTANCEA instance, LPVOID pvRef)
{
	LPDIENUMDEVICEOBJECTSCALLBACKA internalCallback = (LPDIENUMDEVICEOBJECTSCALLBACKA)threadData->GetData("objectsCallback");
	LPVOID internalPVRef = (LPVOID)threadData->GetData("objectsPVRef");
	
	++(*reinterpret_cast<unsigned*>(pvRef));
	BOOL ret = internalCallback(instance, internalPVRef);
	serializer->Push(*instance);

	return ret;
}

HRESULT WINAPI DirectInputDeviceAHook::EnumObjects(LPDIENUMDEVICEOBJECTSCALLBACKA lpCallback,
                                                    LPVOID pvRef,
                                                    DWORD dwFlags)
{
	threadData->AddData("objectsCallback", lpCallback);
	threadData->AddData("objectsPVRef", pvRef);

	unsigned numCalls = 0;
	HRESULT ret = did->EnumObjects(EnumObjectsCallback, &numCalls, dwFlags);
	serializer->Push(ret).Prepend(numCalls).Finalize();

	return ret;
}

HRESULT WINAPI DirectInputDeviceAHook::GetProperty(REFGUID rguidProp,
                                                    LPDIPROPHEADER pdiph)
{
	HRESULT ret = did->GetProperty(rguidProp, pdiph);
	serializer->Push(ret).Push(*pdiph).Finalize();

	return ret;
}

HRESULT WINAPI DirectInputDeviceAHook::SetProperty(THIS_ REFGUID rguidProp,LPCDIPROPHEADER pdiph)
{
	HRESULT ret = did->SetProperty(rguidProp, pdiph);
	serializer->Push(ret).Finalize();

	return ret;
}

HRESULT WINAPI DirectInputDeviceAHook::Acquire(THIS)
{
	HRESULT ret = did->Acquire();
	serializer->Push(ret).Finalize();

	return ret;
}

HRESULT WINAPI DirectInputDeviceAHook::Unacquire(THIS)
{
	HRESULT ret = did->Unacquire();
	serializer->Push(ret).Finalize();

	return ret;
}

HRESULT WINAPI DirectInputDeviceAHook::GetDeviceState(THIS_ DWORD cbData,LPVOID lpvData)
{
	HRESULT ret = did->GetDeviceState(cbData, lpvData);
	serializer->Push(ret).Push(lpvData, cbData).Finalize();

	return ret;
}

HRESULT WINAPI DirectInputDeviceAHook::GetDeviceData(THIS_ DWORD cbObjectData,
	                                                  LPDIDEVICEOBJECTDATA rgdod,
													  LPDWORD pdwInOut,
													  DWORD dwFlags)
{
	HRESULT ret = did->GetDeviceData(cbObjectData, rgdod, pdwInOut, dwFlags);
	serializer->Push(ret).Push(rgdod, cbObjectData).Push(pdwInOut).Finalize();

	return ret;
}

HRESULT WINAPI DirectInputDeviceAHook::SetDataFormat(THIS_ LPCDIDATAFORMAT lpdf)
{
	HRESULT ret = did->SetDataFormat(lpdf);
	serializer->Push(ret).Finalize();

	return ret;
}

HRESULT WINAPI DirectInputDeviceAHook::SetEventNotification(THIS_ HANDLE handle)
{
	HRESULT ret = did->SetEventNotification(handle);
	serializer->Push(ret).Finalize();

	return ret;
}

HRESULT WINAPI DirectInputDeviceAHook::SetCooperativeLevel(THIS_ HWND hwnd, DWORD dwFlags)
{
	HRESULT ret = did->SetCooperativeLevel(hwnd, dwFlags);
	serializer->Push(ret).Finalize();

	return ret;
}

HRESULT WINAPI DirectInputDeviceAHook::GetObjectInfo(THIS_ LPDIDEVICEOBJECTINSTANCEA pdidoi,DWORD dwObj,DWORD dwHow)
{
	HRESULT ret = did->GetObjectInfo(pdidoi, dwObj, dwHow);
	serializer->Push(ret).Push(*pdidoi).Finalize();

	return ret;
}

HRESULT WINAPI DirectInputDeviceAHook::GetDeviceInfo(THIS_ LPDIDEVICEINSTANCEA pdidio)
{
	HRESULT ret = did->GetDeviceInfo(pdidio);
	serializer->Push(ret).Push(*pdidio).Finalize();

	return ret;
}

HRESULT WINAPI DirectInputDeviceAHook::RunControlPanel(THIS_ HWND hwnd,DWORD dwFlags)
{
	HRESULT ret = did->RunControlPanel(hwnd, dwFlags);
	serializer->Push(ret).Finalize();
	
	return ret;
}

HRESULT WINAPI DirectInputDeviceAHook::Initialize(THIS_ HINSTANCE hinst,DWORD dwVersion,REFGUID rguid)
{
	HRESULT ret = did->Initialize(hinst, dwVersion, rguid);
	serializer->Push(ret).Finalize();

	return ret;
}

