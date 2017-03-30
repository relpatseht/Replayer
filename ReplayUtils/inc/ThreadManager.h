/************************************************************************************\
 * ReplayUtils - An Andrew Shurney Production                                       *
\************************************************************************************/

/*! \file		ThreadManager.h
 *  \author		Andrew Shurney
 *  \brief		ID's each thread based on thread which created it.
 */

#ifndef THREAD_MANAGER_H
#define THREAD_MANAGER_H

#include <unordered_map>
#include "ReadWriteLock.h"

class ThreadManager
{
	private:
		typedef std::unordered_map<unsigned, unsigned> IdMap;

		ReadWriteLock rwLock;
		IdMap threadIds;

		static const unsigned BitsPerThread = 6; // Every thread can spawn up to 63 other threads, with up to 5 threads of spwaning depth.

	public:
		unsigned GetCurrentThreadId() const;
		unsigned GetThreadId(unsigned id) const;      // id is a win32 thread id.
		unsigned GetThreadId(void *handle) const;	  // handle is a win32 thread handle.
		unsigned GetParentTheadId(unsigned id) const; // id is a win32 thread id.

		unsigned AddThreadToCurrent(unsigned id);     // id is a win32 thread id.
		unsigned AddThreadToCurrent(void *handle);    // handle is a win32 thread handle.

};

#endif
