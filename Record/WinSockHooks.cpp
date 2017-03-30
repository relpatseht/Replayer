/************************************************************************************\
 * Record - An Andrew Shurney Production                                         *
\************************************************************************************/

/*! \file		WinSockHooks.cpp
 *  \author		Andrew Shurney
 *  \brief		WinSockHooks hooks winsock functions.
 */

#include "WinSockHooks.h"
#include "FuncHookerFactory.h"
#include "FuncCallSerializer.h"
#include "Timer.h"
#include <unordered_map>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WinSock2.h>

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
	auto acceptHooker = FuncHookerCast(acceptPtr, acceptHook);

	Timer t;
	SOCKET ret = acceptHooker->CallInjectee(s, addr, addrlen);

	serializer->Push(s).Push(t.Elapsed()).Push(ret);

	if(addrlen)
		serializer->Push(addr, *addrlen);
	
	serializer->Finalize();

	return ret;
}

int PASCAL bindHook(__in SOCKET s,
	                __in_bcount(namelen) const struct sockaddr *addr,
	                __in int namelen)
{
	auto bindHooker = FuncHookerCast(bindPtr, bindHook);

	int ret = bindHooker->CallInjectee(s, addr, namelen);

	serializer->Push(s).Push(ret).Finalize();

	return ret;
}

int PASCAL connectHook(__in SOCKET s,
	                   __in_bcount(namelen) const struct sockaddr *name,
	                   __in int namelen)
{
	auto connectHooker = FuncHookerCast(connectPtr, connectHook);

	Timer t;
	int ret = connectHooker->CallInjectee(s, name, namelen);
	serializer->Push(s).Push(t.Elapsed()).Push(ret).Finalize();

	return ret;
}

int PASCAL listenHook(__in SOCKET s,
	                  __in int backlog)
{
	auto listenHooker = FuncHookerCast(listenPtr, listenHook);

	int ret = listenHooker->CallInjectee(s, backlog);

	serializer->Push(s).Push(ret).Finalize();

	return ret;
}

int PASCAL recvHook(__in SOCKET s,
	                __out_bcount_part(len, return) __out_data_source(NETWORK) char * buf,
	                __in int len,
	                __in int flags)
{
	auto recvHooker = FuncHookerCast(recvPtr, recvHook);

	Timer t;
	int ret = recvHooker->CallInjectee(s, buf, len, flags);
	serializer->Push(s).Push(t.Elapsed()).Push(ret);

	if(ret > 0)
		serializer->Push(buf, ret);

	serializer->Finalize();

	return ret;
}

int PASCAL recvfromHook(__in SOCKET s,
	                    __out_bcount_part(len, return) __out_data_source(NETWORK) char * buf,
	                    __in int len,
	                    __in int flags,
	                    __out_bcount_opt(*fromlen) struct sockaddr * from,
	                    __inout_opt int * fromlen)
{
	auto recvfromHooker = FuncHookerCast(recvfromPtr, recvfromHook);

	Timer t;
	int ret = recvfromHooker->CallInjectee(s, buf, len, flags, from, fromlen);
	serializer->Push(s).Push(t.Elapsed()).Push(ret);

	if(ret > 0)
		serializer->Push(buf, ret);

	if(fromlen)
		serializer->Push(from, *fromlen);

	serializer->Finalize();

	return ret;
}

int PASCAL selectHook(__in int nfds,
	                  __inout_opt fd_set *readfds,
	                  __inout_opt fd_set *writefds,
	                  __inout_opt fd_set *exceptfds,
	                  __in_opt  const struct timeval *timeout)
{
	auto selectHooker = FuncHookerCast(selectPtr, selectHook);

	Timer t;
	int ret = selectHooker->CallInjectee(nfds, readfds, writefds, exceptfds, timeout);
	serializer->Push(t.Elapsed()).Push(ret);

	if(readfds)
		serializer->Push(*readfds);

	if(writefds)
		serializer->Push(*writefds);

	if(exceptfds)
		serializer->Push(*exceptfds);

	serializer->Finalize();

	return ret;
}

int PASCAL sendHook(__in SOCKET s,
	                __in_bcount(len) const char * buf,
	                __in int len,
	                __in int flags)
{
	auto sendHooker = FuncHookerCast(sendPtr, sendHook);

	Timer t;
	int ret = sendHooker->CallInjectee(s, buf, len, flags);
	serializer->Push(s).Push(t.Elapsed()).Push(ret).Finalize();

	return ret;
}

int PASCAL sendtoHook(__in SOCKET s,
	                  __in_bcount(len) const char * buf,
	                  __in int len,
	                  __in int flags,
	                  __in_bcount_opt(tolen) const struct sockaddr *to,
	                  __in int tolen)
{
	auto sendtoHooker = FuncHookerCast(sendtoPtr, sendtoHook);

	Timer t;
	int ret = sendtoHooker->CallInjectee(s, buf, len, flags, to, tolen);
	serializer->Push(s).Push(t.Elapsed()).Push(ret).Finalize();

	return ret;
}

SOCKET PASCAL socketHook(__in int af,
	                     __in int type,
	                     __in int protocol)
{
	auto socketHooker = FuncHookerCast(socketPtr, socketHook);

	SOCKET ret = socketHooker->CallInjectee(af, type, protocol);

	serializer->Push(ret).Finalize();

	return ret;
}

int PASCAL WSAGetLastErrorHook(void)
{
	auto WSAGetLastErrorHooker = FuncHookerCast(WSAGetLastErrorPtr, WSAGetLastErrorHook);

	int ret = WSAGetLastErrorHooker->CallInjectee();

	serializer->Push(ret).Finalize();

	return ret;
}

int PASCAL shutdownHook(__in SOCKET s,
                        __in int how)
{
	auto shutdownHooker = FuncHookerCast(shutdownPtr, shutdownHook);

	int ret = shutdownHooker->CallInjectee(s, how);

	serializer->Push(s).Push(ret).Finalize();

	return ret;
}

int PASCAL closesocketHook(__in SOCKET s)
{
	auto closesocketHooker = FuncHookerCast(closesocketPtr, closesocketHook);

	int ret = closesocketHooker->CallInjectee(s);

	serializer->Push(s).Push(ret).Finalize();

	return ret;
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
