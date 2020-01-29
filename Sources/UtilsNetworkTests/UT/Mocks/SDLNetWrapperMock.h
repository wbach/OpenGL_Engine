#pragma once
#include <gmock/gmock.h>
#include "../UtilsNetwork/ISDLNetWrapper.h"

namespace Network
{
class SDLNetWrapperMock : public ISDLNetWrapper
{
public:
    MOCK_CONST_METHOD0(Init, int());
    MOCK_CONST_METHOD1(AllocSocketSet, SDLNet_SocketSet(int maxsockets));
    MOCK_CONST_METHOD3(ResolveHost, int(IPaddress *address, const char *host, Uint16 port));
    MOCK_CONST_METHOD1(ResolveIP, const char *(IPaddress *address));
    MOCK_CONST_METHOD1(TCPOpen, TCPsocket(IPaddress *ip));
    MOCK_CONST_METHOD0(GetError, const char *());
    MOCK_CONST_METHOD2(TCPAddSocket, int(SDLNet_SocketSet set, TCPsocket sock));
    MOCK_CONST_METHOD2(TCPDeleteSocket, int(SDLNet_SocketSet set, TCPsocket sock));
    MOCK_CONST_METHOD2(TCPCloseAndDeleteSocket, int(SDLNet_SocketSet set, TCPsocket sock));
    MOCK_METHOD3(SendTcp, bool(TCPsocket sock, const void *data, int len));
    MOCK_METHOD3(RecvTcp, bool(TCPsocket sock, void *data, int maxlen));
    MOCK_CONST_METHOD2(CheckSockets, int(SDLNet_SocketSet set, Uint32 timeout));
    MOCK_CONST_METHOD1(SocketReady, int(SDLNet_GenericSocket sock));
    MOCK_CONST_METHOD1(TCPAccept, TCPsocket(TCPsocket server));
    MOCK_CONST_METHOD1(TCPClose, void(TCPsocket sock));
    MOCK_CONST_METHOD0(GetReceviedBytesPerSecond, uint64());
    MOCK_CONST_METHOD0(GetSentBytesPerSecond, uint64());
    MOCK_METHOD0(PerSecondTick, void());
};
}  // namespace Network
