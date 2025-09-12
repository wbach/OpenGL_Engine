#include "ClientCreator.h"

#include "Logger/Log.h"
#include "Messages/Connection/AuthenticationMessage.h"
#include "Messages/Connection/ConnectionMessage.h"
#include "Utils.h"

namespace Network
{
ClientCreator::ClientCreator(Sender& sender, Receiver& receiver, ISDLNetWrapper& sdlNetWrapper)
    : NetworkCreator(sdlNetWrapper)
    , sender_(sender)
    , receiver_(receiver)
{
}
#define IfThenReturn(x) \
    if (x)   \
        return   \
        {        \
        }

std::optional<ConectContext> ClientCreator::ConnectToServer(const std::string& username, const std::string& password,
                                                            const std::string& host, uint32 port)
{
    context_.port       = port;
    context_.serverName = host;

    IfThenReturn(not Init());
    IfThenReturn(not AllocSocketSet(1));
    IfThenReturn(not ResolveHost(context_.serverName.c_str()));
    IfThenReturn(not ResolveIp());
    IfThenReturn(not OpenTcp());
    IfThenReturn(not AddSocketTcp());
    IfThenReturn(WaitForAcceptConnection() != ClientCreator::WAIT_FOR_AUTHENTICATION);
    IfThenReturn(not WaitForAuthentication(username, password));

    isCreated = true;
    return context_;
}

#undef IfNotReturn

ClientCreator::ConnectionState ClientCreator::WaitForAcceptConnection()
{
    sdlNetWrapper_.CheckSockets(context_.socketSet, 5000);

    auto [status, msg] = receiver_.Receive(context_.socket);

    if (msg == nullptr)
        return ClientCreator::NOT_CONNECTED;

    auto connectingMsg = GetAndValidateConnectionMessage(msg.get());
    if (connectingMsg == nullptr)
        return ClientCreator::NOT_CONNECTED;

    if (connectingMsg->connectionStatus == ConnectionStatus::CONNECTED)
    {
        /* LOG TO FIX*/  LOG_ERROR << ("Connected to server.");
        return ClientCreator::CONNECTED;
    }

    if (connectingMsg->connectionStatus != ConnectionStatus::WAIT_FOR_AUTHENTICATION)
        return ClientCreator::NOT_CONNECTED;

    /* LOG TO FIX*/  LOG_ERROR << ("Wait for authentication...");
    return ClientCreator::WAIT_FOR_AUTHENTICATION;
}

bool ClientCreator::WaitForAuthentication(const std::string& username, const std::string& password)
{
    AuthenticationMessage msg(username, password);

    sender_.SendTcp(context_.socket, msg);

    /* LOG TO FIX*/  LOG_ERROR << ("Sent authenticationMessage");

    sdlNetWrapper_.CheckSockets(context_.socketSet, 5000);

    auto [status, recvMsg] = receiver_.Receive(context_.socket);

    if (recvMsg == nullptr)
        return false;

    auto connectingMsg = GetAndValidateConnectionMessage(recvMsg.get());
    if (connectingMsg == nullptr)
        return false;

    if (connectingMsg->connectionStatus == ConnectionStatus::CONNECTED)
    {
        /* LOG TO FIX*/  LOG_ERROR << ("Joining server now...");
        return true;
    }

    /* LOG TO FIX*/  LOG_ERROR << ("Authentication failed. Wrong username or password.");
    return false;
}

ConnectionMessage* ClientCreator::GetAndValidateConnectionMessage(IMessage* msg)
{
    if (msg == nullptr)
    {
        /* LOG TO FIX*/  LOG_ERROR << ("[ClientCreator::GetAndValidateConnectionMessage] Recv nullptr msg.");
        return nullptr;
    }

    if (msg->GetType() != MessageTypes::ConnectionMsg)
    {
        /* LOG TO FIX*/  LOG_ERROR << ("[ClientCreator::GetAndValidateConnectionMessage] Unsupported msg recv.");
        return nullptr;
    }

    auto connectingMsg = dynamic_cast<ConnectionMessage*>(msg);

    if (connectingMsg == nullptr)
    {
        /* LOG TO FIX*/  LOG_ERROR << (
            "[ClientCreator::GetAndValidateConnectionMessage] Something went wrong. Couldn't cast to "
            "ConnectionMessage*.");
        return nullptr;
    }

    if (connectingMsg->connectionStatus == ConnectionStatus::ERROR_FULL)
    {
        /* LOG TO FIX*/  LOG_ERROR << ("Not connected. Server is full.");
        return nullptr;
    }

    return connectingMsg;
}

}  // namespace Network
