/************************************************************************************\
 * ReplayUtils - An Andrew Shurney Production                                       *
\************************************************************************************/

/*! \file		ReadWriteLock.cpp
 *  \author		Andrew Shurney
 *  \brief		SRWLock manager. Implmented as a critical section on windows <= XP
 */

#include "ReadWriteLock.h"
#include "Mutex.h"

unsigned ReadWriteLock::instances = 0;
void* ReadWriteLock::kernel32Handle = nullptr;
ReadWriteLock::SRWLockPtr    ReadWriteLock::Initialize          = nullptr;
ReadWriteLock::SRWLockPtr    ReadWriteLock::AcquireExclusive    = nullptr;
ReadWriteLock::SRWLockPtr    ReadWriteLock::AcquireShared       = nullptr;
ReadWriteLock::TrySRWLockPtr ReadWriteLock::TryAcquireExclusive = nullptr;
ReadWriteLock::TrySRWLockPtr ReadWriteLock::TryAcquireShared    = nullptr;
ReadWriteLock::SRWLockPtr    ReadWriteLock::ReleaseExclusive    = nullptr;
ReadWriteLock::SRWLockPtr    ReadWriteLock::ReleaseShared       = nullptr;

void ReadWriteLock::IncInstances()
{
	if(!instances++)
	{
		kernel32Handle = LoadLibrary("kernel32.dll");
		if(!kernel32Handle)
			return;

		Initialize          = (SRWLockPtr)    GetProcAddress((HMODULE)kernel32Handle, "InitializeSRWLock");
		AcquireExclusive    = (SRWLockPtr)    GetProcAddress((HMODULE)kernel32Handle, "AcquireSRWLockExclusive");
		AcquireShared       = (SRWLockPtr)    GetProcAddress((HMODULE)kernel32Handle, "AcquireSRWLockShared");
		TryAcquireExclusive = (TrySRWLockPtr) GetProcAddress((HMODULE)kernel32Handle, "TryAcquireSRWLockExclusive");
		TryAcquireShared    = (TrySRWLockPtr) GetProcAddress((HMODULE)kernel32Handle, "TryAcquireSRWLockShared");
		ReleaseExclusive    = (SRWLockPtr)    GetProcAddress((HMODULE)kernel32Handle, "ReleaseSRWLockExclusive");
		ReleaseShared       = (SRWLockPtr)    GetProcAddress((HMODULE)kernel32Handle, "ReleaseSRWLockShared");
	}
}

void ReadWriteLock::DecInstances()
{
	if(!--instances)
	{
		FreeLibrary((HMODULE)kernel32Handle);

		kernel32Handle      = nullptr;
		Initialize          = nullptr;
		AcquireExclusive    = nullptr;
		AcquireShared       = nullptr;
		TryAcquireExclusive = nullptr;
		TryAcquireShared    = nullptr;
		ReleaseExclusive    = nullptr;
		ReleaseShared       = nullptr;
	}
}

ReadWriteLock::ReadWriteLock()
{
	IncInstances();
	
	if(Initialize)
		Initialize(&srwLock);
	else
		mutex = new Mutex();
}

ReadWriteLock::~ReadWriteLock()
{
	if(!Initialize)
		delete mutex;

	DecInstances();
}

void ReadWriteLock::AcquireWrite() const
{
	if(AcquireExclusive)
		AcquireExclusive(&srwLock);
	else
		mutex->Acquire();
}

void ReadWriteLock::AcquireRead() const
{
	if(AcquireShared)
		AcquireShared(&srwLock);
	else
		mutex->Acquire();
}

bool ReadWriteLock::TryAcquireWrite() const
{
	if(TryAcquireExclusive)
		return TryAcquireExclusive(&srwLock);
	else
		return mutex->Request();
}

bool ReadWriteLock::TryAcquireRead() const
{
	if(TryAcquireShared)
		return TryAcquireShared(&srwLock);
	else
		return mutex->Request();
}

void ReadWriteLock::ReleaseWrite() const
{
	if(ReleaseExclusive)
		ReleaseExclusive(&srwLock);
	else
		mutex->Release();
}

void ReadWriteLock::ReleaseRead() const
{
	if(ReleaseShared)
		ReleaseShared(&srwLock);
	else
		mutex->Release();
}
