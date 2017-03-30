/************************************************************************************\
 * Replay - An Andrew Shurney Production                                         *
\************************************************************************************/

/*! \file		DirectInputEffectHook.cpp
 *  \author		Andrew Shurney
 *  \brief		Defines DirectInputEffectHook
 */

#include "DirectInputEffectHooks.h"
#include "PacketSerializer.h"

#pragma warning (disable : 4100)

DirectInputEffectHook::DirectInputEffectHook(IDirectInputEffect *die) : die(die) {}

/*** IUnknown methods ***/
HRESULT WINAPI DirectInputEffectHook::QueryInterface(THIS_ REFIID riid, LPVOID * ppvObj)
{
	return die->QueryInterface(riid, ppvObj);
}

ULONG WINAPI DirectInputEffectHook::AddRef(THIS)
{
	return die->AddRef();
}

ULONG WINAPI DirectInputEffectHook::Release(THIS)
{
	return die->Release();
}

/*** IDirectInputEffect methods ***/
HRESULT WINAPI DirectInputEffectHook::Initialize(HINSTANCE hinst,
                                                  DWORD dwVersion,
                                                  REFGUID rguid)
{
	return serializer->Get<HRESULT>();
}

HRESULT WINAPI DirectInputEffectHook::GetEffectGuid(THIS_ LPGUID pguid)
{
	HRESULT ret;
	serializer->Read(ret).Read(*pguid);

	return ret;
}

HRESULT WINAPI DirectInputEffectHook::GetParameters(LPDIEFFECT peff,
                                                     DWORD dwFlags)
{
	HRESULT ret;
	serializer->Read(ret).Read(*peff);

	return ret;
}

HRESULT WINAPI DirectInputEffectHook::SetParameters(LPCDIEFFECT peff, 
	                                                 DWORD flags)
{
	return serializer->Get<HRESULT>();
}

HRESULT WINAPI DirectInputEffectHook::Start(DWORD dwIterations,
	                                         DWORD flags)
{
	return serializer->Get<HRESULT>();
}

HRESULT WINAPI DirectInputEffectHook::Stop(THIS)
{
	return serializer->Get<HRESULT>();
}

HRESULT WINAPI DirectInputEffectHook::GetEffectStatus(LPDWORD pdwFlags)
{
	HRESULT ret;
	serializer->Read(ret).Read(*pdwFlags);

	return ret;
}

HRESULT WINAPI DirectInputEffectHook::Download(THIS)
{
	return serializer->Get<HRESULT>();
}

HRESULT WINAPI DirectInputEffectHook::Unload(THIS)
{
	return serializer->Get<HRESULT>();
}

HRESULT WINAPI DirectInputEffectHook::Escape(LPDIEFFESCAPE pesc)
{
	HRESULT ret;
	serializer->Read(ret)
		       .Read(pesc->cbOutBuffer)
			   .Read(pesc->lpvInBuffer, static_cast<unsigned>(pesc->cbOutBuffer));

	return ret;
}
