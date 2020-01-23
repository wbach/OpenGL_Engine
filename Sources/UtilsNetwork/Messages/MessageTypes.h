#pragma once
#include <Types.h>

namespace Network
{
enum MessageTypes
{
    Any = 251,
    Ping = 252,
    ConnectionMsg = 253,
    Authentication = 254,
    Text = 255
};
std::string to_string(uint8 type);
#define MESSAGE_TYPES_RANGE_LOW MessageTypes::Any
#define MESSAGE_TYPES_RANGE_HIGH MessageTypes::Text
}  // namespace Network
