/************************************************************************************\
 * Replay - An Andrew Shurney Production                                         *
\************************************************************************************/

/*! \file		DirectInputDevice8AHook.cpp
 *  \author		Andrew Shurney
 *  \brief		Defines DirectInputDevice8AHook.
 */

#include "DirectInputDeviceHooks.h"
#include "DirectInputEffectHooks.h"
#include "PacketSerializer.h"
#include "Mutex.h"
#include "ThreadData.h"
#include <vector>

#pragma warning (disable : 4100)

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
	HRESULT ret = serializer->Get<HRESULT>();

	serializer->Read(*lpDIDevCaps);

	return ret;
}

HRESULT WINAPI DirectInputDevice8AHook::EnumObjects(LPDIENUMDEVICEOBJECTSCALLBACKA lpCallback,
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

HRESULT WINAPI DirectInputDevice8AHook::GetProperty(REFGUID rguidProp,
                                                    LPDIPROPHEADER pdiph)
{
	HRESULT ret;
	serializer->Read(ret).Read(*pdiph);

	return ret;
}

HRESULT WINAPI DirectInputDevice8AHook::SetProperty(THIS_ REFGUID rguidProp,LPCDIPROPHEADER pdiph)
{
	return serializer->Get<HRESULT>();
}

HRESULT WINAPI DirectInputDevice8AHook::Acquire(THIS)
{
	return serializer->Get<HRESULT>();
}

HRESULT WINAPI DirectInputDevice8AHook::Unacquire(THIS)
{
	return serializer->Get<HRESULT>();
}

HRESULT WINAPI DirectInputDevice8AHook::GetDeviceState(THIS_ DWORD cbData,LPVOID lpvData)
{
	HRESULT ret;
	serializer->Read(ret).Read(lpvData, static_cast<unsigned>(cbData));

	return ret;
}

HRESULT WINAPI DirectInputDevice8AHook::GetDeviceData(THIS_ DWORD cbObjectData,
	                                                  LPDIDEVICEOBJECTDATA rgdod,
													  LPDWORD pdwInOut,
													  DWORD dwFlags)
{
	HRESULT ret;
	serializer->Read(ret).Read(rgdod, static_cast<unsigned>(cbObjectData)).Read(pdwInOut);

	return ret;
}

HRESULT WINAPI DirectInputDevice8AHook::SetDataFormat(THIS_ LPCDIDATAFORMAT lpdf)
{
	HRESULT ret;
	serializer->Read(ret);

	return ret;
}

HRESULT WINAPI DirectInputDevice8AHook::SetEventNotification(THIS_ HANDLE handle)
{
	return serializer->Get<HRESULT>();
}

HRESULT WINAPI DirectInputDevice8AHook::SetCooperativeLevel(THIS_ HWND hwnd, DWORD dwFlags)
{
	return serializer->Get<HRESULT>();
}

HRESULT WINAPI DirectInputDevice8AHook::GetObjectInfo(THIS_ LPDIDEVICEOBJECTINSTANCEA pdidoi,DWORD dwObj,DWORD dwHow)
{
	HRESULT ret;
	serializer->Read(ret).Read(*pdidoi);

	return ret;
}

HRESULT WINAPI DirectInputDevice8AHook::GetDeviceInfo(THIS_ LPDIDEVICEINSTANCEA pdidio)
{
	HRESULT ret;
	serializer->Read(ret).Read(*pdidio);

	return ret;
}

HRESULT WINAPI DirectInputDevice8AHook::RunControlPanel(THIS_ HWND hwnd,DWORD dwFlags)
{
	return serializer->Get<HRESULT>();
}

HRESULT WINAPI DirectInputDevice8AHook::Initialize(THIS_ HINSTANCE hinst,DWORD dwVersion,REFGUID rguid)
{
	return serializer->Get<HRESULT>();
}

static Mutex createdEffectsMutex;
static std::vector<IDirectInputEffect*> createdEffects;

HRESULT WINAPI DirectInputDevice8AHook::CreateEffect(THIS_ REFGUID rguid,LPCDIEFFECT lpeff,LPDIRECTINPUTEFFECT *ppdeff,LPUNKNOWN punkOuter)
{
	HRESULT ret = serializer->Get<HRESULT>();
	did->CreateEffect(rguid, lpeff, ppdeff, punkOuter);

	if(SUCCEEDED(ret))
	{
		*ppdeff = new DirectInputEffectHook(reinterpret_cast<IDirectInputEffect*>(*ppdeff));

		Lock lock(createdEffectsMutex);
		createdEffects.push_back(*ppdeff);
	}

	return ret;
}

HRESULT WINAPI DirectInputDevice8AHook::EnumEffects(THIS_ LPDIENUMEFFECTSCALLBACKA lpCallback,LPVOID pvRef,DWORD dwEffType)
{
	unsigned numCalls = serializer->Get<unsigned>();

	for(unsigned i=0; i<numCalls; ++i)
	{
		DIEFFECTINFOA diEff = serializer->Get<DIEFFECTINFOA>();
		lpCallback(&diEff, pvRef);
	}

	return serializer->Get<HRESULT>();
}

HRESULT WINAPI DirectInputDevice8AHook::GetEffectInfo(THIS_ LPDIEFFECTINFOA pdei,REFGUID rguid)
{
	HRESULT ret;
	serializer->Read(ret).Read(*pdei);

	return ret;
}

HRESULT WINAPI DirectInputDevice8AHook::GetForceFeedbackState(THIS_ LPDWORD pdwOut)
{
	HRESULT ret;
	serializer->Read(ret).Read(*pdwOut);

	return ret;
}

HRESULT WINAPI DirectInputDevice8AHook::SendForceFeedbackCommand(THIS_ DWORD dwFlags)
{
	return serializer->Get<HRESULT>();
}

HRESULT WINAPI DirectInputDevice8AHook::EnumCreatedEffectObjects(LPDIENUMCREATEDEFFECTOBJECTSCALLBACK lpCallback,
	                                                             LPVOID pvRef, 
																 DWORD fl)
{
	unsigned numCalls = serializer->Get<unsigned>();

	for(unsigned i=0; i<numCalls; ++i)
	{
		IDirectInputEffect *effect;

		{
			Lock lock(createdEffectsMutex);
			effect = createdEffects.at(serializer->Get<unsigned>());
		}

		lpCallback(effect, pvRef);
	}

	return serializer->Get<HRESULT>();
}

HRESULT WINAPI DirectInputDevice8AHook::Escape(THIS_ LPDIEFFESCAPE pesc)
{
	HRESULT ret;
	serializer->Read(ret).Read(pesc->cbOutBuffer).Read(pesc->lpvOutBuffer, static_cast<unsigned>(pesc->cbOutBuffer));

	return ret;
}

HRESULT WINAPI DirectInputDevice8AHook::Poll(THIS)
{
	return serializer->Get<HRESULT>();
}

HRESULT WINAPI DirectInputDevice8AHook::SendDeviceData(THIS_ DWORD cbObjectData,
	                                                   LPCDIDEVICEOBJECTDATA rgdod,
													   LPDWORD pdwInOut, 
													   DWORD fl)
{
	HRESULT ret;
	serializer->Read(ret).Read(*pdwInOut).Read(const_cast<DIDEVICEOBJECTDATA*>(rgdod), static_cast<unsigned>(*pdwInOut));

	return ret;
}

HRESULT WINAPI DirectInputDevice8AHook::EnumEffectsInFile(THIS_ LPCSTR lpszFileName,
	                                                      LPDIENUMEFFECTSINFILECALLBACK pec,
														  LPVOID pvRef,
														  DWORD dwFlags)
{
	unsigned numCalls = serializer->Get<unsigned>();
	for(unsigned i=0; i<numCalls; ++i)
	{
		DIFILEEFFECT diFe = serializer->Get<DIFILEEFFECT>();
		pec(&diFe, pvRef);
	}

	return serializer->Get<HRESULT>();
}

HRESULT WINAPI DirectInputDevice8AHook::WriteEffectToFile(THIS_ LPCSTR lpszFileName,
	                                                      DWORD dwEntries,
														  LPDIFILEEFFECT rgDiFileEft,
														  DWORD dwFlags)
{
	did->WriteEffectToFile(lpszFileName, dwEntries, rgDiFileEft, dwFlags);

	return serializer->Get<HRESULT>();
}

HRESULT WINAPI DirectInputDevice8AHook::BuildActionMap(THIS_ LPDIACTIONFORMATA lpdiaf,
	                                                   LPCSTR lpszUserName,
													   DWORD dwFlags)
{
	HRESULT ret;
	serializer->Read(ret).Read(*lpdiaf);

	return ret;
}

HRESULT WINAPI DirectInputDevice8AHook::SetActionMap(THIS_ LPDIACTIONFORMATA lpdiActionFormat,
	                                                 LPCSTR lpszUserName,
													 DWORD dwFlags)
{
	return serializer->Get<HRESULT>();
}

HRESULT WINAPI DirectInputDevice8AHook::GetImageInfo(THIS_ LPDIDEVICEIMAGEINFOHEADERA lpdiDevImageInfoHeader)
{
	HRESULT ret;
	serializer->Read(ret).Read(*lpdiDevImageInfoHeader);

	return ret;
}
