/************************************************************************************\
 * Replay - An Andrew Shurney Production                                         *
\************************************************************************************/

/*! \file		DirectInputHooks.h
 *  \author		Andrew Shurney
 *  \brief		Defines classes to hook IDirectInput
 */

#ifndef DIRECT_INPUT_HOOKS_H
#define DIRECT_INPUT_HOOKS_H

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

class DirectInputAHook : public IDirectInputA
{
	private:
		IDirectInputA *di;

	public:
		DirectInputAHook(IDirectInputA *di);

		/*** IUnknown methods ***/
		STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID * ppvObj);
		STDMETHOD_(ULONG,AddRef)(THIS);
		STDMETHOD_(ULONG,Release)(THIS);

		/*** IDirectInputA methods ***/
		STDMETHOD(CreateDevice)(THIS_ REFGUID,LPDIRECTINPUTDEVICEA *,LPUNKNOWN);
		STDMETHOD(EnumDevices)(THIS_ DWORD,LPDIENUMDEVICESCALLBACKA,LPVOID,DWORD);
		STDMETHOD(GetDeviceStatus)(THIS_ REFGUID);
		STDMETHOD(RunControlPanel)(THIS_ HWND,DWORD);
		STDMETHOD(Initialize)(THIS_ HINSTANCE,DWORD);
};

class DirectInputWHook : public IDirectInputW
{
	private:
		IDirectInputW *di;

	public:
		DirectInputWHook(IDirectInputW *di);

		/*** IUnknown methods ***/
		STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID * ppvObj);
		STDMETHOD_(ULONG,AddRef)(THIS);
		STDMETHOD_(ULONG,Release)(THIS);

		/*** IDirectInputW methods ***/
		STDMETHOD(CreateDevice)(THIS_ REFGUID,LPDIRECTINPUTDEVICEW *,LPUNKNOWN);
		STDMETHOD(EnumDevices)(THIS_ DWORD,LPDIENUMDEVICESCALLBACKW,LPVOID,DWORD);
		STDMETHOD(GetDeviceStatus)(THIS_ REFGUID);
		STDMETHOD(RunControlPanel)(THIS_ HWND,DWORD);
		STDMETHOD(Initialize)(THIS_ HINSTANCE,DWORD);
};

class DirectInput8AHook : public IDirectInput8A
{
	private:
		IDirectInput8A *di;

	public:
		DirectInput8AHook(IDirectInput8 *di);

		/*** IUnknown methods ***/
		STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID * ppvObj);
		STDMETHOD_(ULONG,AddRef)(THIS);
		STDMETHOD_(ULONG,Release)(THIS);

		/*** IDirectInput8A methods ***/
		STDMETHOD(CreateDevice)(THIS_ REFGUID,LPDIRECTINPUTDEVICE8A *,LPUNKNOWN);
		STDMETHOD(EnumDevices)(THIS_ DWORD,LPDIENUMDEVICESCALLBACKA,LPVOID,DWORD);
		STDMETHOD(GetDeviceStatus)(THIS_ REFGUID);
		STDMETHOD(RunControlPanel)(THIS_ HWND,DWORD);
		STDMETHOD(Initialize)(THIS_ HINSTANCE,DWORD);
		STDMETHOD(FindDevice)(THIS_ REFGUID,LPCSTR,LPGUID);
		STDMETHOD(EnumDevicesBySemantics)(THIS_ LPCSTR,LPDIACTIONFORMATA,LPDIENUMDEVICESBYSEMANTICSCBA,LPVOID,DWORD);
		STDMETHOD(ConfigureDevices)(THIS_ LPDICONFIGUREDEVICESCALLBACK,LPDICONFIGUREDEVICESPARAMSA,DWORD,LPVOID);
};

class DirectInput8WHook : public IDirectInput8W
{
	private:
		IDirectInput8W *di;

	public:
		DirectInput8WHook(IDirectInput8W *di);

		/*** IUnknown methods ***/
		STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID * ppvObj);
		STDMETHOD_(ULONG,AddRef)(THIS);
		STDMETHOD_(ULONG,Release)(THIS);

		/*** IDirectInput8W methods ***/
		STDMETHOD(CreateDevice)(THIS_ REFGUID,LPDIRECTINPUTDEVICE8W *,LPUNKNOWN);
		STDMETHOD(EnumDevices)(THIS_ DWORD,LPDIENUMDEVICESCALLBACKW,LPVOID,DWORD);
		STDMETHOD(GetDeviceStatus)(THIS_ REFGUID);
		STDMETHOD(RunControlPanel)(THIS_ HWND,DWORD);
		STDMETHOD(Initialize)(THIS_ HINSTANCE,DWORD);
		STDMETHOD(FindDevice)(THIS_ REFGUID,LPCWSTR,LPGUID);
		STDMETHOD(EnumDevicesBySemantics)(THIS_ LPCWSTR,LPDIACTIONFORMATW,LPDIENUMDEVICESBYSEMANTICSCBW,LPVOID,DWORD);
		STDMETHOD(ConfigureDevices)(THIS_ LPDICONFIGUREDEVICESCALLBACK,LPDICONFIGUREDEVICESPARAMSW,DWORD,LPVOID);
};

bool InstallDirectInputHooks();

#endif
