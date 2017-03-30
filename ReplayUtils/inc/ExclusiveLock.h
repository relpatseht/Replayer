/************************************************************************************\
 * ReplayUtils - An Andrew Shurney Production                                       *
\************************************************************************************/

/*! \file		ExclusiveLock.h
 *  \author		Andrew Shurney
 *  \brief		Scoped object for write locking a read/write lock
 */

#ifndef EXCLUSIVE_LOCK_H
#define EXCLUSIVE_LOCK_H

#include "ReadWriteLock.h"

class ExclusiveLock
{
	private:
		const ReadWriteLock &lock;

		ExclusiveLock(const ExclusiveLock&);
		ExclusiveLock& operator=(const ExclusiveLock&);

	public:
		ExclusiveLock(const ReadWriteLock &l);
		~ExclusiveLock();
};

#endif
