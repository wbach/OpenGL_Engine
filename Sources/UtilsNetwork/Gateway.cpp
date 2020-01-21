#include "Gateway.h"

#include <SDL2/SDL_net.h>

#include <algorithm>
#include <chrono>
#include <functional>

#include "IMessage.h"
#include "Logger/Log.h"
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
    , idPool_(0)
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
bool CGateway::ConnectToServer(const std::string& username, const std::string& password, const std::string& host,
                               uint32 port)
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

        auto [status, msg] = receiver_.Receive(user.second->socket);

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

        if (onMessageArrivedSubcribes_.count(msg->GetType()) > 0)
        {
            auto& v = onMessageArrivedSubcribes_.at(msg->GetType());

            for (auto& sub : v)
            {
                sub.second(std::move(msg));
            }
        }
    }
    timeMeasurer_.EndFrame();
}

bool CGateway::Send(uint32 userId, IMessage& message)
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
bool CGateway::Send(IMessage& message)
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

uint32 CGateway::SubscribeOnMessageArrived(uint8 messageType, OnMessageArrived func)
{
    auto id = idPool_++;
    onMessageArrivedSubcribes_[messageType].push_back({id, func});
    return id;
}
void CGateway::UnsubscribeAllOnMessageArrived()
{
    onMessageArrivedSubcribes_.clear();
}
void CGateway::UnsubscrieOnMessageArrived(uint32 id)
{
    for (auto& types : onMessageArrivedSubcribes_)
    {
        auto& v = types.second;

        auto iter = std::find_if(v.begin(), v.end(), [id](auto& pair) { return id == pair.first; });

        if (iter != v.end())
        {
            v.erase(iter);
        }
    }
}
void CGateway::SubscribeForDisconnectUser(DisconectFunc func)
{
    disconnectSubscribes_.push_back(func);
}
}  // namespace Network
