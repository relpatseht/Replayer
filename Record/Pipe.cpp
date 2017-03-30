/************************************************************************************\
 * Record - An Andrew Shurney Production                                         *
\************************************************************************************/

/*! \file		Pipe.cpp
 *  \author		Andrew Shurney
 *  \brief		Manages a named pipe
 */

#include "Pipe.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

Pipe::Pipe(const std::string& pipeName) : handle(CreateFile((std::string("\\\\.\\pipe\\") + pipeName).c_str(), 
	                                                        GENERIC_WRITE, 0, NULL, OPEN_EXISTING, NULL, NULL))
{

}

Pipe::~Pipe()
{
	CloseHandle(handle);
}

bool Pipe::IsValid() const
{
	return handle != nullptr && handle != INVALID_HANDLE_VALUE;
}

unsigned Pipe::Write(const void* data, unsigned size)
{
	DWORD bytesWritten;
	if(!WriteFile(handle, data, size, &bytesWritten, NULL))
		return 0;

	return bytesWritten;
}