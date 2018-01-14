#pragma once
#include "../UtilsNetwork/ISDLNetWrapper.h"
#include <gmock/gmock.h>

namespace Network
{
	class SDLNetWrapperMock : public ISDLNetWrapper
	{
	public:
		MOCK_CONST_METHOD0(Init, int());
		MOCK_CONST_METHOD1(AllocSocketSet, SDLNet_SocketSet(int maxsockets));
		MOCK_CONST_METHOD3(ResolveHost, int(IPaddress *address, const char *host, Uint16 port));
		MOCK_CONST_METHOD1(TCPOpen, TCPsocket(IPaddress *ip));
		MOCK_CONST_METHOD0(GetError, const char*());
		MOCK_CONST_METHOD1(ResolveIP, const char*(IPaddress *address));
		MOCK_CONST_METHOD2(TCPAddSocket, int(SDLNet_SocketSet set, TCPsocket sock));
		MOCK_CONST_METHOD3(SendTcp, int(TCPsocket sock, const void *data, int len));
		MOCK_CONST_METHOD3(RecvTcp, int(TCPsocket sock, void *data, int maxlen));		
		MOCK_CONST_METHOD2(CheckSockets, int(SDLNet_SocketSet set, Uint32 timeout));
		MOCK_CONST_METHOD1(SocketReady, int(SDLNet_GenericSocket sock));
		MOCK_CONST_METHOD1(TCPAccept, TCPsocket(TCPsocket server));
	};
} //Network