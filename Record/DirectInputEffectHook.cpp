/************************************************************************************\
 * Record - An Andrew Shurney Production                                         *
\************************************************************************************/

/*! \file		DirectInputEffectHook.cpp
 *  \author		Andrew Shurney
 *  \brief		Defines DirectInputEffectHook class
 */

#include "DirectInputEffectHooks.h"
#include "FuncCallSerializer.h"

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
	HRESULT ret = die->Initialize(hinst, dwVersion, rguid);
	serializer->Push(ret).Finalize();

	return ret;
}

HRESULT WINAPI DirectInputEffectHook::GetEffectGuid(THIS_ LPGUID pguid)
{
	HRESULT ret = die->GetEffectGuid(pguid);
	serializer->Push(ret).Push(*pguid).Finalize();

	return ret;
}

HRESULT WINAPI DirectInputEffectHook::GetParameters(LPDIEFFECT peff,
                                                     DWORD dwFlags)
{
	HRESULT ret = die->GetParameters(peff, dwFlags);
	serializer->Push(ret).Push(*peff).Finalize();

	return ret;
}

HRESULT WINAPI DirectInputEffectHook::SetParameters(LPCDIEFFECT peff, 
	                                                 DWORD flags)
{
	HRESULT ret = die->SetParameters(peff, flags);
	serializer->Push(ret).Finalize();

	return ret;
}

HRESULT WINAPI DirectInputEffectHook::Start(DWORD dwIterations,
	                                         DWORD flags)
{
	HRESULT ret = die->Start(dwIterations, flags);
	serializer->Push(ret).Finalize();

	return ret;
}

HRESULT WINAPI DirectInputEffectHook::Stop(THIS)
{
	HRESULT ret = die->Stop();
	serializer->Push(ret).Finalize();

	return ret;
}

HRESULT WINAPI DirectInputEffectHook::GetEffectStatus(LPDWORD pdwFlags)
{
	HRESULT ret = die->GetEffectStatus(pdwFlags);
	serializer->Push(ret).Push(*pdwFlags).Finalize();

	return ret;
}

HRESULT WINAPI DirectInputEffectHook::Download(THIS)
{
	HRESULT ret = die->Download();
	serializer->Push(ret).Finalize();

	return ret;
}

HRESULT WINAPI DirectInputEffectHook::Unload(THIS)
{
	HRESULT ret = die->Unload();
	serializer->Push(ret).Finalize();

	return ret;
}

HRESULT WINAPI DirectInputEffectHook::Escape(LPDIEFFESCAPE pesc)
{
	HRESULT ret = die->Escape(pesc);
	serializer->Push(ret).Push(pesc->cbOutBuffer).Push(pesc->lpvOutBuffer, pesc->cbOutBuffer).Finalize();

	return ret;
}
