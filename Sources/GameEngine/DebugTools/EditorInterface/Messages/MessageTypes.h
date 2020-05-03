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
    LightDeleted,
    RemoveComponentMsgInd,
    SelectedObjectChanged,
    PlantBrushEnabled,
    CameraMsg,
    TerrainPainterEnabled,
    NewLight,
    GameObjectRenamed,
    RemoveGameObjectInd,
    Transform,
    SceneStopedNotifMsg,
    ComponentDataMessage,
    SceneFileMsg,
    NewComponentMsgInd,
    SceneStartedNotifMsg,
    AvailableComponentMsgInd,
    NewGameObjectInd,
    ReloadScene,
    ExitResp,
    Param,
    ComponentDeleted,
    GameObjectDeleted
};
const uint8 MIN_VALUE{static_cast<uint8>(MessageTypes::Any)};
const uint8 MAX_VALUE{static_cast<uint8>(MessageTypes::GameObjectDeleted)};
Network::IMessageType Convert(MessageTypes);
std::optional<MessageTypes> Convert(Network::IMessageType);
GameEngine::DebugNetworkInterface::MessageTypes CreateFromString(const std::string&);
} // namespace DebugNetworkInterface
} // namespace GameEngine
namespace std
{
string to_string(GameEngine::DebugNetworkInterface::MessageTypes);
} // namespace std
