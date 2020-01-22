#pragma once
#include "NetworkCreator.h"
#include "Reciever.h"
#include "Sender.h"
#include "optional.hpp"

namespace Network
{
struct ConnectionMessage;
class IMessage;

class ClientCreator : public NetworkCreator
{
    enum ConnectionState
    {
        NOT_CONNECTED,
        CONNECTED,
        WAIT_FOR_AUTHENTICATION
    };

public:
    ClientCreator(Sender& sender, Receiver& receiver, ISDLNetWrapper& sdlNetWrapper);
    std::optional<ConectContext> ConnectToServer(const std::string& username, const std::string& password, const std::string& host, uint32 port);

private:
    ConnectionState WaitForAcceptConnection();
    ConnectionMessage* GetAndValidateConnectionMessage(IMessage* msg);
    bool WaitForAuthentication(const std::string& username, const std::string& password);

private:
    Sender& sender_;
    Receiver& receiver_;
};
}  // namespace Network
