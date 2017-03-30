/************************************************************************************\
 * Record - An Andrew Shurney Production                                         *
\************************************************************************************/

/*! \file		FuncCallSerializer.h
 *  \author		Andrew Shurney
 *  \brief		Serializes a function call, sending it over a pipe
 */

#ifndef FUNC_CALL_SERIALIZER_H
#define FUNC_CALL_SERIALIZER_H

#include <vector>
#include "ReadWriteLock.h"
#include <unordered_map>

class ThreadManager;
class Pipe;

class FuncCallSerializer
{
	private:
		typedef unsigned char byte;
		typedef std::vector<byte> DataVec;
		typedef std::unordered_map<unsigned, DataVec> ThreadDataVecMap;

		ReadWriteLock funcCallsRW;
		ThreadDataVecMap threadFuncCalls;

		const ThreadManager &threadManager;
		Pipe &pipe;

		FuncCallSerializer& operator=(const FuncCallSerializer&); // do not implement
		FuncCallSerializer(const FuncCallSerializer&);            // do not implement

		void EnsureInitialized();
		unsigned GetCurSize() const;
		void AddToCall(const void *data, unsigned size);

	public:
		FuncCallSerializer(const ThreadManager& tm, Pipe& p);

		FuncCallSerializer& Push(const void *data, unsigned size);
		FuncCallSerializer& Prepend(const void *data, unsigned size);

		template<typename T>
		FuncCallSerializer& Push(const T& data)
		{
			return Push(&data, sizeof(T));
		}

		template<typename T>
		FuncCallSerializer& Prepend(const T& data)
		{
			return Prepend(&data, sizeof(T));
		}

		void Submit(const char *funcName);
};

#define Finalize() Submit(__FUNCTION__)

extern FuncCallSerializer *serializer;

#endif
