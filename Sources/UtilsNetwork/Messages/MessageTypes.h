#pragma once
#include "Types.h"

namespace Network
{
enum MessageTypes
{
    Any = 255,
    Ping = 254,
    ConnectionMsg = 253,
    Authentication = 252,
    Text = 251
};
std::string to_string(uint8 type);
}  // namespace Network
