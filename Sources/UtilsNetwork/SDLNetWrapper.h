#pragma once
#include "Types.h"
#include "ISDLNetWrapper.h"

namespace Network
{
	class SDLNetWrapper : public ISDLNetWrapper
	{
	public:
		virtual int Init() const override;
		virtual SDLNet_SocketSet AllocSocketSet(int maxsockets) const override;
		virtual int ResolveHost(IPaddress *address, const char *host, Uint16 port) const override;
		virtual const char * ResolveIP(IPaddress *address) const override;
		virtual TCPsocket TCPOpen(IPaddress *ip) const override;
		virtual const char* GetError() const override;
		virtual int TCPAddSocket(SDLNet_SocketSet set, TCPsocket sock) const override;
		virtual int SendTcp(TCPsocket sock, const void *data, int len) const override;
		virtual int RecvTcp(TCPsocket sock, void *data, int maxlen) const override;
		virtual int CheckSockets(SDLNet_SocketSet set, Uint32 timeout) const override;
		virtual int SocketReady(SDLNet_GenericSocket sock) const override;
		virtual TCPsocket TCPAccept(TCPsocket server) const override;
		virtual void TCPClose(TCPsocket sock) const override;
	};
}