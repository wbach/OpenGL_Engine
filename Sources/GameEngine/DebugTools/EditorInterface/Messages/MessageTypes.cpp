#include "MessageTypes.h"

namespace GameEngine
{
namespace DebugNetworkInterface
{
Network::IMessageType Convert(MessageTypes type)
{
    return static_cast<Network::IMessageType>(type);
}
std::optional<MessageTypes> Convert(Network::IMessageType type)
{
    return (type >= MIN_VALUE and MIN_VALUE <= MAX_VALUE) ? static_cast<MessageTypes>(type) : std::optional<MessageTypes>();
}
GameEngine::DebugNetworkInterface::MessageTypes CreateFromString(const std::string& type)
{
    if (type == "LightDeleted") return GameEngine::DebugNetworkInterface::MessageTypes::LightDeleted;
    if (type == "RemoveComponentMsgInd") return GameEngine::DebugNetworkInterface::MessageTypes::RemoveComponentMsgInd;
    if (type == "SelectedObjectChanged") return GameEngine::DebugNetworkInterface::MessageTypes::SelectedObjectChanged;
    if (type == "PlantBrushEnabled") return GameEngine::DebugNetworkInterface::MessageTypes::PlantBrushEnabled;
    if (type == "CameraMsg") return GameEngine::DebugNetworkInterface::MessageTypes::CameraMsg;
    if (type == "TerrainPainterEnabled") return GameEngine::DebugNetworkInterface::MessageTypes::TerrainPainterEnabled;
    if (type == "NewLight") return GameEngine::DebugNetworkInterface::MessageTypes::NewLight;
    if (type == "GameObjectRenamed") return GameEngine::DebugNetworkInterface::MessageTypes::GameObjectRenamed;
    if (type == "RemoveGameObjectInd") return GameEngine::DebugNetworkInterface::MessageTypes::RemoveGameObjectInd;
    if (type == "Transform") return GameEngine::DebugNetworkInterface::MessageTypes::Transform;
    if (type == "SceneStopedNotifMsg") return GameEngine::DebugNetworkInterface::MessageTypes::SceneStopedNotifMsg;
    if (type == "ComponentDataMessage") return GameEngine::DebugNetworkInterface::MessageTypes::ComponentDataMessage;
    if (type == "SceneFileMsg") return GameEngine::DebugNetworkInterface::MessageTypes::SceneFileMsg;
    if (type == "NewComponentMsgInd") return GameEngine::DebugNetworkInterface::MessageTypes::NewComponentMsgInd;
    if (type == "SceneStartedNotifMsg") return GameEngine::DebugNetworkInterface::MessageTypes::SceneStartedNotifMsg;
    if (type == "AvailableComponentMsgInd") return GameEngine::DebugNetworkInterface::MessageTypes::AvailableComponentMsgInd;
    if (type == "NewGameObjectInd") return GameEngine::DebugNetworkInterface::MessageTypes::NewGameObjectInd;
    if (type == "ReloadScene") return GameEngine::DebugNetworkInterface::MessageTypes::ReloadScene;
    if (type == "ExitResp") return GameEngine::DebugNetworkInterface::MessageTypes::ExitResp;
    if (type == "Param") return GameEngine::DebugNetworkInterface::MessageTypes::Param;
    if (type == "ComponentDeleted") return GameEngine::DebugNetworkInterface::MessageTypes::ComponentDeleted;
    if (type == "GameObjectDeleted") return GameEngine::DebugNetworkInterface::MessageTypes::GameObjectDeleted;
    return GameEngine::DebugNetworkInterface::MessageTypes::Any;
}
} // namespace DebugNetworkInterface
} // namespace GameEngine
namespace std
{
string to_string(GameEngine::DebugNetworkInterface::MessageTypes type)
{
    if (type == GameEngine::DebugNetworkInterface::MessageTypes::LightDeleted) return "LightDeleted";
    if (type == GameEngine::DebugNetworkInterface::MessageTypes::RemoveComponentMsgInd) return "RemoveComponentMsgInd";
    if (type == GameEngine::DebugNetworkInterface::MessageTypes::SelectedObjectChanged) return "SelectedObjectChanged";
    if (type == GameEngine::DebugNetworkInterface::MessageTypes::PlantBrushEnabled) return "PlantBrushEnabled";
    if (type == GameEngine::DebugNetworkInterface::MessageTypes::CameraMsg) return "CameraMsg";
    if (type == GameEngine::DebugNetworkInterface::MessageTypes::TerrainPainterEnabled) return "TerrainPainterEnabled";
    if (type == GameEngine::DebugNetworkInterface::MessageTypes::NewLight) return "NewLight";
    if (type == GameEngine::DebugNetworkInterface::MessageTypes::GameObjectRenamed) return "GameObjectRenamed";
    if (type == GameEngine::DebugNetworkInterface::MessageTypes::RemoveGameObjectInd) return "RemoveGameObjectInd";
    if (type == GameEngine::DebugNetworkInterface::MessageTypes::Transform) return "Transform";
    if (type == GameEngine::DebugNetworkInterface::MessageTypes::SceneStopedNotifMsg) return "SceneStopedNotifMsg";
    if (type == GameEngine::DebugNetworkInterface::MessageTypes::ComponentDataMessage) return "ComponentDataMessage";
    if (type == GameEngine::DebugNetworkInterface::MessageTypes::SceneFileMsg) return "SceneFileMsg";
    if (type == GameEngine::DebugNetworkInterface::MessageTypes::NewComponentMsgInd) return "NewComponentMsgInd";
    if (type == GameEngine::DebugNetworkInterface::MessageTypes::SceneStartedNotifMsg) return "SceneStartedNotifMsg";
    if (type == GameEngine::DebugNetworkInterface::MessageTypes::AvailableComponentMsgInd) return "AvailableComponentMsgInd";
    if (type == GameEngine::DebugNetworkInterface::MessageTypes::NewGameObjectInd) return "NewGameObjectInd";
    if (type == GameEngine::DebugNetworkInterface::MessageTypes::ReloadScene) return "ReloadScene";
    if (type == GameEngine::DebugNetworkInterface::MessageTypes::ExitResp) return "ExitResp";
    if (type == GameEngine::DebugNetworkInterface::MessageTypes::Param) return "Param";
    if (type == GameEngine::DebugNetworkInterface::MessageTypes::ComponentDeleted) return "ComponentDeleted";
    if (type == GameEngine::DebugNetworkInterface::MessageTypes::GameObjectDeleted) return "GameObjectDeleted";
    return "Unknown type";
}
} // namespace std
