/************************************************************************************\
 * Replay - An Andrew Shurney Production                                         *
\************************************************************************************/

/*! \file		WinSockHooks.cpp
 *  \author		Andrew Shurney
 *  \brief		Hooks winsock functions
 */

#include "WinSockHooks.h"
#include "FuncHookerFactory.h"
#include "PacketSerializer.h"
#include "ReadWriteLock.h"
#include "SharedLock.h"
#include "ExclusiveLock.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WinSock2.h>

#pragma warning (disable : 4100)

void *acceptPtr = NULL;
void *bindPtr = NULL;
void *connectPtr = NULL;
void *listenPtr = NULL;
void *recvPtr = NULL;
void *recvfromPtr = NULL;
void *selectPtr = NULL;
void *sendPtr = NULL;
void *sendtoPtr = NULL;
void *socketPtr = NULL;
void *WSAGetLastErrorPtr = NULL;
void *shutdownPtr = NULL;
void *closesocketPtr = NULL;

SOCKET PASCAL acceptHook(__in SOCKET s,
	                     __out_bcount_opt(*addrlen) struct sockaddr *addr,
	                     __inout_opt int *addrlen)
{
	PacketSerializer *ser = &serializer->AddFilter(s);

	Sleep(ser->Get<unsigned>());

	SOCKET ret;
	ser->Read(ret);

	if(addrlen)
		ser->Read(addr, *addrlen);

	return ret;
}

int PASCAL bindHook(__in SOCKET s,
	                __in_bcount(namelen) const struct sockaddr *addr,
	                __in int namelen)
{
	return serializer->AddFilter(s).Get<int>();
}

int PASCAL connectHook(__in SOCKET s,
	                   __in_bcount(namelen) const struct sockaddr *name,
	                   __in int namelen)
{
	PacketSerializer *ser = &serializer->AddFilter(s);

	Sleep(ser->Get<unsigned>());

	return ser->Get<int>();
}

int PASCAL listenHook(__in SOCKET s,
	                  __in int backlog)
{
	return serializer->AddFilter(s).Get<int>();
}

int PASCAL recvHook(__in SOCKET s,
	                __out_bcount_part(len, return) __out_data_source(NETWORK) char * buf,
	                __in int len,
	                __in int flags)
{
	PacketSerializer *ser = &serializer->AddFilter(s);

	Sleep(ser->Get<unsigned>());

	int ret = ser->Get<int>();

	if(ret > 0)
		ser->Read(buf, ret);

	return ret;
}

int PASCAL recvfromHook(__in SOCKET s,
	                    __out_bcount_part(len, return) __out_data_source(NETWORK) char * buf,
	                    __in int len,
	                    __in int flags,
	                    __out_bcount_opt(*fromlen) struct sockaddr * from,
	                    __inout_opt int * fromlen)
{
	Sleep(serializer->Get<unsigned>());

	int ret;
	serializer->Read(ret);

	if(ret > 0)
		serializer->Read(buf, ret);

	if(fromlen)
		serializer->Read(from, *fromlen);

	return ret;
}

int PASCAL selectHook(__in int nfds,
	                  __inout_opt fd_set *readfds,
	                  __inout_opt fd_set *writefds,
	                  __inout_opt fd_set *exceptfds,
	                  __in_opt  const struct timeval *timeout)
{
	Sleep(serializer->Get<unsigned>());

	int ret;
	serializer->Read(ret);

	if(readfds)
		serializer->Read(*readfds);

	if(writefds)
		serializer->Read(*writefds);

	if(exceptfds)
		serializer->Read(*exceptfds);

	return ret;
}

int PASCAL sendHook(__in SOCKET s,
	                __in_bcount(len) const char * buf,
	                __in int len,
	                __in int flags)
{
	PacketSerializer *ser = &serializer->AddFilter(s);

	Sleep(ser->Get<unsigned>());

	return ser->Get<int>();
}

int PASCAL sendtoHook(__in SOCKET s,
	                  __in_bcount(len) const char * buf,
	                  __in int len,
	                  __in int flags,
	                  __in_bcount_opt(tolen) const struct sockaddr *to,
	                  __in int tolen)
{
	PacketSerializer *ser = &serializer->AddFilter(s);

	Sleep(ser->Get<unsigned>());

	return ser->Get<int>();
}

SOCKET PASCAL socketHook(__in int af,
	                     __in int type,
	                     __in int protocol)
{
	return serializer->Get<SOCKET>();
}

int PASCAL WSAGetLastErrorHook(void)
{
	return serializer->Get<int>();
}

int PASCAL shutdownHook(__in SOCKET s,
                        __in int how)
{
	return serializer->AddFilter(s).Get<int>();
}

int PASCAL closesocketHook(__in SOCKET s)
{
	return serializer->AddFilter(s).Get<int>();
}

bool InstallWinSockHooks()
{
	bool ret = true;

#define INSTALL_HOOK(FUNC_NAME)													              \
	auto FUNC_NAME ## Hooker = CreateFuncHooker(#FUNC_NAME, FUNC_NAME ## Hook, "Ws2_32.dll"); \
	FUNC_NAME ## Ptr = FUNC_NAME ## Hooker;                                                   \
	if(!(FUNC_NAME ## Hooker) || !(FUNC_NAME ## Hooker)->InstallHook())			              \
	ret &= false;

	INSTALL_HOOK(accept)
	INSTALL_HOOK(bind)
	INSTALL_HOOK(connect)
	INSTALL_HOOK(listen)
	INSTALL_HOOK(recv)
	INSTALL_HOOK(recvfrom)
	INSTALL_HOOK(select)
	INSTALL_HOOK(send)
	INSTALL_HOOK(sendto)
	INSTALL_HOOK(socket)
	INSTALL_HOOK(WSAGetLastError)
	INSTALL_HOOK(shutdown)
	INSTALL_HOOK(closesocket)

	return ret;
}
