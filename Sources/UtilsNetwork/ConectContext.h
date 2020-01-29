#pragma once
#include "Types.h"
#include "User.h"

#include <SDL2/SDL_net.h>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace Network
{
typedef std::unordered_map<uint32, std::shared_ptr<UtilsNetwork::UserData>> Users;

struct ConectContext
{
    Users users;
    uint32 maxClients;
    uint32 port;
    IPaddress serverIP;
    TCPsocket socket;
    SDLNet_SocketSet socketSet;
    std::string serverName;
    bool isServer_{false};

    ConectContext()
        : ConectContext(30, 1991)
    {
    }

    ConectContext(uint32 maxClients, uint32 port)
        : maxClients(maxClients)
        , port(port)
    {
    }
};
}  // namespace Network
