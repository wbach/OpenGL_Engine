#pragma once
#include "Types.h"

namespace GameEngine
{
enum MessageTypes
{
    Any,
    GameObject,
};
std::string to_string(uint8 type);
#define NETWORK_INTERFACE_MESSAGE_TYPE_RANGE_LOW GameEngine::MessageTypes::Any
#define NETWORK_INTERFACE_TYPE_RANGE_HIGH GameEngine::MessageTypes::GameObject
}  // namespace Network
