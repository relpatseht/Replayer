/************************************************************************************\
 * ReplayUtils - An Andrew Shurney Production                                       *
\************************************************************************************/

/*! \file		Mutex.cpp
 *  \author		Andrew Shurney
 *  \brief		Critical section manager and scoped lock
 */

#include "Mutex.h"

Mutex::Mutex()
{
#ifdef WIN32
	InitializeCriticalSection(&critSec);
#else
	pthread_mutex_init(&mutex, NULL);
#endif
}

Mutex::~Mutex()
{
#ifdef WIN32
	DeleteCriticalSection(&critSec);
#else
	pthread_mutex_destroy(&mutex);
#endif
}

bool Mutex::Request()
{
#ifdef WIN32
	return TryEnterCriticalSection(&critSec) == TRUE;
#else
	return pthread_mutex_trylock(&mutex) == 0;
#endif
}

void Mutex::Acquire()
{
#ifdef WIN32
	EnterCriticalSection(&critSec);
#else
	pthread_mutex_lock(&mutex);
#endif
}

void Mutex::Release()
{
#ifdef WIN32
	LeaveCriticalSection(&critSec);
#else
	pthread_mutex_unlock(&mutex);
#endif
}

Lock::Lock(Mutex &m, bool wait) : m(m)
{
	if(wait)
	{
		acquired = true;
		m.Acquire();
	}
	else
		acquired = m.Request();
}

Lock::~Lock()
{
	if(acquired)
		m.Release();
}

bool Lock::Locked() const
{
	return acquired;
}
