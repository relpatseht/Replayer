/************************************************************************************\
 * ReplayUtils - An Andrew Shurney Production                                       *
\************************************************************************************/

/*! \file		ReadWriteLock.h
 *  \author		Andrew Shurney
 *  \brief		SRWLock manager. Implmented as a critical section on windows <= XP
 */

#ifndef READ_WRITE_LOCK_H
#define READ_WRITE_LOCK_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

class Mutex;

class ReadWriteLock
{
	private:
		 typedef void (WINAPI * SRWLockPtr)(PSRWLOCK);
		 typedef bool (WINAPI * TrySRWLockPtr)(PSRWLOCK);

		static unsigned instances;
		static void* kernel32Handle;
		static SRWLockPtr Initialize;
		static SRWLockPtr AcquireExclusive;
		static SRWLockPtr AcquireShared;
		static TrySRWLockPtr TryAcquireExclusive;
		static TrySRWLockPtr TryAcquireShared;
		static SRWLockPtr ReleaseExclusive;
		static SRWLockPtr ReleaseShared;

		static void IncInstances();
		static void DecInstances();

		union
		{
			mutable SRWLOCK srwLock;
			mutable Mutex *mutex;
		};

	public:
		ReadWriteLock();
		~ReadWriteLock();

		void AcquireWrite() const;
		void AcquireRead() const;

		bool TryAcquireWrite() const;
		bool TryAcquireRead() const;

		void ReleaseWrite() const;
		void ReleaseRead() const;
};

#endif
