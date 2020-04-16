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
    if (type == "AvailableComponentMsgInd") return GameEngine::DebugNetworkInterface::MessageTypes::AvailableComponentMsgInd;
    if (type == "CameraMsg") return GameEngine::DebugNetworkInterface::MessageTypes::CameraMsg;
    if (type == "ComponentDataMessage") return GameEngine::DebugNetworkInterface::MessageTypes::ComponentDataMessage;
    if (type == "NewComponentMsgInd") return GameEngine::DebugNetworkInterface::MessageTypes::NewComponentMsgInd;
    if (type == "NewGameObjectInd") return GameEngine::DebugNetworkInterface::MessageTypes::NewGameObjectInd;
    if (type == "Param") return GameEngine::DebugNetworkInterface::MessageTypes::Param;
    if (type == "RemoveComponentMsgInd") return GameEngine::DebugNetworkInterface::MessageTypes::RemoveComponentMsgInd;
    if (type == "RemoveGameObjectInd") return GameEngine::DebugNetworkInterface::MessageTypes::RemoveGameObjectInd;
    if (type == "SelectedObjectChanged") return GameEngine::DebugNetworkInterface::MessageTypes::SelectedObjectChanged;
    if (type == "Transform") return GameEngine::DebugNetworkInterface::MessageTypes::Transform;
    return GameEngine::DebugNetworkInterface::MessageTypes::Any;
}
} // namespace DebugNetworkInterface
} // namespace GameEngine
namespace std
{
string to_string(GameEngine::DebugNetworkInterface::MessageTypes type)
{
    if (type == GameEngine::DebugNetworkInterface::MessageTypes::AvailableComponentMsgInd) return "AvailableComponentMsgInd";
    if (type == GameEngine::DebugNetworkInterface::MessageTypes::CameraMsg) return "CameraMsg";
    if (type == GameEngine::DebugNetworkInterface::MessageTypes::ComponentDataMessage) return "ComponentDataMessage";
    if (type == GameEngine::DebugNetworkInterface::MessageTypes::NewComponentMsgInd) return "NewComponentMsgInd";
    if (type == GameEngine::DebugNetworkInterface::MessageTypes::NewGameObjectInd) return "NewGameObjectInd";
    if (type == GameEngine::DebugNetworkInterface::MessageTypes::Param) return "Param";
    if (type == GameEngine::DebugNetworkInterface::MessageTypes::RemoveComponentMsgInd) return "RemoveComponentMsgInd";
    if (type == GameEngine::DebugNetworkInterface::MessageTypes::RemoveGameObjectInd) return "RemoveGameObjectInd";
    if (type == GameEngine::DebugNetworkInterface::MessageTypes::SelectedObjectChanged) return "SelectedObjectChanged";
    if (type == GameEngine::DebugNetworkInterface::MessageTypes::Transform) return "Transform";
    return "Unknown type";
}
} // namespace std
