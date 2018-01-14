#pragma once
#include "Types.h"
#include <SDL2/SDL_net.h>

namespace Network
{
	class ISDLNetWrapper
	{
	public:
		virtual ~ISDLNetWrapper() {}
		virtual int Init() const = 0;
		virtual SDLNet_SocketSet AllocSocketSet(int maxsockets) const = 0;
		virtual int ResolveHost(IPaddress *address, const char *host, Uint16 port) const = 0;
		virtual const char * ResolveIP(IPaddress *address) const = 0;
		virtual TCPsocket TCPOpen(IPaddress *ip) const = 0;
		virtual const char* GetError() const = 0;
		virtual int TCPAddSocket(SDLNet_SocketSet set, TCPsocket sock) const = 0;
		virtual int SendTcp(TCPsocket sock, const void *data, int len) const = 0;
		virtual int RecvTcp(TCPsocket sock, void *data, int maxlen) const = 0;
		virtual int CheckSockets(SDLNet_SocketSet set, Uint32 timeout) const = 0;
		virtual int SocketReady(SDLNet_GenericSocket sock) const = 0;
		virtual TCPsocket TCPAccept(TCPsocket server) const = 0;
		virtual void TCPClose(TCPsocket sock) const = 0;
	};
}