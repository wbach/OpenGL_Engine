#include "XmlConnectionMessageConverter.h"

#include <Logger/Log.h>

#include <algorithm>

#include "MessageTypes.h"
#include "Utils/XML/XmlReader.h"
#include "Utils/XML/XmlWriter.h"
#include "UtilsNetwork/IMessage.h"
#include "UtilsNetwork/MessageFormat.h"
#include "UtilsNetwork/Messages/Connection/AuthenticationMessage.h"
#include "UtilsNetwork/Messages/Connection/ConnectionMessage.h"
#include "UtilsNetwork/Messages/TextMessage.h"
#include "XmlConverterUtils.h"

namespace Network
{
XmlConnectionMessageConverter::XmlConnectionMessageConverter()
{
    /* LOG TO FIX*/  LOG_ERROR << ("");
}

bool XmlConnectionMessageConverter::IsValid(IMessageFormat format, IMessageType type) const
{
    return format == Network::ConvertFormat(Network::MessageFormat::Xml) and type >= MESSAGE_TYPES_RANGE_LOW and
           type <= MESSAGE_TYPES_RANGE_HIGH;
}

std::unique_ptr<IMessage> XmlConnectionMessageConverter::Convert(IMessageType type, const IMessageData& message)
{
    /* LOG TO FIX*/  LOG_ERROR << (Network::Convert(message));

    Utils::XmlReader reader;
    std::string a = Network::Convert(message);
    if (not reader.ReadXml(a))
    {
        return nullptr;
    }

    switch (type)
    {
        case Network::MessageTypes::ConnectionMsg:
        {
            auto result = std::make_unique<ConnectionMessage>();
            auto msg    = reader.Get("ConnectionMessage");

            if (msg and msg->isAttributePresent("connectionStatus"))
            {
                result->connectionStatus = static_cast<uint8>(std::stoi(msg->attributes_.at("connectionStatus")));
                return std::move(result);
            }
            break;
        }
        case Network::MessageTypes::Authentication:
        {
            auto msg = reader.Get("AuthenticationMessage");

            if (msg and msg->isAttributePresent("username") and msg->isAttributePresent("password"))
            {
                auto userName = msg->attributes_.at("username");
                auto password = msg->attributes_.at("password");

                return std::make_unique<AuthenticationMessage>(userName, password);
            }
            break;
        }
        case Network::MessageTypes::Text:
        {
            auto text = reader.Get("TextMessage");

            if (text and text->isAttributePresent("text"))
            {
                auto t = text->attributes_.at("text");
                return std::make_unique<TextMessage>(t);
            }
            break;
        }
        default:
            /* LOG TO FIX*/  LOG_ERROR << ("Convert to IMessage. Unsuporrted message.");
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
    /* LOG TO FIX*/  LOG_ERROR << ("Convert to xml. Unsuporrted message.");
    return {};
}

IMessageData XmlConnectionMessageConverter::ConvertConnectionMessage(const IMessage& message)
{
    auto connectionMessage = castMessageAs<ConnectionMessage>(message);

    TreeNode root("ConnectionMessage");
    root.attributes_.insert({"connectionStatus", std::to_string(connectionMessage->connectionStatus)});

    auto v = CreatePayload(root);
    /* LOG TO FIX*/  LOG_ERROR << (Network::Convert(v));
    return v;
}

IMessageData XmlConnectionMessageConverter::ConvertAuthenticationMessage(const IMessage& message)
{
    auto msg = castMessageAs<AuthenticationMessage>(message);

    TreeNode root("AuthenticationMessage");
    root.attributes_.insert({"username", msg->GetUserName()});
    root.attributes_.insert({"password", msg->GetPassword()});

    auto v = CreatePayload(root);
    /* LOG TO FIX*/  LOG_ERROR << (Network::Convert(v));
    return v;
}

IMessageData XmlConnectionMessageConverter::ConvertTextMessage(const IMessage& message)
{
    auto msg = castMessageAs<TextMessage>(message);

    /* LOG TO FIX*/  LOG_ERROR << ("testShortMessage_ : " + msg->GetText());

    TreeNode root("TextMessage");
    root.attributes_.insert({"text", msg->GetText()});

    auto v = CreatePayload(root);
    /* LOG TO FIX*/  LOG_ERROR << (Network::Convert(v));
    return v;
}

}  // namespace Network
