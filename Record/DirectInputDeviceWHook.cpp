/************************************************************************************\
 * Record - An Andrew Shurney Production                                         *
\************************************************************************************/

/*! \file		DirectInputDeviceWHook.cpp
 *  \author		Andrew Shurney
 *  \brief		Defines DirectInputDeviceWHook class
 */

#include "DirectInputDeviceHooks.h"
#include "FuncCallSerializer.h"
#include "DirectInputEffectHooks.h"
#include "Mutex.h"
#include "ThreadData.h"
#include <unordered_map>

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

/*** IDirectInputDeviceW methods ***/
HRESULT WINAPI DirectInputDeviceWHook::GetCapabilities(THIS_ LPDIDEVCAPS lpDIDevCaps)
{
	HRESULT ret = did->GetCapabilities(lpDIDevCaps);
	serializer->Push(ret).Push(*lpDIDevCaps).Finalize();

	return ret;
}

static BOOL PASCAL EnumObjectsCallback(LPCDIDEVICEOBJECTINSTANCEW instance, LPVOID pvRef)
{
	LPDIENUMDEVICEOBJECTSCALLBACKW internalCallback = (LPDIENUMDEVICEOBJECTSCALLBACKW)threadData->GetData("objectsCallback");
	LPVOID internalPVRef = (LPVOID)threadData->GetData("objectsPVRef");
	
	++(*reinterpret_cast<unsigned*>(pvRef));
	BOOL ret = internalCallback(instance, internalPVRef);
	serializer->Push(*instance);

	return ret;
}

HRESULT WINAPI DirectInputDeviceWHook::EnumObjects(LPDIENUMDEVICEOBJECTSCALLBACKW lpCallback,
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

HRESULT WINAPI DirectInputDeviceWHook::GetProperty(REFGUID rguidProp,
                                                    LPDIPROPHEADER pdiph)
{
	HRESULT ret = did->GetProperty(rguidProp, pdiph);
	serializer->Push(ret).Push(*pdiph).Finalize();

	return ret;
}

HRESULT WINAPI DirectInputDeviceWHook::SetProperty(THIS_ REFGUID rguidProp,LPCDIPROPHEADER pdiph)
{
	HRESULT ret = did->SetProperty(rguidProp, pdiph);
	serializer->Push(ret).Finalize();

	return ret;
}

HRESULT WINAPI DirectInputDeviceWHook::Acquire(THIS)
{
	HRESULT ret = did->Acquire();
	serializer->Push(ret).Finalize();

	return ret;
}

HRESULT WINAPI DirectInputDeviceWHook::Unacquire(THIS)
{
	HRESULT ret = did->Unacquire();
	serializer->Push(ret).Finalize();

	return ret;
}

HRESULT WINAPI DirectInputDeviceWHook::GetDeviceState(THIS_ DWORD cbData,LPVOID lpvData)
{
	HRESULT ret = did->GetDeviceState(cbData, lpvData);
	serializer->Push(ret).Push(lpvData, cbData).Finalize();

	return ret;
}

HRESULT WINAPI DirectInputDeviceWHook::GetDeviceData(THIS_ DWORD cbObjectData,
	                                                  LPDIDEVICEOBJECTDATA rgdod,
													  LPDWORD pdwInOut,
													  DWORD dwFlags)
{
	HRESULT ret = did->GetDeviceData(cbObjectData, rgdod, pdwInOut, dwFlags);
	serializer->Push(ret).Push(rgdod, cbObjectData).Push(pdwInOut).Finalize();

	return ret;
}

HRESULT WINAPI DirectInputDeviceWHook::SetDataFormat(THIS_ LPCDIDATAFORMAT lpdf)
{
	HRESULT ret = did->SetDataFormat(lpdf);
	serializer->Push(ret).Finalize();

	return ret;
}

HRESULT WINAPI DirectInputDeviceWHook::SetEventNotification(THIS_ HANDLE handle)
{
	HRESULT ret = did->SetEventNotification(handle);
	serializer->Push(ret).Finalize();

	return ret;
}

HRESULT WINAPI DirectInputDeviceWHook::SetCooperativeLevel(THIS_ HWND hwnd, DWORD dwFlags)
{
	HRESULT ret = did->SetCooperativeLevel(hwnd, dwFlags);
	serializer->Push(ret).Finalize();

	return ret;
}

HRESULT WINAPI DirectInputDeviceWHook::GetObjectInfo(THIS_ LPDIDEVICEOBJECTINSTANCEW pdidoi,DWORD dwObj,DWORD dwHow)
{
	HRESULT ret = did->GetObjectInfo(pdidoi, dwObj, dwHow);
	serializer->Push(ret).Push(*pdidoi).Finalize();

	return ret;
}

HRESULT WINAPI DirectInputDeviceWHook::GetDeviceInfo(THIS_ LPDIDEVICEINSTANCEW pdidio)
{
	HRESULT ret = did->GetDeviceInfo(pdidio);
	serializer->Push(ret).Push(*pdidio).Finalize();

	return ret;
}

HRESULT WINAPI DirectInputDeviceWHook::RunControlPanel(THIS_ HWND hwnd,DWORD dwFlags)
{
	HRESULT ret = did->RunControlPanel(hwnd, dwFlags);
	serializer->Push(ret).Finalize();
	
	return ret;
}

HRESULT WINAPI DirectInputDeviceWHook::Initialize(THIS_ HINSTANCE hinst,DWORD dwVersion,REFGUID rguid)
{
	HRESULT ret = did->Initialize(hinst, dwVersion, rguid);
	serializer->Push(ret).Finalize();

	return ret;
}
