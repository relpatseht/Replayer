/************************************************************************************\
 * ReplayUtils - An Andrew Shurney Production                                       *
\************************************************************************************/

/*! \file		Mutex.h
 *  \author		Andrew Shurney
 *  \brief		Critical section manager and scoped lock
 */

#ifndef MUTEX_H
#define MUTEX_H

#ifdef WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
#else
	#include <pthread.h>
#endif

/*
	\ingroup Utilities
	Simple Mutex class. Implemented using Windows critical sections, rather
	than actual mutexes. This gives a slight speed increase at the expense
	of only working across a single process's threads. */
class Mutex
{
	private:
	#ifdef WIN32
		CRITICAL_SECTION critSec;
	#else
		pthread_mutex_t mutex;
	#endif

	public:
		Mutex();
		virtual ~Mutex();

		virtual bool Request();
		virtual void Acquire();
		virtual void Release();
};

/* Class to lock a mutex and unlock it when it falls out of scope.
	If desired, false may be passed to the second constructor argument
	to tell the Lock to only request mutex acquisition. */
class Lock
{
	private:
		Mutex &m;
		bool acquired;

		Lock(const Lock&);
		Lock& operator=(const Lock&);

	public:
		Lock(Mutex &m, bool wait=true);
		~Lock();

		bool Locked() const;
};

#endif
