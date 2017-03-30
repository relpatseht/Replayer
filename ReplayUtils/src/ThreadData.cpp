/************************************************************************************\
 * ReplayUtils - An Andrew Shurney Production                                       *
\************************************************************************************/

/*! \file		ThreadData.cpp
 *  \author		Andrew Shurney
 *  \brief		Safely thread scope variables
 */

#include "ThreadData.h"
#include "SharedLock.h"
#include "ExclusiveLock.h"
#include "ThreadManager.h"

ThreadData::ThreadData(ThreadManager &m) : manager(m)
{}

ThreadData::~ThreadData()
{}

void *ThreadData::GetData(const std::string& name)
{
	SharedLock l(lock);

	auto nameIt = data.find(name);
	if(nameIt != data.end())
	{
		auto dataIt = nameIt->second.find(manager.GetCurrentThreadId());
		if(dataIt != nameIt->second.end())
			return dataIt->second;
	}

	throw std::exception();
}

void ThreadData::AddData(const std::string& name, void *voidData)
{
	ExclusiveLock l(lock);

	this->data[name][manager.GetCurrentThreadId()] = voidData;
}
