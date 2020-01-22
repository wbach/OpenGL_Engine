#pragma once
#include "NetworkCreator.h"

namespace Network
{
class ServerCreator : public NetworkCreator
{
public:
    ServerCreator(ISDLNetWrapper &sdlNetWrapper);
    ConectContext Create(uint32 maxClients, uint32 port);
};
}  // namespace Network
