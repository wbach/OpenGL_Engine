#include "ClientCreator.h"

#include "Logger/Log.h"
#include "Messages/Conntection/AuthenticationMessage.h"
#include "Messages/Conntection/ConnectionMessage.h"
#include "Utils.h"

namespace Network
{
ClientCreator::ClientCreator(Sender& sender, Receiver& receiver, ISDLNetWrapper& sdlNetWrapper)
    : sdlNetWrapper_(sdlNetWrapper)
    , sender_(sender)
    , receiver_(receiver)
{
}
#define IfNotReturn(x) \
    if (not x)   \
        return   \
        {        \
        }

std::optional<ConectContext> ClientCreator::ConnectToServer(const std::string& username, const std::string& password,
                                                            const std::string& host, uint32 port)
{
    context_.port       = port;
    context_.serverName = host;

    IfNotReturn(Init());
    IfNotReturn(AllocSocketSet(1));
    IfNotReturn(ResolveHost(context_.serverName.c_str()));
    IfNotReturn(ResolveIp());
    IfNotReturn(OpenTcp());
    IfNotReturn(AddSocketTcp());
    IfNotReturn(WaitForAcceptConnection() != ClientCreator::WAIT_FOR_AUTHENTICATION);
    IfNotReturn(WaitForAuthentication(username, password));

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
        DEBUG_LOG("Joining server now...");
        return ClientCreator::CONNECTED;
    }

    if (connectingMsg->connectionStatus != ConnectionStatus::WAIT_FOR_AUTHENTICATION)
        return ClientCreator::NOT_CONNECTED;

    DEBUG_LOG("Wait for authentication...");
    return ClientCreator::WAIT_FOR_AUTHENTICATION;
}

bool ClientCreator::WaitForAuthentication(const std::string& username, const std::string& password)
{
    AuthenticationMessage msg(username, password);

    sender_.SendTcp(context_.socket, msg);

    sdlNetWrapper_.CheckSockets(context_.socketSet, 5000);

    auto [status, recvMsg] = receiver_.Receive(context_.socket);

    if (recvMsg == nullptr)
        return false;

    auto connectingMsg = GetAndValidateConnectionMessage(recvMsg.get());
    if (connectingMsg == nullptr)
        return false;

    if (connectingMsg->connectionStatus == ConnectionStatus::CONNECTED)
    {
        DEBUG_LOG("Joining server now...");
        return true;
    }

    DEBUG_LOG("Authentication failed. Wrong username or password.");
    return false;
}

ConnectionMessage* ClientCreator::GetAndValidateConnectionMessage(IMessage* msg)
{
    if (msg == nullptr)
    {
        ERROR_LOG("[ClientCreator::GetAndValidateConnectionMessage] Recv nullptr msg.");
        return nullptr;
    }

    if (msg->GetType() != MessageTypes::ConnectionMsg)
    {
        ERROR_LOG("[ClientCreator::GetAndValidateConnectionMessage] Unsupported msg recv.");
        return nullptr;
    }

    auto connectingMsg = dynamic_cast<ConnectionMessage*>(msg);

    if (connectingMsg == nullptr)
    {
        ERROR_LOG(
            "[ClientCreator::GetAndValidateConnectionMessage] Something went wrong. Couldn't cast to "
            "ConnectionMessage*.");
        return nullptr;
    }

    if (connectingMsg->connectionStatus == ConnectionStatus::ERROR_FULL)
    {
        ERROR_LOG("Not connected. Server is full.");
        return nullptr;
    }

    return connectingMsg;
}

}  // namespace Network
