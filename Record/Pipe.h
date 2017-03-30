/************************************************************************************\
 * Record - An Andrew Shurney Production                                         *
\************************************************************************************/

/*! \file		Pipe.h
 *  \author		Andrew Shurney
 *  \brief		Manages a named pipe
 */

#ifndef PIPE_H
#define PIPE_H

#include <string>

class Pipe
{
	private:
		void *handle;

	public:
		Pipe(const std::string& pipeName);
		~Pipe();

		bool IsValid() const;

		unsigned Write(const void *data, unsigned size);

		template<typename T>
		unsigned Write(const T& data)
		{
			return Write(&data, sizeof(T));
		}
};

#endif
