/************************************************************************************\
 * Record - An Andrew Shurney Production                                         *
\************************************************************************************/

/*! \file		DirectInputDevice8AHook.cpp
 *  \author		Andrew Shurney
 *  \brief		Defines DirectInputDevice8AHook class
 */

#include "DirectInputDeviceHooks.h"
#include "FuncCallSerializer.h"
#include "DirectInputEffectHooks.h"
#include "Mutex.h"
#include "ThreadData.h"
#include <unordered_map>

extern ThreadData *threadData;

DirectInputDevice8AHook::DirectInputDevice8AHook(IDirectInputDevice8A *did) : did(did)
{}

/*** IUnknown methods ***/
HRESULT WINAPI DirectInputDevice8AHook::QueryInterface(THIS_ REFIID riid, LPVOID * ppvObj)
{
	return did->QueryInterface(riid, ppvObj);
}

ULONG WINAPI DirectInputDevice8AHook::AddRef(THIS)
{
	return did->AddRef();
}

ULONG WINAPI DirectInputDevice8AHook::Release(THIS)
{
	return did->Release();
}

/*** IDirectInputDevice8A methods ***/
HRESULT WINAPI DirectInputDevice8AHook::GetCapabilities(THIS_ LPDIDEVCAPS lpDIDevCaps)
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

