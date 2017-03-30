/************************************************************************************\
 * ReplayUtils - An Andrew Shurney Production                                       *
\************************************************************************************/

/*! \file		ExclusiveLock.cpp
 *  \author		Andrew Shurney
 *  \brief		Scoped object for write locking a read/write lock
 */

#include "ExclusiveLock.h"

ExclusiveLock::ExclusiveLock(const ReadWriteLock &l) : lock(l)
{
	lock.AcquireWrite();
}

ExclusiveLock::~ExclusiveLock()
{
	lock.ReleaseRead();
}
