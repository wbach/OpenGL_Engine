#include "XMLMessageConverter.h"

#include <Utils/XML/XmlReader.h>
#include <Utils/XML/XmlWriter.h>
#include <UtilsNetwork/MessageFormat.h>
#include <UtilsNetwork/Messages/XmlConverterUtils.h>

#include "MessageTypes.h"
#include "Messages/GameObjectMsg.h"

namespace GameEngine
{
XmlMessageConverter::XmlMessageConverter()
{
}

bool XmlMessageConverter::IsValid(Network::IMessageFormat format, Network::IMessageType type) const
{
    return format == Network::ConvertFormat(Network::MessageFormat::Xml) and
           type >= static_cast<uint8>(NETWORK_INTERFACE_MESSAGE_TYPE_RANGE_LOW) and type <= static_cast<uint8>(NETWORK_INTERFACE_TYPE_RANGE_HIGH);
}

std::unique_ptr<Network::IMessage> XmlMessageConverter::Convert(Network::IMessageType, const Network::IMessageData&)
{
    return {};
}

Network::IMessageData XmlMessageConverter::Convert(const Network::IMessage& message)
{
    switch (static_cast<MessageTypes>(message.GetType()))
    {
        case GameEngine::MessageTypes::GameObject:
        {
            auto msg = Network::castMessageAs<GameObjectMsg>(message);

            Utils::XmlNode root("GameObject");
            root.attributes_.insert({"id", std::to_string(msg->id)});
            root.attributes_.insert({"parentId", std::to_string(msg->parentId)});
            root.attributes_.insert({"name", msg->GetName()});
            DEBUG_LOG(Utils::Xml::Write(root));
            auto v = Network::CreatePayload(root);
            // DEBUG_LOG(Network::Convert(v));
            return v;
        }
        break;
    }
    return {};
}
}  // namespace GameEngine
