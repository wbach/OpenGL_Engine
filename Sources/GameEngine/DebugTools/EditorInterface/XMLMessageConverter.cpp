#include "XMLMessageConverter.h"

#include <Utils.h>
#include <Utils/XML/XmlReader.h>
#include <Utils/XML/XmlWriter.h>
#include <UtilsNetwork/MessageFormat.h>
#include <UtilsNetwork/Messages/XmlConverterUtils.h>

#include "MessageTypes.h"
#include "Messages/NewComponentMsgInd.h"
#include "Messages/NewGameObjectIndMsg.h"
#include "Messages/TransformMsg.h"
#include "Messages/RemoveComponentMsgInd.h"
#include "Messages/RemoveGameObjectIndMsg.h"

namespace GameEngine
{
XmlMessageConverter::XmlMessageConverter()
{
}

bool XmlMessageConverter::IsValid(Network::IMessageFormat format, Network::IMessageType type) const
{
    return format == Network::ConvertFormat(Network::MessageFormat::Xml) and
           type >= static_cast<uint8>(NETWORK_INTERFACE_MESSAGE_TYPE_RANGE_LOW) and
           type <= static_cast<uint8>(NETWORK_INTERFACE_TYPE_RANGE_HIGH);
}

std::unique_ptr<Network::IMessage> XmlMessageConverter::Convert(Network::IMessageType, const Network::IMessageData&)
{
    return {};
}

Network::IMessageData XmlMessageConverter::Convert(const Network::IMessage& message)
{
    switch (static_cast<MessageTypes>(message.GetType()))
    {
        case GameEngine::MessageTypes::NewGameObjectInd:
        {
            auto msg = Network::castMessageAs<NewGameObjectIndMsg>(message);

            Utils::XmlNode root("NewGameObjectInd");
            root.attributes_.insert({"id", std::to_string(msg->id)});
            root.attributes_.insert({"parentId", std::to_string(msg->parentId)});
            root.attributes_.insert({"name", msg->GetName()});
            DEBUG_LOG(Utils::Xml::Write(root));
            auto v = Network::CreatePayload(root);
            return v;
        }
        case GameEngine::MessageTypes::Transform:
        {
            auto msg = Network::castMessageAs<TransformMsg>(message);
            Utils::XmlNode root("Transform");
            root.attributes_.insert({"id", std::to_string(msg->objectId_)});

            auto& position = root.AddChild("position");
            position.attributes_.insert({"x", std::to_string(msg->position_.x)});
            position.attributes_.insert({"y", std::to_string(msg->position_.y)});
            position.attributes_.insert({"z", std::to_string(msg->position_.z)});

            auto& rotation = root.AddChild("rotation");
            rotation.attributes_.insert({"x", std::to_string(msg->rotation_.x)});
            rotation.attributes_.insert({"y", std::to_string(msg->rotation_.y)});
            rotation.attributes_.insert({"z", std::to_string(msg->rotation_.z)});

            auto& scale = root.AddChild("scale");
            scale.attributes_.insert({"x", std::to_string(msg->scale_.x)});
            scale.attributes_.insert({"y", std::to_string(msg->scale_.y)});
            scale.attributes_.insert({"z", std::to_string(msg->scale_.z)});

            DEBUG_LOG(Utils::Xml::Write(root));
            auto v = Network::CreatePayload(root);
            return v;
        }
        case GameEngine::MessageTypes::NewComponentMsgInd:
        {
            auto msg = Network::castMessageAs<NewComponentMsgInd>(message);
            Utils::XmlNode root("NewComponentMsgInd");
            root.attributes_.insert({"name", msg->GetName()});
            root.attributes_.insert({"isActive", Utils::BoolToString(msg->isActive_)});
            DEBUG_LOG(Utils::Xml::Write(root));
            auto v = Network::CreatePayload(root);
            return v;
        }
        default:
            break;
    }
    return {};
}
}  // namespace GameEngine
