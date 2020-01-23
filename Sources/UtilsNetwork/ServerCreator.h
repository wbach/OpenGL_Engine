#pragma once
#include "NetworkCreator.h"
#include <optional>

namespace Network
{
class ServerCreator : public NetworkCreator
{
public:
    ServerCreator(ISDLNetWrapper &sdlNetWrapper);
    std::optional<ConectContext> Create(uint32 maxClients, uint32 port);
};
}  // namespace Network
