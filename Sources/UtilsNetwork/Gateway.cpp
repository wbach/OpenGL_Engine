#include "Gateway.h"

#include <SDL2/SDL_net.h>

#include <algorithm>
#include <chrono>
#include <functional>

#include "IMessage.h"
#include "Logger/Log.h"
#include "SDLNetWrapper.h"
#include "Thread.hpp"

#include "Messages/BinaryConnectionMessageConverter.h"
#include "Messages/XmlConnectionMessageConverter.h"

namespace Network
{
Gateway::Gateway()
    : Gateway(Utils::Time::CTimeMeasurer(120, false))
{
}

Gateway::Gateway(Utils::Time::CTimeMeasurer timeMeasurer)
    : timeMeasurer_(timeMeasurer)
    , sdlNetWrapper_(std::make_unique<SDLNetWrapper>())
    , sender_(*sdlNetWrapper_, messageConverters_)
    , receiver_(*sdlNetWrapper_, messageConverters_)
    , connectionManager_(sender_, receiver_, *sdlNetWrapper_, context_)
    , serverCreator_(*sdlNetWrapper_)
    , clientCreator_(sender_, receiver_, *sdlNetWrapper_)
    , running_(false)
    , idPool_(0)
{
    messageConverters_.push_back(std::make_unique<XmlConnectionMessageConverter>());
    messageConverters_.push_back(std::make_unique<BinaryConnectionMessageConverter>());
}

Gateway::~Gateway()
{
    DEBUG_LOG("Disconnecting users");
    for (auto& user : context_.users)
        DisconnectUser(user.second->id);
}

bool Gateway::StartServer(uint32 maxClients, uint32 port)
{
    DEBUG_LOG("");
    auto context = serverCreator_.Create(maxClients, port);

    if (not context)
    {
        DEBUG_LOG("Server creation failed.");
        return false;
    }

    context_           = *context;
    context_.isServer_ = true;
    running_           = true;

    return true;
}
bool Gateway::ConnectToServer(const std::string& username, const std::string& password, const std::string& host,
                              uint32 port)
{
    DEBUG_LOG("");

    auto op_context = clientCreator_.ConnectToServer(username, password, host, port);

    if (!op_context)
        return false;

    context_ = op_context.value();

    // from client pov is only one user - server
    context_.users[0]         = std::make_shared<UtilsNetwork::UserData>();
    context_.users[0]->socket = context_.socket;

    running_ = true;
    return true;
}

void Gateway::Update()
{
    if (not running_)
        return;

    if (!connectionManager_.CheckSocketsActivity())
        return;

    if (context_.isServer_)
        connectionManager_.CheckNewConnectionsToServer();

    for (auto iter = context_.users.begin(); iter != context_.users.end();)
    {
        auto& user = *iter;

        auto [status, msg] = receiver_.Receive(user.second->socket);

        if (status == RecvStatus::Disconnect)
        {
            DEBUG_LOG("Disconnect : " + std::to_string(user.second->id));
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
                sub.second(user.first, std::move(msg));
            }
        }
    }
}

void Gateway::SetDefaultMessageConverterFormat(MessageFormat format)
{
    sender_.SetMessageFormat(format);
}

void Gateway::AddMessageConverter(std::unique_ptr<IMessageConverter> converter)
{
    messageConverters_.push_back(std::move(converter));
}

bool Gateway::Send(uint32 userId, const IMessage& message)
{
    if (context_.users.count(userId) == 0)
    {
        DEBUG_LOG("User not found: Id : " + std::to_string(userId));
        return false;
    }

    auto i = sender_.SendTcp(context_.users.at(userId)->socket, message);
    ;

    if (i == SentStatus::UNDEFINED_ERROR)
    {
        DisconnectUser(userId);
        context_.users.erase(userId);
    }

    return i == SentStatus::OK;
}

bool Gateway::Send(const IMessage& message)
{
    return Send(0, message);
}

void Gateway::DisconnectUser(uint32 id)
{
    connectionManager_.DisconectUser(id);
    for (auto& sub : disconnectSubscribes_)
        sub(id);
}

void Gateway::SubscribeForNewUser(CreationFunc func)
{
    connectionManager_.SubscribeForNewUser(func);
}

uint32 Gateway::SubscribeOnMessageArrived(uint8 messageType, OnMessageArrived func)
{
    auto id = idPool_++;
    onMessageArrivedSubcribes_[messageType].push_back({id, func});
    return id;
}
void Gateway::UnsubscribeAllOnMessageArrived()
{
    onMessageArrivedSubcribes_.clear();
}
void Gateway::UnsubscrieOnMessageArrived(uint32 id)
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
void Gateway::SubscribeForDisconnectUser(DisconectFunc func)
{
    disconnectSubscribes_.push_back(func);
}
}  // namespace Network
