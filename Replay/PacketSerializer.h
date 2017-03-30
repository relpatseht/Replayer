/************************************************************************************\
 * Replay - An Andrew Shurney Production                                         *
\************************************************************************************/

/*! \file		PacketSerializer.h
 *  \author		Andrew Shurney
 *  \brief		A serializer which filters its input data stream by thread id then
 *              any user defined filter.
 */

#ifndef PACKET_SERIALIZER_H
#define PACKET_SERIALIZER_H

#include "ReadWriteLock.h"
#include <unordered_map>
#include "Mutex.h"
#include <deque>

class ThreadManager;

namespace zlib
{
	class CGZip;
}

class PacketSerializer
{
	private:
		struct Packet;

		typedef std::vector<unsigned char> Filter;
		typedef std::unordered_map<unsigned, Filter> ThreadFilters;
		typedef std::unordered_map<unsigned, Packet*> ThreadPackets;

		struct Packet
		{
			unsigned size;
			unsigned char *data;
			unsigned char *curDataPos;
			Packet *prev,*next;

			Packet(const unsigned char *data);
			~Packet();
			bool Empty() const;
		};
		
		ThreadManager& manager;

		ReadWriteLock filterLock;
		ThreadFilters filters;

		ReadWriteLock packetLock;
		ThreadPackets packets;

		ReadWriteLock packetListLock;
		Packet *head, *tail;

		Mutex fileIOLock;
		zlib::CGZip *file;

		void FindPacket();
		bool FindPacketHelper(unsigned threadId, const Filter& filter);
		void EnsureThreadFilterExists();
		void EnsureThreadPacketExists();
		void EnsureBufferLen(unsigned char *&buffer, unsigned char *& curPos, unsigned &bufferSize, unsigned minSize);

		/* Reads from the gzfile packets of data and writes them into the threadFileBuffers
		   map. This assumes the fileIOLock is held by the calling thread.

		   returns false if the file is out of data.
		*/
		bool UpdatePacketList();


		PacketSerializer(const PacketSerializer&);
		PacketSerializer& operator=(const PacketSerializer&);

	public:
		PacketSerializer(ThreadManager &manager, const char *filename);
		~PacketSerializer();

		PacketSerializer& ResetFilter();
		PacketSerializer& AddFilter(const void *buf, unsigned bufSize);
		
		template<typename T>
		PacketSerializer& AddFilter(const T& buf)
		{
			return AddFilter(&buf, sizeof(T));
		}

		PacketSerializer& Read(void *buf, unsigned bufSize);

		template<typename T>
		PacketSerializer& Read(void *buf, T bufSize)
		{
			return Read(buf, static_cast<unsigned>(bufSize));
		}

		template<typename T>
		PacketSerializer& Read(T& buf)
		{
			return Read(&buf, sizeof(T));
		}

		template<typename T>
		T Get()
		{
			T val;
			Read(val);

			return val;
		}
};

extern PacketSerializer *packetSerializer;

#define serializer (&(packetSerializer->ResetFilter().AddFilter(std::hash<const char*>()(__FUNCTION__))))

#endif
