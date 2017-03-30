/************************************************************************************\
 * ReplayUtils - An Andrew Shurney Production                                       *
\************************************************************************************/

/*! \file		SharedLock.cpp
 *  \author		Andrew Shurney
 *  \brief		Scoped object for read locking a read/write lock
 */

#include "SharedLock.h"

SharedLock::SharedLock(const ReadWriteLock& l) : lock(l)
{
	lock.AcquireRead();
}

SharedLock::~SharedLock()
{
	lock.ReleaseRead();
}
