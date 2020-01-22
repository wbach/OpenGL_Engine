#include "MessageFormat.h"
#include <Logger/Log.h>

#define Return(x)  \
    if (type == x) \
    return (#x)

namespace std
{
std::string to_string(Network::MessageFormat type)
{
    Return(Network::MessageFormat::Ping);
    Return(Network::MessageFormat::Binary);
    Return(Network::MessageFormat::Protobuf);
    Return(Network::MessageFormat::Json);
    Return(Network::MessageFormat::Xml);
    Return(Network::MessageFormat::Unknown);

     return "Unknown message type : " + std::to_string(static_cast<int>(type));
}
}  // namespace std

#define ReturnType(x)                 \
    if (value == static_cast<uint8>(x)) \
        return x;

Network::MessageFormat Network::ConvertFormat(uint8 value)
{
    ReturnType(Network::MessageFormat::Ping);
    ReturnType(Network::MessageFormat::Binary);
    ReturnType(Network::MessageFormat::Protobuf);
    ReturnType(Network::MessageFormat::Json);
    ReturnType(Network::MessageFormat::Xml);

    return Network::MessageFormat::Unknown;
}

uint8 Network::ConvertFormat(Network::MessageFormat format)
{
    return static_cast<uint8>(format);
}
