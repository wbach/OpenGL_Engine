#pragma once
#include "Types.h"

namespace Network
{
enum class MessageFormat
{
    Ping,
    Binary,
    Protobuf,
    Json,
    Xml,
    Unknown
};

MessageFormat ConvertFormat(uint8);
uint8 ConvertFormat(MessageFormat);
}  // namespace Network

namespace std
{
std::string to_string(Network::MessageFormat type);
} // namespace std
