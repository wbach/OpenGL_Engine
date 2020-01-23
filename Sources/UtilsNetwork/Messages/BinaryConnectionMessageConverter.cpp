#include "BinaryConnectionMessageConverter.h"
#include <Logger/Log.h>
#include "MessageFormat.h"
#include "MessageTypes.h"
#include "Messages/Connection/AuthenticationMessage.h"
#include "Messages/Connection/ConnectionMessage.h"
#include "Messages/TextMessage.h"
#include "Utils/XML/XmlReader.h"
#include "Utils/XML/XmlWriter.h"
#include "UtilsNetwork/IMessage.h"

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
uint8 BinaryConnectionMessageConverter::GetFormat() const
{
    return ConvertFormat(MessageFormat::Binary);
}
std::unique_ptr<IMessage> BinaryConnectionMessageConverter::Convert(uint8 type, const std::vector<int8>& message)
{
    switch (type)
    {
        case Network::MessageTypes::ConnectionMsg:
            return ConvertMessage<ConnectionMessage>(message);
        case Network::MessageTypes::Authentication:
            return ConvertMessage<AuthenticationMessage>(message);
        case Network::MessageTypes::Text:
            return ConvertMessage<TextMessage>(message);
        default:
            DEBUG_LOG("Convert to IMessage. Unsuporrted message.");
    }

    return nullptr;
}

std::vector<int8> BinaryConnectionMessageConverter::Convert(const IMessage& message)
{
    switch (message.GetType())
    {
        case Network::MessageTypes::ConnectionMsg:
            return ConvertMessage<ConnectionMessage>(message);
        case Network::MessageTypes::Authentication:
            return ConvertMessage<AuthenticationMessage>(message);
        case Network::MessageTypes::Text:
            return ConvertMessage<TextMessage>(message);
    }

    DEBUG_LOG("Convert to binary. Unsuporrted message.");
    return {};
}

template <class T>
std::unique_ptr<IMessage> BinaryConnectionMessageConverter::ConvertMessage(const std::vector<int8>& message)
{
    auto msg = std::make_unique<T>();
    memcpy(msg.get(), &message[0], message.size());
    return std::move(msg);
}

template <class T>
std::vector<int8> BinaryConnectionMessageConverter::ConvertMessage(const IMessage& message)
{
    std::vector<int8> result;
    result.resize(sizeof(T));
    memcpy(&result[0], &message, sizeof(T));
    result.push_back(';');
    return result;
}

}  // namespace Network
