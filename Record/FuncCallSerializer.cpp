/************************************************************************************\
 * Record - An Andrew Shurney Production                                         *
\************************************************************************************/

/*! \file		FuncCallSerializer.cpp
 *  \author		Andrew Shurney
 *  \brief		Serializes a function call, sending it over a pipe
 */

#include "FuncCallSerializer.h"
#include "Pipe.h"
#include "ThreadManager.h"
#include "SharedLock.h"
#include "ExclusiveLock.h"

FuncCallSerializer::FuncCallSerializer(const ThreadManager& tm, Pipe& p) : threadManager(tm), pipe(p) {}

FuncCallSerializer& FuncCallSerializer::Push(const void *data, unsigned size)
{
	EnsureInitialized();
	
	AddToCall(data, size);

	return *this;
}

FuncCallSerializer& FuncCallSerializer::Prepend(const void *data, unsigned size)
{
	EnsureInitialized();
	
	unsigned threadId = threadManager.GetCurrentThreadId();
	{
		ExclusiveLock lock(funcCallsRW);
		DataVec& funcCall = threadFuncCalls[threadId];

		const byte* dataPtr = reinterpret_cast<const byte*>(data);
		funcCall.insert(funcCall.begin() + sizeof(unsigned short) + sizeof(unsigned)*2, dataPtr, dataPtr + size);
	}

	return *this;
}

void FuncCallSerializer::Submit(const char *funcName)
{
	unsigned funcHash = std::hash<const char*>()(funcName);
	unsigned threadId = threadManager.GetCurrentThreadId();

	{
		SharedLock lock(funcCallsRW);
		auto dataVecIt = threadFuncCalls.find(threadId);

		if(dataVecIt != threadFuncCalls.end())
		{
			unsigned size = static_cast<unsigned>(dataVecIt->second.size());
			unsigned short *callSize = reinterpret_cast<unsigned short*>(&dataVecIt->second[0]);
			*callSize = static_cast<unsigned short>(size);

			unsigned *funcHashPos = reinterpret_cast<unsigned*>(callSize+1)+1;
			*funcHashPos = funcHash;

			pipe.Write(callSize, static_cast<unsigned>(size));
		}
	}

	{
		ExclusiveLock lock(funcCallsRW);
		threadFuncCalls[threadId].clear();
	}
}

void FuncCallSerializer::EnsureInitialized()
{
	if(!GetCurSize())
	{
		// We will be storing the size of the packet in front, so make room for it.
		unsigned short callSize = 0;
		AddToCall(&callSize, sizeof(callSize));
		
		unsigned threadId = threadManager.GetCurrentThreadId();
		AddToCall(&threadId, sizeof(threadId));

		unsigned funcHash = 0;
		AddToCall(&funcHash, sizeof(funcHash));
	}
}

unsigned FuncCallSerializer::GetCurSize() const
{
	unsigned threadId = threadManager.GetCurrentThreadId();

	SharedLock lock(funcCallsRW);
	auto dataVecIt = threadFuncCalls.find(threadId);
	if(dataVecIt != threadFuncCalls.end())
		return static_cast<unsigned>(dataVecIt->second.size());

	return 0;
}

void FuncCallSerializer::AddToCall(const void *data, unsigned size)
{
	unsigned threadId = threadManager.GetCurrentThreadId();

	ExclusiveLock lock(funcCallsRW);
	DataVec& funcCall = threadFuncCalls[threadId];

	unsigned oldSize = static_cast<unsigned>(funcCall.size());
	funcCall.resize(oldSize + size);
	std::memcpy(&funcCall[oldSize], data, size);
}
