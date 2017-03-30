/************************************************************************************\
 * ReplayUtils - An Andrew Shurney Production                                       *
\************************************************************************************/

/*! \file		ThreadData.h
 *  \author		Andrew Shurney
 *  \brief		Safely thread scope variables
 */

#ifndef THREAD_DATA_H
#define THREAD_DATA_H

#include <unordered_map>
#include <string>
#include "ReadWriteLock.h"

class ThreadManager;

class ThreadData
{
	private:
		typedef std::unordered_map<unsigned, void*> IdDataMap;
		typedef std::unordered_map<std::string, IdDataMap> NameIdDataMap;

		ThreadManager &manager;
		ReadWriteLock lock;
		NameIdDataMap data;

		ThreadData(const ThreadData&);
		ThreadData& operator=(const ThreadData&);

	public:
		ThreadData(ThreadManager &m);
		~ThreadData();

		void *GetData(const std::string& name);
		void AddData(const std::string& name, void *data);
};

#endif
