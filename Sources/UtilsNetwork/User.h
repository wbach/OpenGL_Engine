#pragma once
#include <SDL2/SDL_net.h>
#include "SDLNetWrapper.h"
#include "Types.h"
#include "optional.hpp"

namespace UtilsNetwork
{
struct UserData
{
    TCPsocket socket;
    uint32 id;
    wb::optional<Timepoint> connectionFailsStart;

    UserData()
        : socket(nullptr)
    {
    }
    uint32 GetNextId()
    {
        static uint32 s_id = 0;
        return s_id++;
    }
    virtual ~UserData()
    {
    }
};
}  // namespace UtilsNetwork