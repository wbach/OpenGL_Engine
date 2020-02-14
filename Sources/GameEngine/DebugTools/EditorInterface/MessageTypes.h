#pragma once
#include "Types.h"

namespace GameEngine
{
enum class MessageTypes
{
    Any,
    NewGameObjectInd,
    RemoveGameObjectInd,
    Transform,
    AvailableComponentMsgInd,
    NewComponentMsgInd,
    RemoveComponentMsgInd
};
std::string to_string(MessageTypes type);
#define NETWORK_INTERFACE_MESSAGE_TYPE_RANGE_LOW GameEngine::MessageTypes::Any
#define NETWORK_INTERFACE_TYPE_RANGE_HIGH GameEngine::MessageTypes::RemoveComponentMsgInd
}  // namespace Network
