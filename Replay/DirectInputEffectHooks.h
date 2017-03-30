/************************************************************************************\
 * Replay - An Andrew Shurney Production                                         *
\************************************************************************************/

/*! \file		DirectInputEffectHooks.h
 *  \author		Andrew Shurney
 *  \brief		Defines classes to hook IDirectInputEffect
 */

#ifndef DIRECT_INPUT_EFFECT_HOOKS_H
#define DIRECT_INPUT_EFFECT_HOOKS_H

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

class DirectInputEffectHook : public IDirectInputEffect
{
	private:
		IDirectInputEffect *die;

	public:
		DirectInputEffectHook(IDirectInputEffect *die);

		/*** IUnknown methods ***/
		STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID * ppvObj);
		STDMETHOD_(ULONG,AddRef)(THIS);
		STDMETHOD_(ULONG,Release)(THIS);

		/*** IDirectInputEffect methods ***/
		STDMETHOD(Initialize)(THIS_ HINSTANCE,DWORD,REFGUID);
		STDMETHOD(GetEffectGuid)(THIS_ LPGUID);
		STDMETHOD(GetParameters)(THIS_ LPDIEFFECT,DWORD);
		STDMETHOD(SetParameters)(THIS_ LPCDIEFFECT,DWORD);
		STDMETHOD(Start)(THIS_ DWORD,DWORD);
		STDMETHOD(Stop)(THIS);
		STDMETHOD(GetEffectStatus)(THIS_ LPDWORD);
		STDMETHOD(Download)(THIS);
		STDMETHOD(Unload)(THIS);
		STDMETHOD(Escape)(THIS_ LPDIEFFESCAPE);
};

#endif