HRESULT WINAPI DirectInputDevice8AHook::EnumObjects(LPDIENUMDEVICEOBJECTSCALLBACKA lpCallback,
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

HRESULT WINAPI DirectInputDevice8AHook::GetProperty(REFGUID rguidProp,
                                                    LPDIPROPHEADER pdiph)
{
	HRESULT ret = did->GetProperty(rguidProp, pdiph);
	serializer->Push(ret).Push(*pdiph).Finalize();

	return ret;
}

HRESULT WINAPI DirectInputDevice8AHook::SetProperty(THIS_ REFGUID rguidProp,LPCDIPROPHEADER pdiph)
{
	HRESULT ret = did->SetProperty(rguidProp, pdiph);
	serializer->Push(ret).Finalize();

	return ret;
}

HRESULT WINAPI DirectInputDevice8AHook::Acquire(THIS)
{
	HRESULT ret = did->Acquire();
	serializer->Push(ret).Finalize();

	return ret;
}

HRESULT WINAPI DirectInputDevice8AHook::Unacquire(THIS)
{
	HRESULT ret = did->Unacquire();
	serializer->Push(ret).Finalize();

	return ret;
}

HRESULT WINAPI DirectInputDevice8AHook::GetDeviceState(THIS_ DWORD cbData,LPVOID lpvData)
{
	HRESULT ret = did->GetDeviceState(cbData, lpvData);
	serializer->Push(ret).Push(lpvData, cbData).Finalize();

	return ret;
}

HRESULT WINAPI DirectInputDevice8AHook::GetDeviceData(THIS_ DWORD cbObjectData,
	                                                  LPDIDEVICEOBJECTDATA rgdod,
													  LPDWORD pdwInOut,
													  DWORD dwFlags)
{
	HRESULT ret = did->GetDeviceData(cbObjectData, rgdod, pdwInOut, dwFlags);
	serializer->Push(ret).Push(rgdod, cbObjectData).Push(pdwInOut).Finalize();

	return ret;
}

HRESULT WINAPI DirectInputDevice8AHook::SetDataFormat(THIS_ LPCDIDATAFORMAT lpdf)
{
	HRESULT ret = did->SetDataFormat(lpdf);
	serializer->Push(ret).Finalize();

	return ret;
}

HRESULT WINAPI DirectInputDevice8AHook::SetEventNotification(THIS_ HANDLE handle)
{
	HRESULT ret = did->SetEventNotification(handle);
	serializer->Push(ret).Finalize();

	return ret;
}

HRESULT WINAPI DirectInputDevice8AHook::SetCooperativeLevel(THIS_ HWND hwnd, DWORD dwFlags)
{
	HRESULT ret = did->SetCooperativeLevel(hwnd, dwFlags);
	serializer->Push(ret).Finalize();

	return ret;
}

HRESULT WINAPI DirectInputDevice8AHook::GetObjectInfo(THIS_ LPDIDEVICEOBJECTINSTANCEA pdidoi,DWORD dwObj,DWORD dwHow)
{
	HRESULT ret = did->GetObjectInfo(pdidoi, dwObj, dwHow);
	serializer->Push(ret).Push(*pdidoi).Finalize();

	return ret;
}

HRESULT WINAPI DirectInputDevice8AHook::GetDeviceInfo(THIS_ LPDIDEVICEINSTANCEA pdidio)
{
	HRESULT ret = did->GetDeviceInfo(pdidio);
	serializer->Push(ret).Push(*pdidio).Finalize();

	return ret;
}

HRESULT WINAPI DirectInputDevice8AHook::RunControlPanel(THIS_ HWND hwnd,DWORD dwFlags)
{
	HRESULT ret = did->RunControlPanel(hwnd, dwFlags);
	serializer->Push(ret).Finalize();
	
	return ret;
}

HRESULT WINAPI DirectInputDevice8AHook::Initialize(THIS_ HINSTANCE hinst,DWORD dwVersion,REFGUID rguid)
{
	HRESULT ret = did->Initialize(hinst, dwVersion, rguid);
	serializer->Push(ret).Finalize();

	return ret;
}

typedef std::pair<unsigned, IDirectInputEffect*> IdDIEffPair;
static Mutex createdEffectsMutex;
static std::unordered_map<IDirectInputEffect*, IdDIEffPair> createdEffects;

HRESULT WINAPI DirectInputDevice8AHook::CreateEffect(THIS_ REFGUID rguid,LPCDIEFFECT lpeff,LPDIRECTINPUTEFFECT *ppdeff,LPUNKNOWN punkOuter)
{
	HRESULT ret = did->CreateEffect(rguid, lpeff, ppdeff, punkOuter);
	if(SUCCEEDED(ret))
	{
		IDirectInputEffect *oldEff = *ppdeff;
		*ppdeff = new DirectInputEffectHook(oldEff);

		Lock lock(createdEffectsMutex);
		createdEffects.insert(std::make_pair(oldEff, std::make_pair(createdEffects.size(), *ppdeff)));
	}

	serializer->Push(ret).Finalize();

	return ret;
}

static BOOL PASCAL EffectsCallback(LPCDIEFFECTINFOA pdei, LPVOID pvref)
{
	LPDIENUMEFFECTSCALLBACKA internalCallback = (LPDIENUMEFFECTSCALLBACKA)threadData->GetData("effectsCallback");
	LPVOID internalPVRef = (LPVOID)threadData->GetData("effectsPVRef");

	++(*reinterpret_cast<unsigned*>(pvref));
	BOOL ret = internalCallback(pdei, internalPVRef);
	serializer->Push(*pdei);

	return ret;
}

HRESULT WINAPI DirectInputDevice8AHook::EnumEffects(THIS_ LPDIENUMEFFECTSCALLBACKA lpCallback,LPVOID pvRef,DWORD dwEffType)
{
	threadData->AddData("effectsCallback", lpCallback);
	threadData->AddData("effectsPVRef", pvRef);

	unsigned numCalls = 0;
	HRESULT ret = did->EnumEffects(EffectsCallback, &numCalls, dwEffType);
	serializer->Push(ret).Prepend(numCalls).Finalize();

	return ret;
}

HRESULT WINAPI DirectInputDevice8AHook::GetEffectInfo(THIS_ LPDIEFFECTINFOA pdei,REFGUID rguid)
{
	HRESULT ret = did->GetEffectInfo(pdei, rguid);
	serializer->Push(ret).Push(*pdei).Finalize();

	return ret;
}

HRESULT WINAPI DirectInputDevice8AHook::GetForceFeedbackState(THIS_ LPDWORD pdwOut)
{
	HRESULT ret = did->GetForceFeedbackState(pdwOut);
	serializer->Push(ret).Push(*pdwOut).Finalize();

	return ret;
}

HRESULT WINAPI DirectInputDevice8AHook::SendForceFeedbackCommand(THIS_ DWORD dwFlags)
{
	HRESULT ret = did->SendForceFeedbackCommand(dwFlags);
	serializer->Push(ret).Finalize();

	return ret;
}

static BOOL PASCAL CreatedEffectsCallback(LPDIRECTINPUTEFFECT peff, LPVOID pvRef)
{
	LPDIENUMCREATEDEFFECTOBJECTSCALLBACK internalCallback = (LPDIENUMCREATEDEFFECTOBJECTSCALLBACK)threadData->GetData("createdEffects");
	LPVOID internalPVRef = (LPVOID)threadData->GetData("createdPVRef");
	
	++(*reinterpret_cast<unsigned*>(pvRef));

	IdDIEffPair pair;

	{
		Lock lock(createdEffectsMutex);
		pair = createdEffects.find(peff)->second;
	}

	BOOL ret = internalCallback(pair.second, internalPVRef);
	serializer->Push(pair.first);

	return ret;
}

HRESULT WINAPI DirectInputDevice8AHook::EnumCreatedEffectObjects(LPDIENUMCREATEDEFFECTOBJECTSCALLBACK lpCallback,
	                                                             LPVOID pvRef, 
																 DWORD fl)
{
	threadData->AddData("createdEffects", lpCallback);
	threadData->AddData("createdPVRef", pvRef);

	unsigned numCalls = 0;
	HRESULT ret = did->EnumCreatedEffectObjects(CreatedEffectsCallback, &numCalls, fl);
	serializer->Push(ret).Prepend(numCalls).Finalize();

	return ret;
}

HRESULT WINAPI DirectInputDevice8AHook::Escape(THIS_ LPDIEFFESCAPE pesc)
{
	HRESULT ret = did->Escape(pesc);
	serializer->Push(ret).Push(pesc->cbOutBuffer).Push(pesc->lpvOutBuffer).Finalize();

	return ret;
}

HRESULT WINAPI DirectInputDevice8AHook::Poll(THIS)
{
	HRESULT ret = did->Poll();
	serializer->Push(ret).Finalize();

	return ret;
}

HRESULT WINAPI DirectInputDevice8AHook::SendDeviceData(THIS_ DWORD cbObjectData,
	                                                   LPCDIDEVICEOBJECTDATA rgdod,
													   LPDWORD pdwInOut, 
													   DWORD fl)
{
	HRESULT ret = did->SendDeviceData(cbObjectData, rgdod, pdwInOut, fl);
	serializer->Push(ret).Push(*pdwInOut).Push(rgdod, *pdwInOut).Finalize();

	return ret;
}

BOOL PASCAL FileEffectsCallback(LPCDIFILEEFFECT lpDiFileEf, LPVOID pvRef)
{
	LPDIENUMEFFECTSINFILECALLBACK internalCallback = (LPDIENUMEFFECTSINFILECALLBACK)threadData->GetData("fileEffects");
	LPVOID internalPVRef = (LPVOID)threadData->GetData("filePVRef");
	
	++(*reinterpret_cast<unsigned*>(pvRef));
	BOOL ret = internalCallback(lpDiFileEf, internalPVRef);
	serializer->Push(*lpDiFileEf);

	return ret;
}

HRESULT WINAPI DirectInputDevice8AHook::EnumEffectsInFile(THIS_ LPCSTR lpszFileName,
	                                                      LPDIENUMEFFECTSINFILECALLBACK pec,
														  LPVOID pvRef,
														  DWORD dwFlags)
{
	threadData->AddData("fileEffects", pec);
	threadData->AddData("filePVRef", pvRef);

	unsigned numCalls = 0;
	HRESULT ret = did->EnumEffectsInFile(lpszFileName, FileEffectsCallback, &numCalls, dwFlags);
	serializer->Push(ret).Prepend(numCalls).Finalize();

	return ret;
}

HRESULT WINAPI DirectInputDevice8AHook::WriteEffectToFile(THIS_ LPCSTR lpszFileName,
	                                                      DWORD dwEntries,
														  LPDIFILEEFFECT rgDiFileEft,
														  DWORD dwFlags)
{
	HRESULT ret = did->WriteEffectToFile(lpszFileName, dwEntries, rgDiFileEft, dwFlags);
	serializer->Push(ret).Finalize();

	return ret;
}

HRESULT WINAPI DirectInputDevice8AHook::BuildActionMap(THIS_ LPDIACTIONFORMATA lpdiaf,
	                                                   LPCSTR lpszUserName,
													   DWORD dwFlags)
{
	HRESULT ret = did->BuildActionMap(lpdiaf, lpszUserName, dwFlags);
	serializer->Push(ret).Push(*lpdiaf).Finalize();

	return ret;
}

HRESULT WINAPI DirectInputDevice8AHook::SetActionMap(THIS_ LPDIACTIONFORMATA lpdiActionFormat,
	                                                 LPCSTR lpszUserName,
													 DWORD dwFlags)
{
	HRESULT ret = did->SetActionMap(lpdiActionFormat, lpszUserName, dwFlags);
	serializer->Push(ret).Finalize();
	
	return ret;
}

HRESULT WINAPI DirectInputDevice8AHook::GetImageInfo(THIS_ LPDIDEVICEIMAGEINFOHEADERA lpdiDevImageInfoHeader)
{
	HRESULT ret = did->GetImageInfo(lpdiDevImageInfoHeader);
	serializer->Push(ret).Push(*lpdiDevImageInfoHeader).Finalize();

	return ret;
}
