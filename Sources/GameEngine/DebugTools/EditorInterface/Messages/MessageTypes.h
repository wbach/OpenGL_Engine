#pragma once
#include <Types.h>
#include <optional>
#include <UtilsNetwork/IMessage.h>

namespace GameEngine
{
namespace DebugNetworkInterface
{
enum class MessageTypes
{
    Any,
    AvailableComponentMsgInd,
    CameraMsg,
    ComponentDataMessage,
    ComponentDeleted,
    ExitResp,
    GameObjectDeleted,
    GameObjectRenamed,
    LightDeleted,
    NewComponentMsgInd,
    NewGameObjectInd,
    NewLight,
    Param,
    PlantBrushEnabled,
    ReloadScene,
    RemoveComponentMsgInd,
    RemoveGameObjectInd,
    SceneFileMsg,
    SceneStartedNotifMsg,
    SceneStopedNotifMsg,
    SelectedObjectChanged,
    TerrainPainterEnabled,
    Transform
};
const uint8 MIN_VALUE{static_cast<uint8>(MessageTypes::Any)};
const uint8 MAX_VALUE{static_cast<uint8>(MessageTypes::Transform)};
Network::IMessageType Convert(MessageTypes);
std::optional<MessageTypes> Convert(Network::IMessageType);
GameEngine::DebugNetworkInterface::MessageTypes CreateFromString(const std::string&);
} // namespace DebugNetworkInterface
} // namespace GameEngine
namespace std
{
string to_string(GameEngine::DebugNetworkInterface::MessageTypes);
} // namespace std
