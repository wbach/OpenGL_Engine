#include "BinaryMessageConverter.h"

namespace common
{
BinaryMessageConverter::BinaryMessageConverter()
{
}

bool BinaryMessageConverter::IsValid(Network::IMessageFormat format, Network::IMessageType type) const
{
    return false;
}

std::unique_ptr<Network::IMessage> BinaryMessageConverter::Convert(Network::IMessageType type, const Network::IMessageData &message)
{
    return nullptr;
}

Network::IMessageData BinaryMessageConverter::Convert(const Network::IMessage &message)
{
    return {};
}
}  // namespace common
