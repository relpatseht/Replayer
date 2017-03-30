/************************************************************************************\
 * Replay - An Andrew Shurney Production                                         *
\************************************************************************************/

/*! \file		DirectInputHooks.cpp
 *  \author		Andrew Shurney
 *  \brief		Hooks functions used to create direct input devices
 */

#include "DirectInputHooks.h"
#include "PacketSerializer.h"
#include "FuncHookerFactory.h"

void *DirectInputCreateAPtr = NULL;
void *DirectInputCreateWPtr = NULL;
void *DirectInput8CreatePtr = NULL;

HRESULT WINAPI DirectInputCreateAHook(HINSTANCE hinst, 
	                                  DWORD dwVersion, 
									  LPDIRECTINPUTA *ppDI, 
									  LPUNKNOWN punkOuter)
{
	auto DirectInputCreateAHooker = FuncHookerCast(DirectInputCreateAPtr, DirectInputCreateAHook);

	HRESULT ret = DirectInputCreateAHooker->CallInjectee(hinst, dwVersion, ppDI, punkOuter);
	if(SUCCEEDED(ret))
		*ppDI = new DirectInputAHook(reinterpret_cast<IDirectInputA*>(*ppDI));

	return ret;
}

HRESULT WINAPI DirectInputCreateWHook(HINSTANCE hinst, 
	                                  DWORD dwVersion, 
									  LPDIRECTINPUTW *ppDI, 
									  LPUNKNOWN punkOuter)
{
	auto DirectInputCreateWHooker = FuncHookerCast(DirectInputCreateWPtr, DirectInputCreateWHook);

	HRESULT ret = DirectInputCreateWHooker->CallInjectee(hinst, dwVersion, ppDI, punkOuter);
	if(SUCCEEDED(ret))
		*ppDI = new DirectInputWHook(reinterpret_cast<IDirectInputW*>(*ppDI));

	return ret;
}

HRESULT WINAPI DirectInput8CreateHook(HINSTANCE hinst, 
	                                  DWORD dwVersion, 
								      REFIID riidltf, 
								      LPVOID *ppvOut, 
								      LPUNKNOWN punkOuter)
{
	auto DirectInput8CreateHooker = FuncHookerCast(DirectInput8CreatePtr, DirectInput8CreateHook);

	HRESULT ret = DirectInput8CreateHooker->CallInjectee(hinst, dwVersion, riidltf, ppvOut, punkOuter);
	if(SUCCEEDED(ret))
		*ppvOut = new DirectInput8AHook(reinterpret_cast<IDirectInput8A*>(*ppvOut));

	return ret;
}

bool InstallDirectInputHooks()
{
	bool ret = true;

	#define INSTALL_HOOK(FUNC_NAME)												\
	auto FUNC_NAME ## Hooker = CreateFuncHooker(#FUNC_NAME, FUNC_NAME ## Hook);	\
	FUNC_NAME ## Ptr = FUNC_NAME ## Hooker;                                     \
	if(!(FUNC_NAME ## Hooker) || !(FUNC_NAME ## Hooker)->InstallHook())			\
	ret &= false;

	INSTALL_HOOK(DirectInputCreateA)
	INSTALL_HOOK(DirectInputCreateW)
	INSTALL_HOOK(DirectInput8Create)

	return ret;
}