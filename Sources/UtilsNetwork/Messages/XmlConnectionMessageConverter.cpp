#include "XmlConnectionMessageConverter.h"
#include <Logger/Log.h>
#include "UtilsNetwork/MessageFormat.h"
#include "MessageTypes.h"
#include "UtilsNetwork/Messages/Connection/AuthenticationMessage.h"
#include "UtilsNetwork/Messages/Connection/ConnectionMessage.h"
#include "UtilsNetwork/Messages/TextMessage.h"
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

XmlConnectionMessageConverter::XmlConnectionMessageConverter()
{
    DEBUG_LOG("");
}

bool XmlConnectionMessageConverter::IsValid(IMessageFormat format, IMessageType type) const
{
    return format == Network::ConvertFormat(Network::MessageFormat::Xml) and type >= MESSAGE_TYPES_RANGE_LOW and type <= MESSAGE_TYPES_RANGE_HIGH;
}

std::unique_ptr<IMessage> XmlConnectionMessageConverter::Convert(IMessageType type, const IMessageData& message)
{
    Utils::XmlReader reader;
    std::string a = Network::Convert(message);
    reader.ReadXml(a);
    DEBUG_LOG(Network::Convert(message));

    switch (type)
    {
        case Network::MessageTypes::ConnectionMsg:
        {
            auto result              = std::make_unique<ConnectionMessage>();
            result->connectionStatus = std::stoi(reader.Get("ConnectionMessage")->attributes_.at("connectionStatus"));
            return std::move(result);
        }
        case Network::MessageTypes::Authentication:
        {
            auto msg = reader.Get("AuthenticationMessage");

            if (not msg and (not msg->IsAttributeExist("username") or not msg->IsAttributeExist("password")))
            {
                DEBUG_LOG("AuthenticationMessage : format error");
                break;
            }

            auto userName = msg->attributes_.at("username");
            auto password = msg->attributes_.at("password");

            return std::make_unique<AuthenticationMessage>(userName, password);
        }
        case Network::MessageTypes::Text:
        {
            auto text = reader.Get("TextMessage")->attributes_.at("text");
            return std::make_unique<TextMessage>(text);
        }
        default:
            DEBUG_LOG("Convert to IMessage. Unsuporrted message.");
    }

    return nullptr;
}
IMessageData XmlConnectionMessageConverter::Convert(const IMessage& message)
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
    //  XML
    DEBUG_LOG("Convert to xml. Unsuporrted message.");
    return {};
}

IMessageData CreatePayload(Utils::XmlNode& root)
{
    auto s = Utils::Xml::Write(root);
    std::vector<int8> v;
    std::copy(s.begin(), s.end(), std::back_inserter(v));
    v.push_back('\0');
    v.push_back(';');
    return v;
}

IMessageData XmlConnectionMessageConverter::ConvertConnectionMessage(const IMessage& message)
{
    auto connectionMessage = castMessageAs<ConnectionMessage>(message);

    Utils::XmlNode root("ConnectionMessage");
    root.attributes_.insert({"connectionStatus", std::to_string(connectionMessage->connectionStatus)});

    auto v = CreatePayload(root);
    DEBUG_LOG(Network::Convert(v));
    return v;
}

IMessageData XmlConnectionMessageConverter::ConvertAuthenticationMessage(const IMessage& message)
{
    auto msg = castMessageAs<AuthenticationMessage>(message);

    Utils::XmlNode root("AuthenticationMessage");
    root.attributes_.insert({"username", msg->GetUserName()});
    root.attributes_.insert({"password", msg->GetPassword()});

    auto v = CreatePayload(root);
    DEBUG_LOG(Network::Convert(v));
    return v;
}

IMessageData XmlConnectionMessageConverter::ConvertTextMessage(const IMessage& message)
{
    auto msg = castMessageAs<TextMessage>(message);

    Utils::XmlNode root("TextMessage");
    root.attributes_.insert({"text", msg->GetText()});

    auto v = CreatePayload(root);
    DEBUG_LOG(Network::Convert(v));
    return v;
}

}  // namespace Network
