#include "Gateway.h"
#include <SDL2/SDL_net.h>
#include <chrono>
#include <functional>
#include "Logger/Log.h"
#include "IMessage.h"
#include "SDLNetWrapper.h"
#include "Thread.hpp"

namespace Network
{
CGateway::CGateway()
    : CGateway(Utils::Time::CTimeMeasurer(120, false))
{
}

CGateway::CGateway(Utils::Time::CTimeMeasurer timeMeasurer)
    : timeMeasurer_(timeMeasurer)
    , sdlNetWrapper_(std::make_unique<SDLNetWrapper>())
    , sender_(*sdlNetWrapper_, messageConverters_)
    , receiver_(*sdlNetWrapper_, messageConverters_)
    , connectionManager_(sender_, receiver_, *sdlNetWrapper_, context_)
    , clientCreator_(sender_, receiver_, *sdlNetWrapper_)
    , isServer(false)
    , running(false)
{
}

CGateway::~CGateway()
{
}

void CGateway::StartServer(uint32 maxClients, uint32 port)
{
    context_ = serverCreator_.Create(maxClients, port);
    isServer = true;
    running  = true;
}
bool CGateway::ConnectToServer(const std::string& username, const std::string& password, const std::string& host, uint32 port)
{
    auto op_context = clientCreator_.ConnectToServer(username, password, host, port);

    if (!op_context)
        return false;

    context_ = op_context.value();

    // from client pov is only one user - server
    context_.users[0]         = std::make_shared<UtilsNetwork::UserData>();
    context_.users[0]->socket = context_.socket;

    running = true;
    return true;
}

void CGateway::Update()
{
    if (!running)
        return;

    timeMeasurer_.StartFrame();

    if (!connectionManager_.CheckSocketsActivity())
        return;

    if (isServer)
        connectionManager_.CheckNewConnectionsToServer();

    for (auto iter = context_.users.begin(); iter != context_.users.end();)
    {
        auto& user = *iter;

        auto [ status, msg ]      = receiver_.Receive(user.second->socket);

        if (status == RecvStatus::Disconnect)
        {
            DisconnectUser(user.second->id);
            iter = context_.users.erase(iter);
            continue;
        }
        else
        {
            ++iter;
        }

        if (status == RecvStatus::NotReady or not msg)
            continue;

        for (auto& sub : onMessageArrivedSubcribes_)
        {
            auto subscribedType = sub.second.first;
            auto subscribedFunc = sub.second.second;

            if (subscribedType == msg->GetType())
                subscribedFunc({user.first, std::move(msg)});
        }
    }
    timeMeasurer_.EndFrame();
}

bool CGateway::Send(uint32 userId, IMessage* message)
{
    auto i = sender_.SendTcp(context_.users[userId]->socket, message);
    ;

    if (i == SentStatus::ERROR)
    {
        DisconnectUser(userId);
        context_.users.erase(userId);
    }

    return i == SentStatus::OK;
}
bool CGateway::Send(IMessage* message)
{
    return Send(0, message);
}

void CGateway::DisconnectUser(uint32 id)
{
    connectionManager_.DisconectUser(id);
    for (auto& sub : disconnectSubscribes_)
        sub(id);
}

void CGateway::SubscribeForNewUser(CreationFunc func)
{
    connectionManager_.SubscribeForNewUser(func);
}

void CGateway::SubscribeOnMessageArrived(const std::string& label, OnMessageArrived func, uint8 type)
{
    onMessageArrivedSubcribes_[label] = {type, func};
}
void CGateway::UnsubscribeAllOnMessageArrived()
{
    onMessageArrivedSubcribes_.clear();
}
void CGateway::UnsubscrieOnMessageArrived(const std::string& label)
{
    onMessageArrivedSubcribes_.erase(label);
}
void CGateway::SubscribeForDisconnectUser(DisconectFunc func)
{
    disconnectSubscribes_.push_back(func);
}
}  // namespace Network
