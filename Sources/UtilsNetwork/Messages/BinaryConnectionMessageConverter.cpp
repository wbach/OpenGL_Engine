#include "BinaryConnectionMessageConverter.h"
#include <Logger/Log.h>
#include "UtilsNetwork/MessageFormat.h"
#include "MessageTypes.h"
#include "UtilsNetwork/Messages/Connection/AuthenticationMessage.h"
#include "UtilsNetwork/Messages/Connection/ConnectionMessage.h"
#include "UtilsNetwork/Messages/TextMessage.h"
#include "Utils/XML/XmlReader.h"
#include "Utils/XML/XmlWriter.h"
#include "UtilsNetwork/IMessage.h"
#include "UtilsNetwork/PayloadUtils.h"

namespace Network
{
namespace
{
std::string Convert(const std::vector<int8>& message)
{
    std::string result;
    std::copy(message.begin(), message.end(), std::back_inserter(result));
    return result;
}
}  // namespace

BinaryConnectionMessageConverter::BinaryConnectionMessageConverter()
{
    DEBUG_LOG("");
}

bool BinaryConnectionMessageConverter::IsValid(IMessageFormat format, IMessageType type) const
{
    return format == Network::ConvertFormat(Network::MessageFormat::Binary) and type >= MESSAGE_TYPES_RANGE_LOW and type <= MESSAGE_TYPES_RANGE_HIGH;
}

std::unique_ptr<IMessage> BinaryConnectionMessageConverter::Convert(IMessageType type, const IMessageData& message)
{
    switch (type)
    {
        case Network::MessageTypes::ConnectionMsg:
            return ConvertMessage<Network::ConnectionMessage>(message);
        case Network::MessageTypes::Authentication:
            return ConvertMessage<Network::AuthenticationMessage>(message);
        case Network::MessageTypes::Text:
            return ConvertMessage<Network::TextMessage>(message);
        default:
            DEBUG_LOG("Convert to IMessage. Unsuporrted message.");
    }

    return nullptr;
}

IMessageData BinaryConnectionMessageConverter::Convert(const IMessage& message)
{
    switch (message.GetType())
    {
        case Network::MessageTypes::ConnectionMsg:
            return ConvertMessage<Network::ConnectionMessage>(message);
        case Network::MessageTypes::Authentication:
            return ConvertMessage<Network::AuthenticationMessage>(message);
        case Network::MessageTypes::Text:
            return ConvertMessage<Network::TextMessage>(message);
    }

    DEBUG_LOG("Convert to binary. Unsuporrted message.");
    return {};
}
}  // namespace Network
