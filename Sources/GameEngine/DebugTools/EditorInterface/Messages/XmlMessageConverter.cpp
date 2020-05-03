#include "XmlMessageConverter.h"
#include <UtilsNetwork/MessageFormat.h>
#include <UtilsNetwork/Messages/XmlConverterUtils.h>
#include "MessageTypes.h"
#include "LightDeletedXmlSerializer.h"
#include "LightDeletedXmlDeserializer.h"
#include "RemoveComponentMsgIndXmlSerializer.h"
#include "RemoveComponentMsgIndXmlDeserializer.h"
#include "SelectedObjectChangedXmlSerializer.h"
#include "SelectedObjectChangedXmlDeserializer.h"
#include "PlantBrushEnabledXmlSerializer.h"
#include "PlantBrushEnabledXmlDeserializer.h"
#include "CameraMsgXmlSerializer.h"
#include "CameraMsgXmlDeserializer.h"
#include "TerrainPainterEnabledXmlSerializer.h"
#include "TerrainPainterEnabledXmlDeserializer.h"
#include "NewLightXmlSerializer.h"
#include "NewLightXmlDeserializer.h"
#include "GameObjectRenamedXmlSerializer.h"
#include "GameObjectRenamedXmlDeserializer.h"
#include "RemoveGameObjectIndXmlSerializer.h"
#include "RemoveGameObjectIndXmlDeserializer.h"
#include "TransformXmlSerializer.h"
#include "TransformXmlDeserializer.h"
#include "SceneStopedNotifMsgXmlSerializer.h"
#include "SceneStopedNotifMsgXmlDeserializer.h"
#include "ComponentDataMessageXmlSerializer.h"
#include "ComponentDataMessageXmlDeserializer.h"
#include "SceneFileMsgXmlSerializer.h"
#include "SceneFileMsgXmlDeserializer.h"
#include "NewComponentMsgIndXmlSerializer.h"
#include "NewComponentMsgIndXmlDeserializer.h"
#include "SceneStartedNotifMsgXmlSerializer.h"
#include "SceneStartedNotifMsgXmlDeserializer.h"
#include "AvailableComponentMsgIndXmlSerializer.h"
#include "AvailableComponentMsgIndXmlDeserializer.h"
#include "NewGameObjectIndXmlSerializer.h"
#include "NewGameObjectIndXmlDeserializer.h"
#include "ReloadSceneXmlSerializer.h"
#include "ReloadSceneXmlDeserializer.h"
#include "ExitRespXmlSerializer.h"
#include "ExitRespXmlDeserializer.h"
#include "ParamXmlSerializer.h"
#include "ParamXmlDeserializer.h"
#include "ComponentDeletedXmlSerializer.h"
#include "ComponentDeletedXmlDeserializer.h"
#include "GameObjectDeletedXmlSerializer.h"
#include "GameObjectDeletedXmlDeserializer.h"

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
    case MessageTypes::LightDeleted: return DeserializeLightDeleted(reader);
    case MessageTypes::RemoveComponentMsgInd: return DeserializeRemoveComponentMsgInd(reader);
    case MessageTypes::SelectedObjectChanged: return DeserializeSelectedObjectChanged(reader);
    case MessageTypes::PlantBrushEnabled: return DeserializePlantBrushEnabled(reader);
    case MessageTypes::CameraMsg: return DeserializeCameraMsg(reader);
    case MessageTypes::TerrainPainterEnabled: return DeserializeTerrainPainterEnabled(reader);
    case MessageTypes::NewLight: return DeserializeNewLight(reader);
    case MessageTypes::GameObjectRenamed: return DeserializeGameObjectRenamed(reader);
    case MessageTypes::RemoveGameObjectInd: return DeserializeRemoveGameObjectInd(reader);
    case MessageTypes::Transform: return DeserializeTransform(reader);
    case MessageTypes::SceneStopedNotifMsg: return DeserializeSceneStopedNotifMsg(reader);
    case MessageTypes::ComponentDataMessage: return DeserializeComponentDataMessage(reader);
    case MessageTypes::SceneFileMsg: return DeserializeSceneFileMsg(reader);
    case MessageTypes::NewComponentMsgInd: return DeserializeNewComponentMsgInd(reader);
    case MessageTypes::SceneStartedNotifMsg: return DeserializeSceneStartedNotifMsg(reader);
    case MessageTypes::AvailableComponentMsgInd: return DeserializeAvailableComponentMsgInd(reader);
    case MessageTypes::NewGameObjectInd: return DeserializeNewGameObjectInd(reader);
    case MessageTypes::ReloadScene: return DeserializeReloadScene(reader);
    case MessageTypes::ExitResp: return DeserializeExitResp(reader);
    case MessageTypes::Param: return DeserializeParam(reader);
    case MessageTypes::ComponentDeleted: return DeserializeComponentDeleted(reader);
    case MessageTypes::GameObjectDeleted: return DeserializeGameObjectDeleted(reader);
    default: break;
    }
    return nullptr;
}
Network::IMessageData XmlMessageConverter::Convert(const Network::IMessage& message)
{
    auto type = DebugNetworkInterface::Convert(message.GetType());
    if (not type) return {};
    switch (*type)
    {
    case MessageTypes::LightDeleted:
    {
        auto msg = Network::castMessageAs<LightDeleted>(message);
        return Serialize(*msg);
    }
    case MessageTypes::RemoveComponentMsgInd:
    {
        auto msg = Network::castMessageAs<RemoveComponentMsgInd>(message);
        return Serialize(*msg);
    }
    case MessageTypes::SelectedObjectChanged:
    {
        auto msg = Network::castMessageAs<SelectedObjectChanged>(message);
        return Serialize(*msg);
    }
    case MessageTypes::PlantBrushEnabled:
    {
        auto msg = Network::castMessageAs<PlantBrushEnabled>(message);
        return Serialize(*msg);
    }
    case MessageTypes::CameraMsg:
    {
        auto msg = Network::castMessageAs<CameraMsg>(message);
        return Serialize(*msg);
    }
    case MessageTypes::TerrainPainterEnabled:
    {
        auto msg = Network::castMessageAs<TerrainPainterEnabled>(message);
        return Serialize(*msg);
    }
    case MessageTypes::NewLight:
    {
        auto msg = Network::castMessageAs<NewLight>(message);
        return Serialize(*msg);
    }
    case MessageTypes::GameObjectRenamed:
    {
        auto msg = Network::castMessageAs<GameObjectRenamed>(message);
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
    case MessageTypes::SceneStopedNotifMsg:
    {
        auto msg = Network::castMessageAs<SceneStopedNotifMsg>(message);
        return Serialize(*msg);
    }
    case MessageTypes::ComponentDataMessage:
    {
        auto msg = Network::castMessageAs<ComponentDataMessage>(message);
        return Serialize(*msg);
    }
    case MessageTypes::SceneFileMsg:
    {
        auto msg = Network::castMessageAs<SceneFileMsg>(message);
        return Serialize(*msg);
    }
    case MessageTypes::NewComponentMsgInd:
    {
        auto msg = Network::castMessageAs<NewComponentMsgInd>(message);
        return Serialize(*msg);
    }
    case MessageTypes::SceneStartedNotifMsg:
    {
        auto msg = Network::castMessageAs<SceneStartedNotifMsg>(message);
        return Serialize(*msg);
    }
    case MessageTypes::AvailableComponentMsgInd:
    {
        auto msg = Network::castMessageAs<AvailableComponentMsgInd>(message);
        return Serialize(*msg);
    }
    case MessageTypes::NewGameObjectInd:
    {
        auto msg = Network::castMessageAs<NewGameObjectInd>(message);
        return Serialize(*msg);
    }
    case MessageTypes::ReloadScene:
    {
        auto msg = Network::castMessageAs<ReloadScene>(message);
        return Serialize(*msg);
    }
    case MessageTypes::ExitResp:
    {
        auto msg = Network::castMessageAs<ExitResp>(message);
        return Serialize(*msg);
    }
    case MessageTypes::Param:
    {
        auto msg = Network::castMessageAs<Param>(message);
        return Serialize(*msg);
    }
    case MessageTypes::ComponentDeleted:
    {
        auto msg = Network::castMessageAs<ComponentDeleted>(message);
        return Serialize(*msg);
    }
    case MessageTypes::GameObjectDeleted:
    {
        auto msg = Network::castMessageAs<GameObjectDeleted>(message);
        return Serialize(*msg);
    }
    default: break;
    }
    return {};
}
} // namespace DebugNetworkInterface
} // namespace GameEngine
