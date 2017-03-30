/************************************************************************************\
 * Replay - An Andrew Shurney Production                                         *
\************************************************************************************/

/*! \file		PacketSerializer.cpp
 *  \author		Andrew Shurney
 *  \brief		A serializer which filters its input data stream by thread id then
 *              any user defined filter.
 */

#include "ThreadManager.h"
#include "PacketSerializer.h"
#include "SharedLock.h"
#include "ExclusiveLock.h"
#include "FuncHookerFactory.h"
#pragma warning(disable:4091)
#include "gzip.h"
#pragma warning(default:4091)

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

PacketSerializer::PacketSerializer(ThreadManager &manager, const char *filename) : manager(manager), 
	                                                                               head(nullptr),
	                                                                               file(new zlib::CGZip())
{
	if(!file->Open(filename, zlib::CZipper::ArchiveModeRead))
		throw std::exception();
}

PacketSerializer::~PacketSerializer()
{
	Packet *curNode = head;
	while(curNode)
	{
		Packet *next = curNode->next;
		delete curNode;
		curNode = next;
	}
}

PacketSerializer& PacketSerializer::ResetFilter()
{
	EnsureThreadFilterExists();
	unsigned threadId = manager.GetCurrentThreadId();

	{
		SharedLock lock(filterLock);
		Filter &filter = filters[threadId];

		filter.clear();
		filter.insert(filter.begin(), reinterpret_cast<unsigned char*>(&threadId), reinterpret_cast<unsigned char*>(&threadId+1));
	}

	return *this;
}

PacketSerializer& PacketSerializer::AddFilter(const void *buf, unsigned bufSize)
{
	EnsureThreadFilterExists();

	unsigned threadId = manager.GetCurrentThreadId();

	{
		SharedLock lock(filterLock);
		Filter &filter = filters[threadId];

		const unsigned char *bufBytes = reinterpret_cast<const unsigned char*>(buf);
		filter.insert(filter.end(), bufBytes, bufBytes+bufSize);
	}

	return *this;
}

PacketSerializer& PacketSerializer::Read(void *buf, unsigned bufSize)
{
	unsigned threadId = manager.GetCurrentThreadId();

	SharedLock lock(packetLock);
	Packet *& packet = packets[threadId];

	if(!packet)
		FindPacket();

	if(!packet)
		throw std::exception();

	std::memcpy(buf, packet->curDataPos, bufSize);
	packet->curDataPos += bufSize;

	if(packet->Empty())
	{		
		delete packet;
		packet = nullptr;
	}

	return *this;
}

void PacketSerializer::FindPacket()
{
	unsigned threadId = manager.GetCurrentThreadId();

	EnsureThreadPacketExists();

	filterLock.AcquireRead();
	const Filter filter = filters[threadId];
	filterLock.ReleaseRead();

	static const unsigned MAX_SEARCH_CALLS = 4;
	bool found = false;
	for(unsigned i=0; i<MAX_SEARCH_CALLS; ++i)
	{
		if(FindPacketHelper(threadId, filter))
		{
			found = true;
			break;
		}
		else if(!UpdatePacketList())
			break;
	}
	
	if(!found)
		FindPacketHelper(threadId, filter);
}

bool PacketSerializer::FindPacketHelper(unsigned threadId, const Filter& filter)
{
	SharedLock lock(packetLock);
	Packet *& packet = packets[threadId];

	ExclusiveLock listLock(packetListLock);
	if(!packet)
		packet = head;

	while(packet)
	{
		if(packet->size >= filter.size() && std::memcmp(packet->data, &filter[0], filter.size()) == 0)
		{
			packet->curDataPos += filter.size();

			if(packet == head)
				head = packet->next;
			else
				packet->prev->next = packet->next;

			if(packet == tail)
				tail = packet->prev;
			else
				packet->next->prev = packet->prev;

			packet->next = packet->prev = nullptr;

			break;
		}

		packet = packet->next;
	}

	return packet != nullptr;
}

void PacketSerializer::EnsureThreadFilterExists()
{
	unsigned threadId = manager.GetCurrentThreadId();

	filterLock.AcquireRead();
	auto it = filters.find(threadId);
	filterLock.ReleaseRead();

	if(it == filters.end())
	{
		filterLock.AcquireWrite();
		filters.insert(std::make_pair(threadId, Filter()));
		filterLock.ReleaseWrite();
	}
}

void PacketSerializer::EnsureThreadPacketExists()
{
	unsigned threadId = manager.GetCurrentThreadId();

	packetLock.AcquireRead();
	auto it = packets.find(threadId);
	packetLock.ReleaseRead();

	if(it == packets.end())
	{
		packetLock.AcquireWrite();
		packets.insert(std::make_pair(threadId, nullptr));
		packetLock.ReleaseWrite();
	}
}

void PacketSerializer::EnsureBufferLen(unsigned char *&buffer, unsigned char *& curPos, unsigned &bufferSize, unsigned minSize)
{
	if(bufferSize < minSize)
	{
		unsigned char *tmpBuffer = new unsigned char[minSize];
		std::memcpy(tmpBuffer, curPos, bufferSize);

		delete [] buffer;
		buffer = tmpBuffer;

		bufferSize += file->ReadBufferSize(buffer + bufferSize, minSize - bufferSize);
		curPos = buffer;
	}
}


template<typename T>
static T ReadFromBuffer(unsigned char *& curPos, unsigned &bufferSize)
{
	T val = *reinterpret_cast<const T*>(curPos);
	curPos += sizeof(T);
	bufferSize -= sizeof(T);

	return val;
}

bool PacketSerializer::UpdatePacketList()
{
	if(file->IsEOF())
		return false;

	if(!fileIOLock.Request())
	{
		fileIOLock.Acquire();
		fileIOLock.Release();
		return true;
	}

	unsigned char *buffer = new unsigned char[1024];
	unsigned bufferSize = file->ReadBufferSize(buffer, sizeof(unsigned char)*1024);

	unsigned char *curPos = buffer;
	while(bufferSize > 0)
	{
		EnsureBufferLen(buffer, curPos, bufferSize, sizeof(unsigned short));

		unsigned short packetSize = *reinterpret_cast<unsigned short*>(curPos);
		EnsureBufferLen(buffer, curPos, bufferSize, packetSize);
		
		Packet *newPacket = new Packet(curPos);
		curPos += packetSize;
		bufferSize -= packetSize;

		{
			ExclusiveLock listLock(packetListLock);

			if(!head)
				head = tail = newPacket;
			else
			{
				tail->next = newPacket;
				newPacket->prev = tail;

				tail = newPacket;
			}
		}
	}

	delete [] buffer;

	fileIOLock.Release();

	return true;
}

PacketSerializer::Packet::Packet(const unsigned char *pData) : size(*reinterpret_cast<const unsigned short*>(pData) - sizeof(unsigned short)), 
	                                                           data(new unsigned char[size]), 
															   curDataPos(data), 
															   prev(nullptr), next(nullptr)
{
	std::memcpy(this->data, pData + sizeof(unsigned short), size);
}

PacketSerializer::Packet::~Packet()
{
	delete [] data;
}

bool PacketSerializer::Packet::Empty() const
{
	return curDataPos >= (data + size);
}