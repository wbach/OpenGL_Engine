#include "SDLNetWrapper.h"

namespace Network
{
struct SDLNetWrapper::Impl
{
    IPaddress* ip_{nullptr};
};

SDLNetWrapper::SDLNetWrapper()
    : receviedBytes_{0}
    , sentBytes_{0}
    , impl_(std::make_unique<Impl>())
{
}

SDLNetWrapper::~SDLNetWrapper()
{
    SDLNet_Quit();
}

int SDLNetWrapper::Init() const
{
    return SDLNet_Init();
}
SDLNet_SocketSet SDLNetWrapper::AllocSocketSet(int maxsockets) const
{
    return SDLNet_AllocSocketSet(maxsockets);
}
int SDLNetWrapper::ResolveHost(IPaddress* address, const char* host, Uint16 port) const
{
    return SDLNet_ResolveHost(address, host, port);
}
const char* SDLNetWrapper::ResolveIP(IPaddress* address) const
{
    return SDLNet_ResolveIP(address);
}
TCPsocket SDLNetWrapper::TCPOpen(IPaddress* ip) const
{
    impl_->ip_ = ip;
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
bool SDLNetWrapper::SendTcp(TCPsocket sock, const void* data, int len)
{
    auto bytes = SDLNet_TCP_Send(sock, data, len);
    if (bytes > 0)
    {
        sentBytes_ += static_cast<uint64>(bytes);
        return true;
    }

    return false;
}
bool SDLNetWrapper::RecvTcp(TCPsocket sock, void* data, int maxlen)
{
    auto bytes =  SDLNet_TCP_Recv(sock, data, maxlen);
    if (bytes > 0)
    {
        receviedBytes_ += static_cast<uint64>(bytes);
        return true;
    }
    return false;
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

uint64 SDLNetWrapper::GetReceviedBytesPerSecond() const
{
    return receviedBytes_;
}

uint64 SDLNetWrapper::GetSentBytesPerSecond() const
{
    return sentBytes_;
}

void SDLNetWrapper::PerSecondTick()
{
    receviedBytes_ = 0;
    sentBytes_     = 0;
}
}  // namespace Network
