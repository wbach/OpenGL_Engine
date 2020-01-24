#include "MessageTypes.h"

#define Return(x)  \
    if (type == x) \
    return (#x)

std::string Network::to_string(MessageTypes type)
{
    Return(Any);
    Return(Ping);
    Return(ConnectionMsg);
    Return(Authentication);
    Return(Text);
    return std::to_string(type);
}
