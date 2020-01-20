#pragma once
#include "ISDLNetWrapper.h"
#include "Types.h"

namespace Network
{
class SDLNetWrapper : public ISDLNetWrapper
{
public:
    SDLNetWrapper();
    virtual int Init() const override;
    virtual SDLNet_SocketSet AllocSocketSet(int maxsockets) const override;
    virtual int ResolveHost(IPaddress *address, const char *host, Uint16 port) const override;
    virtual const char *ResolveIP(IPaddress *address) const override;
    virtual TCPsocket TCPOpen(IPaddress *ip) const override;
    virtual const char *GetError() const override;
    virtual int TCPAddSocket(SDLNet_SocketSet set, TCPsocket sock) const override;
    virtual int TCPDeleteSocket(SDLNet_SocketSet set, TCPsocket sock) const override;
    virtual int TCPCloseAndDeleteSocket(SDLNet_SocketSet set, TCPsocket sock) const override;
    virtual bool SendTcp(TCPsocket sock, const void *data, int len) override;
    virtual bool RecvTcp(TCPsocket sock, void *data, int maxlen) override;
    virtual int CheckSockets(SDLNet_SocketSet set, Uint32 timeout) const override;
    virtual int SocketReady(SDLNet_GenericSocket sock) const override;
    virtual TCPsocket TCPAccept(TCPsocket server) const override;
    virtual void TCPClose(TCPsocket sock) const override;
    virtual uint64 GetReceviedBytesPerSecond() const override;
    virtual uint64 GetSentBytesPerSecond() const override;
    virtual void PerSecondTick() override;

private:
    uint64 receviedBytes_;
    uint64 sentBytes_;
};
}  // namespace Network
