#include "XMLMessageConverter.h"
#include <Utils/XML/XmlReader.h>
#include <Utils/XML/XmlWriter.h>
#include <UtilsNetwork/MessageFormat.h>
#include "MessageTypes.h"
#include "Messages/GameObjectMsg.h"

namespace GameEngine
{
XmlMessageConverter::XmlMessageConverter()
{
}

bool XmlMessageConverter::IsValid(Network::IMessageFormat format, Network::IMessageType type) const
{
    return format == Network::ConvertFormat(Network::MessageFormat::Xml) and type >= NETWORK_INTERFACE_MESSAGE_TYPE_RANGE_LOW and type <= NETWORK_INTERFACE_TYPE_RANGE_HIGH;
}

std::unique_ptr<Network::IMessage> XmlMessageConverter::Convert(Network::IMessageType, const Network::IMessageData&)
{
    return {};
}

Network::IMessageData CreatePayload(Utils::XmlNode& root)
{
    auto s = Utils::Xml::Write(root);
    std::vector<int8> v;
    std::copy(s.begin(), s.end(), std::back_inserter(v));
    v.push_back('\0');
    v.push_back(';');
    return v;
}

Network::IMessageData XmlMessageConverter::Convert(const Network::IMessage& message)
{
    switch (message.GetType())
    {
        case GameEngine::MessageTypes::GameObject:
        {
            auto msg = Network::castMessageAs<GameObjectMsg>(message);

            Utils::XmlNode root("GameObject");
            root.attributes_.insert({"id", std::to_string(msg->id)});
            root.attributes_.insert({"parentId", std::to_string(msg->parentId)});
            root.attributes_.insert({"name", msg->GetName()});
            DEBUG_LOG(Utils::Xml::Write(root));
            auto v = CreatePayload(root);
            //DEBUG_LOG(Network::Convert(v));
            return v;
        }
        break;
    }
    return {};
}
}  // namespace GameEngine
