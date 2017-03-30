/************************************************************************************\
 * ReplayUtils - An Andrew Shurney Production                                       *
\************************************************************************************/

/*! \file		ThreadManager.cpp
 *  \author		Andrew Shurney
 *  \brief		ID's each thread based on thread which created it.
 */

#include "ThreadManager.h"
#include "SharedLock.h"
#include "ExclusiveLock.h"
#include "RemoteThread.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>


unsigned ThreadManager::GetCurrentThreadId() const
{
	return GetThreadId(::GetCurrentThreadId());
}

unsigned ThreadManager::GetThreadId(unsigned id) const
{
	SharedLock l(rwLock);

	auto it = threadIds.find(id);
	if(it == threadIds.end())
		return 0;

	return it->second;
}

unsigned ThreadManager::GetThreadId(void *handle) const
{
	if(!handle)
		return 0;

	return GetThreadId(RemoteThread::GetThreadId(handle));
}

unsigned ThreadManager::GetParentTheadId(unsigned id) const
{
	return GetThreadId(id) >> BitsPerThread;
}

unsigned ThreadManager::AddThreadToCurrent(unsigned id)
{
	unsigned curThreadId = GetCurrentThreadId();

	unsigned parentId = curThreadId << BitsPerThread;
	const unsigned childMask = (1 << BitsPerThread) - 1;
	const unsigned parentMask = ~childMask;

	if(curThreadId != (parentId >> BitsPerThread))
	{
		MessageBox(NULL, "The thread depth is too deep. You are generally only allowed to spawn threads five deep.", "Recording error", MB_OK | MB_ICONERROR);
		throw std::exception();
	}

	unsigned maxChildNum = 0;

	for(auto it=threadIds.begin(); it != threadIds.end(); ++it)
	{
		unsigned maskedId = it->second & parentMask;
		if(maskedId == parentId)
		{
			unsigned childNum = it->second & childMask;
			if(childNum > maxChildNum)
				maxChildNum = childNum;
		}
	}

	unsigned newChildNum = maxChildNum+1;
	if(newChildNum != (newChildNum & childMask))
	{
		MessageBox(NULL, "This thread has too many direct children. In general, you are only allowed 63 children per thread.", "Recording error", MB_OK | MB_ICONERROR);
		throw std::exception();
	}

	unsigned newChildId = parentId | ((maxChildNum+1) & childMask);

	{
		ExclusiveLock l(rwLock);
		threadIds.insert(std::make_pair(id, newChildId));
	}

	return newChildId;
}

unsigned ThreadManager::AddThreadToCurrent(void *handle)
{
	if(!handle)
		return 0;

	return AddThreadToCurrent(RemoteThread::GetThreadId(handle));
}
