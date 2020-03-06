#include "XmlMessageConverter.h"
#include <UtilsNetwork/MessageFormat.h>
#include <UtilsNetwork/Messages/XmlConverterUtils.h>
#include "MessageTypes.h"
#include "AvailableComponentMsgIndXmlSerializer.h"
#include "AvailableComponentMsgIndXmlDeserializer.h"
#include "CameraMsgXmlSerializer.h"
#include "CameraMsgXmlDeserializer.h"
#include "ComponentDataMessageXmlSerializer.h"
#include "ComponentDataMessageXmlDeserializer.h"
#include "NewComponentMsgIndXmlSerializer.h"
#include "NewComponentMsgIndXmlDeserializer.h"
#include "NewGameObjectIndXmlSerializer.h"
#include "NewGameObjectIndXmlDeserializer.h"
#include "ParamXmlSerializer.h"
#include "ParamXmlDeserializer.h"
#include "RemoveComponentMsgIndXmlSerializer.h"
#include "RemoveComponentMsgIndXmlDeserializer.h"
#include "RemoveGameObjectIndXmlSerializer.h"
#include "RemoveGameObjectIndXmlDeserializer.h"
#include "TransformXmlSerializer.h"
#include "TransformXmlDeserializer.h"

namespace GameEngine
{
namespace DebugNetworkInterface
{
bool XmlMessageConverter::IsValid(Network::IMessageFormat format, Network::IMessageType type) const
{
    return format == Network::ConvertFormat(Network::MessageFormat::Xml) and
        type >= GameEngine::DebugNetworkInterface::MIN_VALUE and
        type <= GameEngine::DebugNetworkInterface::MAX_VALUE;
}
std::unique_ptr<Network::IMessage> XmlMessageConverter::Convert(Network::IMessageType messageType, const Network::IMessageData& message)
{
    DEBUG_LOG(Network::Convert(message));
    Utils::XmlReader reader;
    std::string convertedMessage = Network::Convert(message);
    if (not reader.ReadXml(convertedMessage)) return nullptr;
    auto type = DebugNetworkInterface::Convert(messageType);
    if (not type) return nullptr;
    switch (*type)
    {
    case MessageTypes::AvailableComponentMsgInd: return DeserializeAvailableComponentMsgInd(reader);
    case MessageTypes::CameraMsg: return DeserializeCameraMsg(reader);
    case MessageTypes::ComponentDataMessage: return DeserializeComponentDataMessage(reader);
    case MessageTypes::NewComponentMsgInd: return DeserializeNewComponentMsgInd(reader);
    case MessageTypes::NewGameObjectInd: return DeserializeNewGameObjectInd(reader);
    case MessageTypes::Param: return DeserializeParam(reader);
    case MessageTypes::RemoveComponentMsgInd: return DeserializeRemoveComponentMsgInd(reader);
    case MessageTypes::RemoveGameObjectInd: return DeserializeRemoveGameObjectInd(reader);
    case MessageTypes::Transform: return DeserializeTransform(reader);
    }
    return nullptr;
}
Network::IMessageData XmlMessageConverter::Convert(const Network::IMessage& message)
{
    auto type = DebugNetworkInterface::Convert(message.GetType());
    if (not type) return {};
    switch (*type)
    {
    case MessageTypes::AvailableComponentMsgInd:
    {
        auto msg = Network::castMessageAs<AvailableComponentMsgInd>(message);
        return Serialize(*msg);
    }
    case MessageTypes::CameraMsg:
    {
        auto msg = Network::castMessageAs<CameraMsg>(message);
        return Serialize(*msg);
    }
    case MessageTypes::ComponentDataMessage:
    {
        auto msg = Network::castMessageAs<ComponentDataMessage>(message);
        return Serialize(*msg);
    }
    case MessageTypes::NewComponentMsgInd:
    {
        auto msg = Network::castMessageAs<NewComponentMsgInd>(message);
        return Serialize(*msg);
    }
    case MessageTypes::NewGameObjectInd:
    {
        auto msg = Network::castMessageAs<NewGameObjectInd>(message);
        return Serialize(*msg);
    }
    case MessageTypes::Param:
    {
        auto msg = Network::castMessageAs<Param>(message);
        return Serialize(*msg);
    }
    case MessageTypes::RemoveComponentMsgInd:
    {
        auto msg = Network::castMessageAs<RemoveComponentMsgInd>(message);
        return Serialize(*msg);
    }
    case MessageTypes::RemoveGameObjectInd:
    {
        auto msg = Network::castMessageAs<RemoveGameObjectInd>(message);
        return Serialize(*msg);
    }
    case MessageTypes::Transform:
    {
        auto msg = Network::castMessageAs<Transform>(message);
        return Serialize(*msg);
    }
    }
    return {};
}
} // namespace DebugNetworkInterface
} // namespace GameEngine
