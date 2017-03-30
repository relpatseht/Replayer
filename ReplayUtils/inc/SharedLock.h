/************************************************************************************\
 * ReplayUtils - An Andrew Shurney Production                                       *
\************************************************************************************/

/*! \file		SharedLock.h
 *  \author		Andrew Shurney
 *  \brief		Scoped object for read locking a read/write lock
 */

#ifndef SHARED_LOCK_H
#define SHARED_LOCK_H

#include "ReadWriteLock.h"

class SharedLock
{
	private:
		const ReadWriteLock &lock;

		SharedLock(const SharedLock&);
		SharedLock& operator=(const SharedLock&);

	public:
		SharedLock(const ReadWriteLock& l);
		~SharedLock();
};

#endif
