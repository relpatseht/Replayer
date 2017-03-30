/************************************************************************************\
 * Record - An Andrew Shurney Production                                         *
\************************************************************************************/

/*! \file		RandomHooks.cpp
 *  \author		Andrew Shurney
 *  \brief		Hooks functions used in pseudo random number generation
 */

#include "RandomHooks.h"
#include "FuncCallSerializer.h"
#include "FuncHookerFactory.h"
#include <stdlib.h>

void *srandPtr = NULL;

void srandHook(unsigned seed)
{
	auto srandHooker = FuncHookerCast(srandPtr, srandHook);

	srandHooker->CallInjectee(seed);

	serializer->Push(seed).Finalize();
}

bool InstallRandomHooks()
{
	auto srandHooker = CreateFuncHooker(srand, srandHook);
	if(!srandHooker || !srandHooker->InstallHook())
		return false;

	srandPtr = srandHooker;
	return true;
}
