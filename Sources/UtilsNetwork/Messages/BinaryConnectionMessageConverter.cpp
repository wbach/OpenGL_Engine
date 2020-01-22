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
    DEBUG_LOG(Network::Convert(message));

    switch (type)
    {
            //        case Network::MessageTypes::ConnectionMsg:
            //        {
            //        }
            //        case Network::MessageTypes::Authentication:
            //        {
            //        }
            //        case Network::MessageTypes::Text:
            //        {
            //        }
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
            return ConvertConnectionMessage(message);
        case Network::MessageTypes::Authentication:
            return ConvertAuthenticationMessage(message);
        case Network::MessageTypes::Text:
            return ConvertTextMessage(message);
    }

    DEBUG_LOG("Convert to binary. Unsuporrted message.");
    return {};
}

std::vector<int8> BinaryConnectionMessageConverter::ConvertConnectionMessage(const IMessage& message)
{
    return {};
}

std::vector<int8> BinaryConnectionMessageConverter::ConvertAuthenticationMessage(const IMessage& message)
{
    return {};
}

std::vector<int8> BinaryConnectionMessageConverter::ConvertTextMessage(const IMessage& message)
{
    return {};
}

}  // namespace Network
