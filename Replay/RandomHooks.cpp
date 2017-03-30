/************************************************************************************\
 * Replay - An Andrew Shurney Production                                         *
\************************************************************************************/

/*! \file		RandomHooks.cpp
 *  \author		Andrew Shurney
 *  \brief		Hooks pseudo random number functions
 */

#include "RandomHooks.h"
#include "PacketSerializer.h"
#include "FuncHookerFactory.h"
#include <stdlib.h>

void *srandPtr = NULL;

void srandHook(unsigned /*seed*/)
{
	auto srandHooker = FuncHookerCast(srandPtr, srandHook);

	srandHooker->CallInjectee(serializer->Get<unsigned>());
}

bool InstallRandomHooks()
{
	auto srandHooker = CreateFuncHooker(srand, srandHook);
	if(!srandHooker || !srandHooker->InstallHook())
		return false;

	srandPtr = srandHooker;
	return true;
}
