#pragma once
#include <memory>
#include <vector>
#include "UtilsNetwork/IMessage.h"

namespace Network
{
template <class T>
std::unique_ptr<Network::IMessage> ConvertMessage(const Network::IMessageData& message)
{
    auto msg = std::make_unique<T>();
    memcpy(msg.get(), &message[0], message.size());
    return std::move(msg);
}

template <class T>
Network::IMessageData ConvertMessage(const Network::IMessage& message)
{
    std::vector<int8> result;
    result.resize(sizeof(T));
    memcpy(&result[0], &message, sizeof(T));
    result.push_back(';');
    return result;
}
}  // namespace Network
