#include "SDLNetWrapper.h"

namespace Network
{
	int SDLNetWrapper::Init() const
	{
		return SDLNet_Init();
	}
	SDLNet_SocketSet SDLNetWrapper::AllocSocketSet(int maxsockets) const
	{
		return SDLNet_AllocSocketSet(maxsockets);
	}
	int SDLNetWrapper::ResolveHost(IPaddress * address, const char * host, Uint16 port) const
	{
		return SDLNet_ResolveHost(address, host, port);
	}
	const char * SDLNetWrapper::ResolveIP(IPaddress * address) const
	{
		return SDLNet_ResolveIP(address);
	}
	TCPsocket SDLNetWrapper::TCPOpen(IPaddress * ip) const
	{
		return SDLNet_TCP_Open(ip);
	}
	const char* SDLNetWrapper::GetError() const
	{
		return SDLNet_GetError();
	}
	int SDLNetWrapper::TCPAddSocket(SDLNet_SocketSet set, TCPsocket sock) const
	{
		return SDLNet_TCP_AddSocket(set, sock);
	}
	int SDLNetWrapper::TCPDeleteSocket(SDLNet_SocketSet set, TCPsocket sock) const
	{
		return SDLNet_TCP_DelSocket(set, sock);
	}
	int SDLNetWrapper::TCPCloseAndDeleteSocket(SDLNet_SocketSet set, TCPsocket sock) const
	{
		TCPClose(sock);
		return SDLNet_TCP_DelSocket(set, sock);
	}
	int SDLNetWrapper::SendTcp(TCPsocket sock, const void * data, int len) const
	{
		return SDLNet_TCP_Send(sock, data, len);
	}
	int SDLNetWrapper::RecvTcp(TCPsocket sock, void * data, int maxlen) const
	{
		return SDLNet_TCP_Recv(sock, data, maxlen);
	}
	int SDLNetWrapper::CheckSockets(SDLNet_SocketSet set, Uint32 timeout) const
	{
		return SDLNet_CheckSockets(set, timeout);
	}
	int SDLNetWrapper::SocketReady(SDLNet_GenericSocket sock) const
	{
		return SDLNet_SocketReady(sock);
	}
	TCPsocket SDLNetWrapper::TCPAccept(TCPsocket server) const
	{
		return SDLNet_TCP_Accept(server);
	}
	void SDLNetWrapper::TCPClose(TCPsocket sock) const
	{
		SDLNet_TCP_Close(sock);
	}
}